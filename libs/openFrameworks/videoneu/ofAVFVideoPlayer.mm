#include "ofAVFVideoPlayer.h"
#include "ofLog.h"
#include "ofUtils.h"

#include "ofFileUtils.h"
#include <unistd.h>

#import <AVFoundation/AVFoundation.h>
#import <CoreVideo/CoreVideo.h>
#import <Accelerate/Accelerate.h>

// MARK: - Objective-C Player Interface

@interface OFAVFPlayer : NSObject

@property (nonatomic, readonly) AVPlayer *player;
@property (nonatomic, readonly) AVPlayerItemVideoOutput *videoOutput;
@property (nonatomic, readonly) BOOL isReady;
@property (nonatomic, readonly) BOOL isLoaded;
@property (nonatomic, readonly) BOOL isPlaying;
@property (nonatomic, readonly) BOOL hasNewFrame;
@property (nonatomic, readonly) CGSize videoSize;
@property (nonatomic, readonly) CGFloat frameRate;
@property (nonatomic, readonly) CMTime duration;
@property (nonatomic, assign) BOOL useYUV;
@property (nonatomic, readonly) NSInteger loopState;

- (BOOL)loadWithPath:(const fs::path&)path async:(BOOL)async;
- (void)unload;
- (void)play;
- (void)pause;
- (void)stop;
- (void)seekToPosition:(float)position;
- (void)seekToTime:(CMTime)time;
- (void)seekToFrame:(int)frame;
- (CVPixelBufferRef)copyCurrentFrame;
- (void)setupVideoOutputWithYUV:(BOOL)useYUV;

@end

// MARK: - Implementation

@implementation OFAVFPlayer {
    AVPlayerItem *_playerItem;
    id _timeObserver;
    id _itemEndObserver;

    BOOL _isStream;
    ofLoopType _loopState;
    float _speed;
    float _volume;
    BOOL _frameReady;
    BOOL _wasPlayingBeforeSeek;
    CMTime _currentFrameTime;
}

#pragma mark - Lifecycle

- (instancetype)init {
    self = [super init];
    if (self) {
        _player = [[AVPlayer alloc] init];
        _speed = 1.0f;
        _volume = 1.0f;
        _loopState = OF_LOOP_NORMAL;
        [self setupVideoOutput];
    }
    return self;
}

- (void)dealloc {
    [self unload];
}

- (void)setupVideoOutput {
    [self setupVideoOutputWithYUV:NO];
}

- (void)setupVideoOutputWithYUV:(BOOL)useYUV {
    _useYUV = useYUV;

    NSDictionary *attributes;
    if (useYUV) {
        // Use bi-planar YUV 4:2:0 - significantly less bandwidth
        attributes = @{
            (id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_420YpCbCr8BiPlanarVideoRange),
            (id)kCVPixelBufferOpenGLCompatibilityKey: @YES
        };
    } else {
        // Standard BGRA
        attributes = @{
            (id)kCVPixelBufferPixelFormatTypeKey: @(kCVPixelFormatType_32BGRA),
            (id)kCVPixelBufferOpenGLCompatibilityKey: @YES
        };
    }

    _videoOutput = [[AVPlayerItemVideoOutput alloc] initWithPixelBufferAttributes:attributes];
    _videoOutput.suppressesPlayerRendering = YES;
}

#pragma mark - Loading

- (BOOL)loadWithPath:(const fs::path&)path async:(BOOL)async {
    [self unload];

    NSURL *url = nil;
    std::string pathStr = ofPathToString(path);

    // Detect streams
    _isStream = (pathStr.find("http://") == 0 ||
                 pathStr.find("https://") == 0 ||
                 pathStr.find("rtsp://") == 0);

    if (_isStream) {
        url = [NSURL URLWithString:[NSString stringWithUTF8String:pathStr.c_str()]];
    } else {
        NSString *localPath = [NSString stringWithUTF8String:ofToDataPath(path).c_str()];
        url = [NSURL fileURLWithPath:localPath];
    }

    if (!url) {
        ofLogError("ofAVFVideoPlayer") << "Invalid URL for path: " << path;
        return NO;
    }

    NSDictionary *options = @{(id)AVURLAssetPreferPreciseDurationAndTimingKey: @YES};
    AVURLAsset *asset = [AVURLAsset URLAssetWithURL:url options:options];

    if (!asset) {
        ofLogError("ofAVFVideoPlayer") << "Failed to create asset for: " << path;
        return NO;
    }

    // Always load asynchronously to avoid blocking main thread
    dispatch_async(dispatch_get_global_queue(QOS_CLASS_USER_INITIATED, 0), ^{
        [asset loadValuesAsynchronouslyForKeys:@[@"tracks", @"duration"] completionHandler:^{
            dispatch_async(dispatch_get_main_queue(), ^{
                [self setupAsset:asset];
            });
        }];
    });

    // Return YES to indicate load was initiated
    // Actual success determined by isLoaded() check later
    return YES;
}

- (void)setupAsset:(AVURLAsset *)asset {
    NSError *error = nil;

    if ([asset statusOfValueForKey:@"tracks" error:&error] != AVKeyValueStatusLoaded) {
        ofLogError("ofAVFVideoPlayer") << "Failed to load tracks: "
            << [[error localizedDescription] UTF8String];
        return;
    }

    _duration = asset.duration;

    // Get video track info - using deprecated API for compatibility
    // TODO: Migrate to loadTracksWithMediaType:completionHandler: for macOS 15+
    AVAssetTrack *videoTrack = nil;
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wdeprecated-declarations"
    videoTrack = [[asset tracksWithMediaType:AVMediaTypeVideo] firstObject];
    #pragma clang diagnostic pop

    if (videoTrack && !_isStream) {
        _videoSize = videoTrack.naturalSize;
        _frameRate = videoTrack.nominalFrameRate;
    }

    // Create player item
    _playerItem = [AVPlayerItem playerItemWithAsset:asset];
    [_playerItem addOutput:_videoOutput];

    // Setup end observation
    __weak OFAVFPlayer *weakSelf = self;
    _itemEndObserver = [[NSNotificationCenter defaultCenter]
        addObserverForName:AVPlayerItemDidPlayToEndTimeNotification
                    object:_playerItem
                     queue:[NSOperationQueue mainQueue]
                usingBlock:^(NSNotification * /*note*/) {
                    [weakSelf handlePlaybackEnded];
                }];

    [_player replaceCurrentItemWithPlayerItem:_playerItem];

    _isLoaded = YES;
    _isReady = YES;
    _frameReady = NO;

    // Apply settings
    [_player setRate:_speed];
    _player.volume = _volume;

    ofLogNotice("ofAVFVideoPlayer") << "Loaded " << (int)_videoSize.width << "x"
        << (int)_videoSize.height << " @ " << _frameRate << "fps";
}

- (void)unload {
    [self stop];

    if (_itemEndObserver) {
        [[NSNotificationCenter defaultCenter] removeObserver:_itemEndObserver];
        _itemEndObserver = nil;
    }

    if (_timeObserver) {
        [_player removeTimeObserver:_timeObserver];
        _timeObserver = nil;
    }

    [_player replaceCurrentItemWithPlayerItem:nil];
    _playerItem = nil;

    _isLoaded = NO;
    _isReady = NO;
    _isPlaying = NO;
}

#pragma mark - Playback Control

- (void)play {
    if (!_isLoaded) return;
    [_player play];
    [_player setRate:_speed];
    _isPlaying = YES;
}

- (void)pause {
    [_player pause];
    _isPlaying = NO;
}

- (void)stop {
    [self pause];
    [self seekToTime:kCMTimeZero];
}

- (void)seekToPosition:(float)position {
    if (!_isLoaded) return;
    CMTime time = CMTimeMultiplyByFloat64(_duration, position);
    [self seekToTime:time];
}

- (void)seekToTime:(CMTime)time {
    if (!_isLoaded) return;

    _wasPlayingBeforeSeek = _isPlaying;
    if (_isPlaying) [_player pause];

    [_playerItem seekToTime:time completionHandler:^(BOOL /*finished*/) {
        if (self->_wasPlayingBeforeSeek) {
            [self play];
        }
    }];
}

- (void)seekToFrame:(int)frame {
    if (_frameRate <= 0) return;
    float position = frame / (_frameRate * CMTimeGetSeconds(_duration));
    [self seekToPosition:position];
}

- (void)handlePlaybackEnded {
    switch (_loopState) {
        case OF_LOOP_NORMAL:
            [self seekToTime:kCMTimeZero];
            [self play];
            break;
        case OF_LOOP_PALINDROME:
            _speed = -_speed;
            [_player setRate:_speed];
            break;
        case OF_LOOP_NONE:
        default:
            _isPlaying = NO;
            break;
    }
}

#pragma mark - Frame Access

- (CVPixelBufferRef)copyCurrentFrame {
    if (!_isReady) return nil;

    CMTime itemTime = [_videoOutput itemTimeForHostTime:CACurrentMediaTime()];

    if (![_videoOutput hasNewPixelBufferForItemTime:itemTime]) {
        return nil;
    }

    _hasNewFrame = YES;
    _currentFrameTime = itemTime;

    return [_videoOutput copyPixelBufferForItemTime:itemTime itemTimeForDisplay:nil];
}

#pragma mark - Properties

- (void)setVolume:(float)volume {
    _volume = volume;
    _player.volume = volume;
}

- (void)setSpeed:(float)speed {
    _speed = speed;
    if (_isPlaying) {
        [_player setRate:speed];
    }
}

- (void)setLoopState:(ofLoopType)state {
    _loopState = state;
}

- (NSInteger)loopState {
    return _loopState;
}

@end

// MARK: - C++ Implementation

class ofAVFVideoPlayer::Impl {
public:
    OFAVFPlayer *player = nil;
    ofPixels pixels;
    ofTexture texture;
    std::unique_ptr<ofAVFYUVRenderer> yuvRenderer;
    ofPixelFormat pixelFormat = OF_PIXELS_RGBA;
    bool bFrameNew = false;
    bool bUpdatePixels = false;
    bool bUpdateTexture = true;
    bool bTextureCache = true;
    bool bUseYUV = false;

#if defined(TARGET_OSX)
    CVOpenGLTextureCacheRef textureCache = nullptr;
    CVOpenGLTextureRef cvTexture = nullptr;
#elif defined(TARGET_OF_IOS)
    CVOpenGLESTextureCacheRef textureCache = nullptr;
    CVOpenGLESTextureRef cvTexture = nullptr;
#endif
};

ofAVFVideoPlayer::ofAVFVideoPlayer()
    : pImpl(std::make_unique<Impl>()) {

    pImpl->player = [[OFAVFPlayer alloc] init];
    pImpl->bTextureCache = ofGetUsingArbTex();
}

ofAVFVideoPlayer::~ofAVFVideoPlayer() {
    close();
    pImpl->player = nil;
}

ofAVFVideoPlayer::ofAVFVideoPlayer(ofAVFVideoPlayer&& other) noexcept
    : pImpl(std::move(other.pImpl)) {
}

ofAVFVideoPlayer& ofAVFVideoPlayer::operator=(ofAVFVideoPlayer&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
    }
    return *this;
}

// MARK: Loading

bool ofAVFVideoPlayer::load(const fs::path& fileName) {
    close();

    // Initiate load (async)
    BOOL initiated = [pImpl->player loadWithPath:fileName async:NO];
    if (!initiated) {
        return false;
    }

    // Short wait for fast local files - don't block window creation
    // For truly async loading without any blocking, use loadAsync() instead
    int attempts = 0;
    const int maxAttempts = 20; // 20 * 16ms = ~320ms max blocking
    while (![pImpl->player isLoaded] && attempts < maxAttempts) {
        usleep(16000); // 16ms ~ 1 frame at 60fps
        attempts++;
        // Process any pending main queue items
        [[NSRunLoop currentRunLoop] runMode:NSDefaultRunLoopMode beforeDate:[NSDate dateWithTimeIntervalSinceNow:0.01]];
    }

    if (![pImpl->player isLoaded]) {
        // Still loading - window can now show, video will appear when ready
        ofLogVerbose("ofAVFVideoPlayer") << "Load continues in background: " << fileName;
        return true; // Return true since load was initiated successfully
    }

    // Force first frame ready
    update();
    return true;
}

void ofAVFVideoPlayer::loadAsync(const fs::path& fileName) {
    close();
    [pImpl->player loadWithPath:fileName async:YES];
}

void ofAVFVideoPlayer::close() {
    [pImpl->player unload];
    pImpl->pixels.clear();
    pImpl->texture.clear();
    pImpl->bFrameNew = false;
}

// MARK: Playback

void ofAVFVideoPlayer::play() {
    [pImpl->player play];
}

void ofAVFVideoPlayer::stop() {
    [pImpl->player stop];
}

void ofAVFVideoPlayer::setPaused(bool bPause) {
    if (bPause) {
        [pImpl->player pause];
    } else {
        play();
    }
}

void ofAVFVideoPlayer::setPosition(float pct) {
    [pImpl->player seekToPosition:pct];
}

void ofAVFVideoPlayer::setSpeed(float speed) {
    [pImpl->player setSpeed:speed];
}

void ofAVFVideoPlayer::setVolume(float volume) {
    [pImpl->player setVolume:volume];
}

void ofAVFVideoPlayer::setLoopState(ofLoopType state) {
    [pImpl->player setLoopState:state];
}

void ofAVFVideoPlayer::setFrame(int frame) {
    [pImpl->player seekToFrame:frame];
}

// MARK: Update

void ofAVFVideoPlayer::update() {
    pImpl->bFrameNew = NO;

    if (!pImpl->player.isReady) {
        return;
    }

    int w = (int)pImpl->player.videoSize.width;
    int h = (int)pImpl->player.videoSize.height;

    // Allocate resources immediately when dimensions are known
    if (w > 0 && h > 0) {
        if (pImpl->bUseYUV) {
            if (!pImpl->yuvRenderer || !pImpl->yuvRenderer->isAllocated()) {
                pImpl->yuvRenderer = std::make_unique<ofAVFYUVRenderer>();
                if (!pImpl->yuvRenderer->setup(w, h)) {
                    ofLogError("ofAVFVideoPlayer") << "Failed to setup YUV renderer";
                    pImpl->yuvRenderer.reset();
                    pImpl->bUseYUV = false; // Fall back to RGB
                }
            }
        } else if (!pImpl->texture.isAllocated()) {
            pImpl->texture.allocate(w, h, GL_RGBA);
            pImpl->pixels.allocate(w, h, pImpl->pixelFormat);
        }
    }

    // Get new frame
    CVPixelBufferRef pixelBuffer = [pImpl->player copyCurrentFrame];
    if (!pixelBuffer) {
        return;
    }

    pImpl->bFrameNew = YES;

    // Handle YUV format (bi-planar)
    if (pImpl->bUseYUV && CVPixelBufferIsPlanar(pixelBuffer)) {
        CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);

        // Get Y plane
        size_t planeCount = CVPixelBufferGetPlaneCount(pixelBuffer);
        if (planeCount >= 2) {
            uint8_t* yPlane = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 0);
            uint8_t* uvPlane = (uint8_t*)CVPixelBufferGetBaseAddressOfPlane(pixelBuffer, 1);
            size_t yStride = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 0);
            size_t uvStride = CVPixelBufferGetBytesPerRowOfPlane(pixelBuffer, 1);

            if (yPlane && uvPlane && pImpl->yuvRenderer) {
                pImpl->yuvRenderer->uploadYUV(yPlane, (int)yStride, uvPlane, (int)uvStride);
            }
        }

        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }
    // Handle packed formats (BGRA, etc)
    else if (!pImpl->bUseYUV && pImpl->bUpdateTexture) {
        int bufW = (int)CVPixelBufferGetWidth(pixelBuffer);
        int bufH = (int)CVPixelBufferGetHeight(pixelBuffer);

        CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);

        void* baseAddress = CVPixelBufferGetBaseAddress(pixelBuffer);
        if (baseAddress) {
            pImpl->texture.loadData((const uint8_t*)baseAddress, bufW, bufH, GL_BGRA);
        }

        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }

    // Copy to pixels if needed (only for RGB path)
    if (!pImpl->bUseYUV && pImpl->bUpdatePixels) {
        int bufW = (int)CVPixelBufferGetWidth(pixelBuffer);
        int bufH = (int)CVPixelBufferGetHeight(pixelBuffer);

        if (!pImpl->pixels.isAllocated() ||
            (int)pImpl->pixels.getWidth() != bufW ||
            (int)pImpl->pixels.getHeight() != bufH) {
            pImpl->pixels.allocate(bufW, bufH, OF_PIXELS_RGBA);
        }

        CVPixelBufferLockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);

        void* src = CVPixelBufferGetBaseAddress(pixelBuffer);
        size_t srcBytesPerRow = CVPixelBufferGetBytesPerRow(pixelBuffer);

        if (src && srcBytesPerRow == pImpl->pixels.getBytesStride()) {
            memcpy(pImpl->pixels.getData(), src, bufH * srcBytesPerRow);
        } else if (src) {
            // Row by row copy for stride mismatch
            for (int y = 0; y < bufH; y++) {
                memcpy(pImpl->pixels.getData() + y * bufW * 4,
                       (uint8_t*)src + y * srcBytesPerRow,
                       bufW * 4);
            }
        }

        CVPixelBufferUnlockBaseAddress(pixelBuffer, kCVPixelBufferLock_ReadOnly);
    }

    CFRelease(pixelBuffer);
}

// MARK: Queries

bool ofAVFVideoPlayer::isFrameNew() const {
    return pImpl->bFrameNew;
}

bool ofAVFVideoPlayer::isLoaded() const {
    return pImpl->player.isLoaded;
}

bool ofAVFVideoPlayer::isPlaying() const {
    return pImpl->player.isPlaying;
}

bool ofAVFVideoPlayer::isPaused() const {
    return pImpl->player.isLoaded && !pImpl->player.isPlaying;
}

float ofAVFVideoPlayer::getPosition() const {
    if (!pImpl->player.isLoaded) return 0;
    CMTime current = pImpl->player.player.currentTime;
    CMTime duration = pImpl->player.duration;
    if (CMTimeCompare(duration, kCMTimeZero) == 0) return 0;
    return CMTimeGetSeconds(current) / CMTimeGetSeconds(duration);
}

float ofAVFVideoPlayer::getSpeed() const {
    return pImpl->player.player.rate;
}

float ofAVFVideoPlayer::getDuration() const {
    return CMTimeGetSeconds(pImpl->player.duration);
}

float ofAVFVideoPlayer::getWidth() const {
    return pImpl->player.videoSize.width;
}

float ofAVFVideoPlayer::getHeight() const {
    return pImpl->player.videoSize.height;
}

int ofAVFVideoPlayer::getCurrentFrame() const {
    float fps = pImpl->player.frameRate;
    if (fps <= 0) return 0;
    return (int)(getPosition() * getDuration() * fps);
}

int ofAVFVideoPlayer::getTotalNumFrames() const {
    float fps = pImpl->player.frameRate;
    if (fps <= 0) return 0;
    return (int)(getDuration() * fps);
}

ofLoopType ofAVFVideoPlayer::getLoopState() const {
    if (pImpl->player) {
        return (ofLoopType)pImpl->player.loopState;
    }
    return OF_LOOP_NONE;
}

// MARK: Frame Stepping

void ofAVFVideoPlayer::firstFrame() {
    setPosition(0);
}

void ofAVFVideoPlayer::nextFrame() {
    setFrame(getCurrentFrame() + 1);
}

void ofAVFVideoPlayer::previousFrame() {
    setFrame(getCurrentFrame() - 1);
}

// MARK: Pixel/Texture Access

bool ofAVFVideoPlayer::setPixelFormat(ofPixelFormat pixelFormat) {
    // Support RGBA (default) and YUV (via YUV renderer)
    if (pixelFormat == OF_PIXELS_RGBA) {
        pImpl->pixelFormat = pixelFormat;
        pImpl->bUseYUV = false;
        [pImpl->player setupVideoOutputWithYUV:NO];
        return true;
    }
    if (pixelFormat == OF_PIXELS_YUY2 || pixelFormat == OF_PIXELS_NV12 ||
        pixelFormat == OF_PIXELS_I420 || pixelFormat == OF_PIXELS_YV12) {
        pImpl->pixelFormat = pixelFormat;
        pImpl->bUseYUV = true;
        [pImpl->player setupVideoOutputWithYUV:YES];
        return true;
    }
    return false;
}

ofPixelFormat ofAVFVideoPlayer::getPixelFormat() const {
    return pImpl->pixelFormat;
}

ofPixels& ofAVFVideoPlayer::getPixels() {
    pImpl->bUpdatePixels = true;
    return pImpl->pixels;
}

const ofPixels& ofAVFVideoPlayer::getPixels() const {
    return pImpl->pixels;
}

ofTexture* ofAVFVideoPlayer::getTexturePtr() {
    if (pImpl->bUseYUV && pImpl->yuvRenderer) {
        // YUV renderer doesn't have a single output texture yet
        // Users should use draw() or the YUV renderer directly
        return nullptr;
    }
    return &pImpl->texture;
}

void ofAVFVideoPlayer::draw(float x, float y) const {
    draw(x, y, getWidth(), getHeight());
}

void ofAVFVideoPlayer::draw(float x, float y, float w, float h) const {
    if (pImpl->bUseYUV && pImpl->yuvRenderer) {
        pImpl->yuvRenderer->draw(x, y, w, h);
    } else if (pImpl->texture.isAllocated()) {
        pImpl->texture.draw(x, y, w, h);
    }
}
