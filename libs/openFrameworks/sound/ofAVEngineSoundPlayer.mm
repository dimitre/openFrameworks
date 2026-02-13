//
//  ofAVEngineSoundPlayer.cpp
//  soundPlayerExample
//
//  Created by Theo Watson on 3/24/21.
//  Modified by Dan Rosser 9/5/22

#include "ofAVEngineSoundPlayer.h"
#include "ofAppRunner.h" // ofToDataPath

#ifdef OF_SOUND_PLAYER_AV_ENGINE

#include "ofLog.h"
#include "ofEvents.h"

//#ifdef __OBJC__
//	#import <Foundation/Foundation.h>
//	#import <AVFoundation/AVFoundation.h>
//	#import <Accelerate/Accelerate.h>
//#endif


//REFS: https://github.com/ooper-shlab/AVAEMixerSample-Swift/blob/master/AVAEMixerSample/AudioEngine.m
// https://developer.apple.com/documentation/avfaudio/avaudioengine
// https://developer.apple.com/forums/thread/14138
// https://developer.apple.com/forums/thread/48442
// https://github.com/garynewby/XYAudioView/blob/master/XYAudioView/BasicAVAudioEngine.m
// https://github.com/twilio/video-quickstart-ios/blob/master/AudioDeviceExample/AudioDevices/ExampleAVAudioEngineDevice.m

// static BOOL audioSessionSetup = NO;
static AVAudioEngine * _engine = nullptr;

static NSString *kShouldEnginePauseNotification = @"kShouldEnginePauseNotification";

@interface AVEnginePlayer : NSObject

@property(nonatomic, retain) NSTimer * timer;

- (BOOL)loadWithFile:(NSString*)file;
- (BOOL)loadWithPath:(NSString*)path;
- (BOOL)loadWithURL:(NSURL*)url;
- (BOOL)loadWithSoundFile:(AVAudioFile*)aSoundFile;

- (void)unloadSound;

- (void)play;
- (void)play: (float)startTime;
- (void)pause;
- (void)stop;

- (BOOL)isLoaded;
- (BOOL)isPlaying;

- (void)volume:(float)value;
- (float)volume;

- (void)pan:(float)value;
- (float)pan;

- (void)speed:(float)value;
- (float)speed;

- (void)loop:(BOOL)value;
- (BOOL)loop;

- (void)multiPlay:(BOOL)value;
- (BOOL)multiPlay;

- (void)position:(float)value;
- (float)position;

- (void)positionMs:(int)value;
- (int)positionMs;

- (float)positionSeconds;
- (float)soundDurationSeconds;


- (void)sessionInterupted;

- (AVAudioFile *)getSoundFile;

- (AVAudioEngine *)engine;

- (void)beginInterruption; /* something has caused your audio session to be interrupted */

- (void)endInterruption; /* endInterruptionWithFlags: will be called instead if implemented. */

/* notification for input become available or unavailable */
- (void)inputIsAvailableChanged:(BOOL)isInputAvailable;

@end


@interface AVEnginePlayer ()

//@property(nonatomic, strong) AVAudioEngine *engine;
@property(nonatomic, strong) AVAudioMixerNode *mainMixer;
@property(nonatomic, strong) AVAudioUnitVarispeed *variSpeed;
@property(nonatomic, strong) AVAudioPlayerNode *soundPlayer;
@property(nonatomic, strong) AVAudioFile *soundFile;
@property(nonatomic, assign) bool mShouldLoop;
@property(nonatomic, assign) BOOL bInterruptedWhileRunning;
@property(nonatomic, assign) bool bIsPlaying;
@property(nonatomic, assign) int mGaurdCount;
@property(nonatomic, assign) int mRestorePlayCount;
@property(nonatomic, assign) bool mMultiPlay;
@property(nonatomic, assign) bool isSessionInterrupted;
@property(nonatomic, assign) bool isConfigChangePending;
@property(nonatomic, assign) float mRequestedPositonSeconds;
@property(nonatomic, assign) AVAudioFramePosition startedSampleOffset;

@property(nonatomic, assign) bool mPlayingAtInterruption;
@property(nonatomic, assign) float mPositonSecondsAtInterruption;

@property(nonatomic, assign) BOOL resetAudioEngine;

@end

@implementation AVEnginePlayer

@synthesize timer;

- (void)beginInterruption {
	// Handle audio session interruption begin
}

- (void)endInterruption {
	// Handle audio session interruption end
}

- (AVAudioEngine *) engine {

  if( _engine == nullptr ){
	@autoreleasepool {
		_engine = [[AVAudioEngine alloc] init];
	}
	self.resetAudioEngine = NO;
  }

  return _engine;
}


- (void) engineReconnect {
	NSLog(@"engineReconnect");

	if( [self engine] != nil && [[self engine] isRunning] ){
		NSLog(@"engineReconnect isRunning");
	} else {
		NSLog(@"engineReconnect is NOT Running");
	}
	if([self engine]) {
		BOOL found = NO;
		for(AVAudioPlayerNode* node in [self engine].attachedNodes) {
			if(node == self.soundPlayer) {
				break;
			}
		}
		if(found) {
			NSLog(@"engineReconnect found Node AVAudioPlayerNode - Disconnecting");
			[[self engine] detachNode:self.soundPlayer];
		}
		found = NO;
		for(AVAudioUnitVarispeed* node in [self engine].attachedNodes) {
			if(node == self.variSpeed) {
				break;
			}
		}
		if(found) {
			NSLog(@"engineReconnect found Node  variSpeed- Disconnecting");
			[[self engine] detachNode:self.variSpeed];
		}
	}
}

- (void) engineReset {
	if( [self engine] != nil && [[self engine] isRunning] ){
		NSLog(@"engineReset isRunning");
	} else {
		NSLog(@"engineReset is NOT Running");
	}
	if([self engine] && [[self engine] isRunning]) {
		[_engine stop];
		self.resetAudioEngine = NO;
	}
	@autoreleasepool {
		if(_engine != nil) {
			_engine = nil;
		}
	}
	[self engine];
}


- (void)sessionInterupted {
	self.isSessionInterrupted = YES;
}


/* the interruption is over */
- (void)endInterruptionWithFlags:(NSUInteger)flags API_AVAILABLE(ios(4.0), watchos(2.0), tvos(9.0)) { /* Currently the only flag is AVAudioSessionInterruptionFlags_ShouldResume. */
	NSLog(@"AVEnginePlayer::endInterruptionWithFlags");
	if(flags == AVAudioSessionInterruptionTypeBegan) {
		[self beginInterruption];
	} else if(flags == AVAudioSessionInterruptionTypeEnded) {
		[self endInterruption];
	}
}

/* notification for input become available or unavailable */
- (void)inputIsAvailableChanged:(BOOL)isInputAvailable {
	NSLog(@"AVEnginePlayer::inputIsAvailableChanged");
}

// setupSharedSession is to prevent other iOS Classes closing the audio feed, such as AVAssetReader, when reading from disk
// It is set once on first launch of a AVAudioPlayer and remains as a set property from then on
- (void) setupSharedSession {
#ifndef TARGET_OSX
	if(audioSessionSetup) {
			return;
	}
	NSString * playbackCategory = AVAudioSessionCategoryPlayback;

	AVAudioSession * audioSession = [AVAudioSession sharedInstance];
	NSError * err = nil;


	if(![audioSession setCategory:playbackCategory
													  withOptions:AVAudioSessionCategoryOptionInterruptSpokenAudioAndMixWithOthers
									error:&err]) {

		NSLog(@"Unable to setCategory: withOptions error %@, %@", err, [err userInfo]);
		err = nil;

	}

	if(![[AVAudioSession sharedInstance] setActive: YES withOptions:AVAudioSessionSetActiveOptionNotifyOthersOnDeactivation error: &err]) {
		NSLog(@"Unable to setActive: error %@, %@", err, [err userInfo]);
		err = nil;
	}

	double hwSampleRate = 44100.0;
	BOOL success = [[AVAudioSession sharedInstance] setPreferredSampleRate:hwSampleRate error:&err];
		if (!success) NSLog(@"Error setting preferred sample rate! %@\n", [err localizedDescription]);


	audioSessionSetup = YES;
#endif
}

- (instancetype)init
{
	self = [super init];
	if (self) {
		[self setupSharedSession];
		_mainMixer = [[self engine] mainMixerNode];
		_mainMixer.outputVolume = 0.98;

		_mShouldLoop = false;
		_mGaurdCount = 0;
		_mMultiPlay = false;
		_bIsPlaying = false;
		_isSessionInterrupted = NO;
		_mRequestedPositonSeconds = 0.0f;
		_startedSampleOffset = 0;
		_bInterruptedWhileRunning = NO;
		_resetAudioEngine = NO;
		_mPlayingAtInterruption = NO;
		_mPositonSecondsAtInterruption = 0;
		_isConfigChangePending = NO;


#ifndef TARGET_OSX

		//from: https://github.com/robovm/apple-ios-samples/blob/master/UsingAVAudioEngineforPlaybackMixingandRecording/AVAEMixerSample/AudioEngine.m

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleInterruption:)
													 name:AVAudioSessionInterruptionNotification
												   object:[AVAudioSession sharedInstance]];

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleRouteChange:)
													 name:AVAudioSessionRouteChangeNotification
												   object:[AVAudioSession sharedInstance]];

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleMediaServicesReset:)
													 name:AVAudioSessionMediaServicesWereLostNotification
												   object:[AVAudioSession sharedInstance]];

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(engineConfigurationChange:)
													 name:AVAudioEngineConfigurationChangeNotification
												   object:nil];

		[[NSNotificationCenter defaultCenter] addObserver:self
												 selector:@selector(handleEngineShouldPauseNotification:)
													 name:kShouldEnginePauseNotification
												   object:nil];
#endif

	}

	return self;
}

- (void)dealloc
{
#ifndef TARGET_OSX
	[[NSNotificationCenter defaultCenter] removeObserver:self
													 name:AVAudioSessionInterruptionNotification
												   object:[AVAudioSession sharedInstance]];

	[[NSNotificationCenter defaultCenter] removeObserver:self
													 name:AVAudioSessionRouteChangeNotification
												   object:[AVAudioSession sharedInstance]];

	[[NSNotificationCenter defaultCenter] removeObserver:self
													 name:AVAudioSessionMediaServicesWereLostNotification
												   object:[AVAudioSession sharedInstance]];

	[[NSNotificationCenter defaultCenter] removeObserver:self
													 name:AVAudioEngineConfigurationChangeNotification
												   object:nil];

	[[NSNotificationCenter defaultCenter] removeObserver:self
													 name:kShouldEnginePauseNotification
												   object:nil];
#endif
}

#ifndef TARGET_OSX

- (void)engineConfigurationChange:(NSNotification *)notification
{
	NSLog(@"AVEnginePlayer::engineConfigurationChange");

	bool bWasPlaying = self.bIsPlaying;
	float posSeconds = [self positionSeconds];
	float positionsSecondsAtInterruption = self.mPositonSecondsAtInterruption;

	NSLog(@"engineConfigurationChange bWasPlaying %d posSeconds %0.2f", bWasPlaying, posSeconds);

	[[self engine] stop];

	//engineReset was blowing up and causing all sorts of crashes and having no sound in the app after a phone call.
	//instead we attempt to connect nodes again after engine config change happens.
	if(self.resetAudioEngine) {
		[self engineReset];
	}else{
		[self engineReconnect];
	}

	BOOL bSuccess = [self startEngine];
	if(!bSuccess) {
		NSLog(@"AVEnginePlayer::engineConfigurationChange unable to startEngine!");
	}

	[self connectPlayerToEngine];

	if(self.isSessionInterrupted && self.mPlayingAtInterruption) {
		[self play:positionsSecondsAtInterruption];
	}else if( bWasPlaying ){
		[self play:posSeconds];
	}
}

- (void)handleEngineShouldPauseNotification:(NSNotification *)notification {
	NSLog(@"AVEnginePlayer::handleEngineShouldPauseNotification");
	if(self.bIsPlaying) {
		[self pause];
	}
}

- (void)handleInterruption:(NSNotification *)notification
{
	NSLog(@"AVEnginePlayer::handleInterruption");
	NSNumber* reason = [notification.userInfo valueForKey:AVAudioSessionInterruptionTypeKey];
	NSNumber* opt = [notification.userInfo valueForKey:AVAudioSessionInterruptionOptionKey];

	bool playingAtInterruption = false;
	float positionsSecondsAtInterruption = 0;

	if ([reason intValue] == AVAudioSessionInterruptionTypeBegan) {
		NSLog(@"AVEnginePlayer::handleInterruption - BEGIN");

		playingAtInterruption = self.bIsPlaying;
		positionsSecondsAtInterruption = [self positionSeconds];

		if([[self engine] isRunning]) {
			[[self engine] pause];
			//self.bInterruptedWhileRunning = YES;
		}

		[self sessionInterupted];
	} else {
		NSLog(@"AVEnginePlayer::handleInterruption - END");

		BOOL shouldResume = [opt intValue] == AVAudioSessionInterruptionOptionShouldResume;
		NSLog(@"AVEnginePlayer::handleInterruption - END - shouldResume %d", shouldResume);

		BOOL bSuccess = [self startEngine];
		if(!bSuccess) {
			NSLog(@"AVEnginePlayer::handleInterruption unable to startEngine!");
		}

		self.isSessionInterrupted = NO;

		if( playingAtInterruption ){
			[self play:positionsSecondsAtInterruption];
		}
	}

	self.mPlayingAtInterruption = playingAtInterruption;
	self.mPositonSecondsAtInterruption = positionsSecondsAtInterruption;
}

- (void)handleRouteChange:(NSNotification *)notification
{
	NSLog(@"AVEnginePlayer::handleRouteChange");
	NSNumber* reason = [notification.userInfo valueForKey:AVAudioSessionRouteChangeReasonKey];

	if( [reason intValue] == AVAudioSessionRouteChangeReasonOldDeviceUnavailable ){
		NSLog(@"AVEnginePlayer::handleRouteChange - device unavailable");
		[[NSNotificationCenter defaultCenter] postNotificationName:kShouldEnginePauseNotification object:nil];
	}
}

- (void)handleMediaServicesReset:(NSNotification *)notification
{
	NSLog(@"AVEnginePlayer::handleMediaServicesReset");
	[self engineReset];
	BOOL bSuccess = [self startEngine];
	if(!bSuccess) {
		NSLog(@"AVEnginePlayer::handleMediaServicesReset unable to startEngine!");
	}
	[self connectPlayerToEngine];
}

#endif

- (AVAudioMixerNode *)mainMixer {
	return _mainMixer;
}

- (BOOL)loadWithFile:(NSString*)file {
	NSString * filePath = [[NSBundle mainBundle] pathForResource:file ofType:nil];
	return [self loadWithPath:filePath];
}

- (BOOL)loadWithURL:(NSURL*)url {

	if( url == nil ){
		NSLog(@"AVEnginePlayer::loadWithURL url is nil!");
		return NO;
	}

	AVAudioFile * soundFile;
	NSError * err = nil;
	@autoreleasepool {
		soundFile = [[AVAudioFile alloc] initForReading:url error:&err];
	}
	if( err != nil ){
		NSLog(@"AVEnginePlayer::loadWithURL error reading file %@ !\n", [err localizedDescription]);
		return NO;
	}

	return [self loadWithSoundFile:soundFile];
}

- (BOOL)loadWithPath:(NSString*)path {

	if( path == nil ){
		NSLog(@"AVEnginePlayer::loadWithPath path is nil!");
		return NO;
	}

	NSURL * url;
	@autoreleasepool {
		url = [NSURL fileURLWithPath:path];
	}
	return [self loadWithURL:url];
}

- (BOOL)loadWithSoundFile:(AVAudioFile*)aSoundFile {

	if( aSoundFile == nil ){
		NSLog(@"AVEnginePlayer::loadWithSoundFile aSoundFile is nil!");
		return NO;
	}

	if( ![self startEngine] ){
		NSLog(@"AVEnginePlayer::loadWithSoundFile unable to startEngine!");
		return NO;
	}

	_soundFile = aSoundFile;
	if( ![self createPlayerNode] ){
		NSLog(@"AVEnginePlayer::loadWithSoundFile unable to createPlayerNode!");
		return NO;
	}
	if( ![self connectPlayerToEngine] ){
		NSLog(@"AVEnginePlayer::loadWithSoundFile unable to connectPlayerToEngine!");
		return NO;
	}

	return YES;
}

- (void)unloadSound {

	[self stop];

	if(self.soundPlayer != nil) {
		if([[self engine] isRunning]) {
			[[self engine] detachNode:self.soundPlayer];
		}
		@autoreleasepool {
			self.soundPlayer = nil;
		}
	}

	if(self.variSpeed != nil) {
		if([[self engine] isRunning]) {
			[[self engine] detachNode:self.variSpeed];
		}
		@autoreleasepool {
			self.variSpeed = nil;
		}
	}

	@autoreleasepool {
		self.soundFile = nil;
	}
}

- (BOOL)createPlayerNode {

	if( self.soundFile == nil ){
		NSLog(@"AVEnginePlayer::createPlayerNode soundFile is nil!");
		return NO;
	}

	if( self.soundPlayer != nil ){
		[self unloadSound];
	}

	@autoreleasepool {
		self.soundPlayer = [[AVAudioPlayerNode alloc] init];
		self.variSpeed = [[AVAudioUnitVarispeed alloc] init];
	}
	[[self engine] attachNode:self.soundPlayer];
	[[self engine] attachNode:self.variSpeed];

	return YES;
}

- (BOOL)connectPlayerToEngine {

	if( self.soundFile == nil ){
		NSLog(@"AVEnginePlayer::connectPlayerToEngine soundFile is nil!");
		return NO;
	}
	if( self.soundPlayer == nil ){
		NSLog(@"AVEnginePlayer::connectPlayerToEngine soundPlayer is nil!");
		return NO;
	}
	if( self.variSpeed == nil ){
		NSLog(@"AVEnginePlayer::connectPlayerToEngine variSpeed is nil!");
		return NO;
	}

	[[self engine] connect:self.soundPlayer to:self.variSpeed format:[self.soundFile processingFormat]];
	[[self engine] connect:self.variSpeed to:self.mainMixer format:[self.soundFile processingFormat]];

	return YES;
}

- (BOOL)startEngine {

	if([[self engine] isRunning]) {
		return YES;
	}

	NSError * err = nil;
	BOOL bOk = [[self engine] startAndReturnError:&err];
	if( !bOk ){
		NSLog(@"AVEnginePlayer::startEngine unable to start engine!");
		NSLog(@"AVEnginePlayer::startEngine error %@", [err localizedDescription]);
		return NO;
	}
	return YES;
}

- (void)scheduleFile {

	if( self.soundPlayer == nil ){
		NSLog(@"AVEnginePlayer::scheduleFile soundPlayer is nil!");
		return;
	}
	if( self.soundFile == nil ){
		NSLog(@"AVEnginePlayer::scheduleFile soundFile is nil!");
		return;
	}

	__weak AVEnginePlayer * weakSelf = self;

	if(self.mShouldLoop) {
		[self.soundPlayer scheduleFile:self.soundFile atTime:nil completionHandler:^{
			if(weakSelf.mShouldLoop && weakSelf.soundPlayer.isPlaying) {
				[weakSelf scheduleFile];
			}
		}];
	} else {
		[self.soundPlayer scheduleFile:self.soundFile atTime:nil completionHandler:^{
			weakSelf.bIsPlaying = false;
		}];
	}
}

- (void)play {
	[self play:0];
}

- (void)play: (float)startTime {

	if( self.soundFile == nil ){
		NSLog(@"AVEnginePlayer::play soundFile is nil!");
		return;
	}
	if( self.soundPlayer == nil ){
		NSLog(@"AVEnginePlayer::play soundPlayer is nil!");
		return;
	}
	if(![self startEngine]) {
		NSLog(@"AVEnginePlayer::play unable to startEngine!");
		return;
	}

	AVAudioFramePosition samplePos = startTime * self.soundFile.processingFormat.sampleRate;

	[self.soundPlayer stop];

	if(samplePos >= (self.soundFile.length-1)) {
		samplePos = 0;
	}

	if(samplePos > 0) {
		self.soundFile.framePosition = samplePos;
	} else {
		self.soundFile.framePosition = 0;
	}

	_startedSampleOffset = self.soundFile.framePosition;

	[self scheduleFile];
	[self.soundPlayer play];

	_bIsPlaying = true;
}

- (void)stop {
	if( self.soundPlayer == nil ){
		return;
	}
	_mRequestedPositonSeconds = 0.0f;
	[self.soundPlayer stop];
	_bIsPlaying = false;
}

- (void)pause {
	if( self.soundPlayer == nil ){
		return;
	}
	_mRequestedPositonSeconds = [self positionSeconds];
	[self.soundPlayer pause];
	_bIsPlaying = false;
}

- (BOOL)isLoaded {
	if( self.soundFile != nil ){
		return YES;
	}
	return NO;
}

- (BOOL)isPlaying {
	return _bIsPlaying;
}

- (void)volume:(float)value {
	if( self.soundPlayer == nil ){
		return;
	}
	self.soundPlayer.volume = value;
}

- (float)volume {
	if( self.soundPlayer == nil ){
		return 0;
	}
	return self.soundPlayer.volume;
}

- (void)pan:(float)value {
	if( self.soundPlayer == nil ){
		return;
	}
	self.soundPlayer.pan = value;
}

- (float)pan {
	if( self.soundPlayer == nil ){
		return 0;
	}
	return self.soundPlayer.pan;
}

- (void)speed:(float)value {
	if( self.variSpeed == nil ){
		return;
	}
	self.variSpeed.rate = value;
}

- (float)speed {
	if( self.variSpeed == nil ){
		return 1;
	}
	return self.variSpeed.rate;
}

- (void)loop:(BOOL)value {
	_mShouldLoop = value;
}

- (BOOL)loop {
	return _mShouldLoop;
}

- (void)multiPlay:(BOOL)value {
	_mMultiPlay = value;
}

- (BOOL)multiPlay {
	return _mMultiPlay;
}

- (void)position:(float)value {

	if( self.soundFile == nil ){
		return;
	}

	float duration = [self soundDurationSeconds];
	float posSeconds = value * duration;
	[self play:posSeconds];
}

- (float)position {

	if( self.soundFile == nil ){
		return 0.0;
	}

	float duration = [self soundDurationSeconds];
	if( duration == 0 ){
		return 0.0;
	}
	float posSeconds = [self positionSeconds];
	return posSeconds / duration;
}

- (void)positionMs:(int)value {

	if( self.soundFile == nil ){
		return;
	}

	float posSeconds = value / 1000.0;
	[self play:posSeconds];
}

- (int)positionMs {
	return [self positionSeconds] * 1000.0;
}

- (AVAudioFile *)getSoundFile {
	return self.soundFile;
}

- (float)positionSeconds{
	if( [self isLoaded] && self.soundPlayer != nil && self.engine != nil){
		if( !self.soundPlayer.isPlaying ){
			return _mRequestedPositonSeconds;
		}else{

			AVAudioTime * nodeTime = [self.soundPlayer lastRenderTime];
			AVAudioTime * playerTime = [self.soundPlayer playerTimeForNodeTime:nodeTime];

			double sampleRate = self.soundFile.processingFormat.sampleRate;
			double sampleTime = (double)playerTime.sampleTime;
			if( sampleRate > 0 ){
				return (sampleTime / sampleRate) + ((double)_startedSampleOffset / sampleRate);
			}
		}
	}
	return 0.0;
}

- (float)soundDurationSeconds{
	if( [self isLoaded] && self.variSpeed != nil && self.engine != nil){
		float duration = 0.0;
		if(self.soundFile.processingFormat != nil)
			duration = self.soundFile.length / self.soundFile.processingFormat.sampleRate;
		return duration;
	}
	return 0.0;
}

@end



using namespace std;

// Initialize static members
ofSoundFFT ofAVEngineSoundPlayer::systemFFT;
std::vector<float> ofAVEngineSoundPlayer::systemBins;
bool ofAVEngineSoundPlayer::systemFFTInstalled = false;

ofAVEngineSoundPlayer::ofAVEngineSoundPlayer() {
	soundPlayer = NULL;
}

ofAVEngineSoundPlayer::~ofAVEngineSoundPlayer() {
	unload();
}

bool ofAVEngineSoundPlayer::load(const fs::path & fileName, bool /* stream */) {
	if(soundPlayer != NULL) {
		unload();
	}

	auto filePath = ofCore.toDataPath(fileName);
	@autoreleasepool {
		soundPlayer = [[AVEnginePlayer alloc] init];
	}
	BOOL bOk = [(AVEnginePlayer *)soundPlayer loadWithPath:[NSString stringWithUTF8String:filePath.c_str()]];

	// Auto-install system FFT on first successful load
	if (bOk && !systemFFTInstalled) {
		installSystemFFT();
	}

	return bOk;
}

void ofAVEngineSoundPlayer::unload() {
	if(soundPlayer != NULL) {

		[(AVEnginePlayer *)soundPlayer unloadSound];
		@autoreleasepool {
			soundPlayer = nil;
		}
	}
	if( bAddedUpdate ){
		ofRemoveListener(ofEvents().update, this, &ofAVEngineSoundPlayer::updateFunction);
		bAddedUpdate = false;
	}
	cleanupMultiplayers();
}

void ofAVEngineSoundPlayer::play() {
	if(soundPlayer == NULL) {
		return;
	}

	auto mainPlayer = (AVEnginePlayer *)soundPlayer;
	if( [mainPlayer multiPlay] && [mainPlayer isPlaying] ){

		AVEnginePlayer * extraPlayer;
		@autoreleasepool {
		   extraPlayer = [[AVEnginePlayer alloc] init];
		}
		BOOL bOk = [extraPlayer loadWithSoundFile:[mainPlayer getSoundFile]];
		if( bOk ){
				[extraPlayer speed:[mainPlayer speed]];
				[extraPlayer pan:[mainPlayer pan]];
				[extraPlayer volume:[mainPlayer volume]];
				[extraPlayer play];

				mMultiplayerSoundPlayers.push_back(extraPlayer);

				if( !bAddedUpdate ){
					ofAddListener(ofEvents().update, this, &ofAVEngineSoundPlayer::updateFunction);
					bAddedUpdate = true;
				}
		}

	}else{
		[(AVEnginePlayer *)soundPlayer play];
	}
}

void ofAVEngineSoundPlayer::cleanupMultiplayers(){
	for( auto mMultiPlayer : mMultiplayerSoundPlayers ){
		if( mMultiPlayer != NULL ){
			[(AVEnginePlayer *)mMultiPlayer stop];
			[(AVEnginePlayer *)mMultiPlayer unloadSound];
						@autoreleasepool {
							mMultiPlayer = nil;
						}
		}
	}
	mMultiplayerSoundPlayers.clear();
}

bool ofAVEngineSoundPlayer::removeMultiPlayer(void * aPlayer){
	return( aPlayer == NULL );
}

//better do do this in a thread?
//feels safer to use ofEvents().update so we don't need to lock.
// FIXME: removed unused param args
void ofAVEngineSoundPlayer::updateFunction( ofEventArgs & ){

	vector <ObjectType> playerPlayingList;

	for( auto mMultiPlayerPtr : mMultiplayerSoundPlayers ){
			if( mMultiPlayerPtr != NULL ){
				if( [(AVEnginePlayer *)mMultiPlayerPtr isLoaded] && [(AVEnginePlayer *)mMultiPlayerPtr isPlaying] ){
					playerPlayingList.push_back(mMultiPlayerPtr);
				}else{
					[(AVEnginePlayer *)mMultiPlayerPtr unloadSound];
					@autoreleasepool {
						mMultiPlayerPtr = nil;
					}
				}
			}
	}

	mMultiplayerSoundPlayers = playerPlayingList;
}

void ofAVEngineSoundPlayer::stop() {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer stop];
	cleanupMultiplayers();
}

void ofAVEngineSoundPlayer::setVolume(float value) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer volume:value];
}

void ofAVEngineSoundPlayer::setPan(float value) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer pan:value];
}

void ofAVEngineSoundPlayer::setSpeed(float value) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer speed:value];
}

void ofAVEngineSoundPlayer::setPaused(bool bPause) {
	if(soundPlayer == NULL) {
		return;
	}
	if(bPause) {
		[(AVEnginePlayer *)soundPlayer pause];
	} else {
		[(AVEnginePlayer *)soundPlayer play];
	}
}

void ofAVEngineSoundPlayer::setLoop(bool bLoop) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer loop:bLoop];
}

void ofAVEngineSoundPlayer::setMultiPlay(bool bMultiPlay) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer multiPlay:bMultiPlay];
}

void ofAVEngineSoundPlayer::setPosition(float position) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer position:position];
}

void ofAVEngineSoundPlayer::setPositionMS(int positionMS) {
	if(soundPlayer == NULL) {
		return;
	}
	[(AVEnginePlayer *)soundPlayer positionMs:positionMS];
}

float ofAVEngineSoundPlayer::getPosition()  const{
	if(soundPlayer == NULL) {
		return 0;
	}
	return [(AVEnginePlayer *)soundPlayer position];
}

int ofAVEngineSoundPlayer::getPositionMS() const {
	if(soundPlayer == NULL) {
		return 0;
	}
	return [(AVEnginePlayer *)soundPlayer positionMs];
}

bool ofAVEngineSoundPlayer::isPlaying()  const{
	if(soundPlayer == NULL) {
		return false;
	}

	bool bMainPlaying = [(AVEnginePlayer *)soundPlayer isPlaying];
	if( !bMainPlaying && mMultiplayerSoundPlayers.size() ){
		return true;
	}

	return bMainPlaying;
}

float ofAVEngineSoundPlayer::getSpeed()  const{
	if(soundPlayer == NULL) {
		return 0;
	}
	return [(AVEnginePlayer *)soundPlayer speed];
}

float ofAVEngineSoundPlayer::getPan()  const{
	if(soundPlayer == NULL) {
		return 0;
	}
	return [(AVEnginePlayer *)soundPlayer pan];
}

bool ofAVEngineSoundPlayer::isLoaded()  const{
	if(soundPlayer == NULL) {
		return false;
	}
	return [(AVEnginePlayer *)soundPlayer isLoaded];
}

float ofAVEngineSoundPlayer::getVolume()  const{
	if(soundPlayer == NULL) {
		return false;
	}
	return [(AVEnginePlayer *)soundPlayer volume];
}

float ofAVEngineSoundPlayer::getDuration() const {
	if(soundPlayer == NULL) {
		return 0.0f;
	}
	return [(AVEnginePlayer *)soundPlayer soundDurationSeconds];
}

unsigned int ofAVEngineSoundPlayer::getDurationMS() const {
	if(soundPlayer == NULL) {
		return 0;
	}
	return ([(AVEnginePlayer *)soundPlayer soundDurationSeconds] * 1000.0f);
}

void * ofAVEngineSoundPlayer::getAVEnginePlayer() {
	return (__bridge void *)soundPlayer;
}

void ofAVEngineSoundPlayer::installFFTOnMixer() {
	if (soundPlayer == NULL) {
		ofLogWarning("ofAVEngineSoundPlayer") << "installFFTOnMixer(): soundPlayer is NULL";
		return;
	}

	AVAudioMixerNode* mixer = [(AVEnginePlayer*)soundPlayer mainMixer];
	if (mixer == nullptr) {
		ofLogError("ofAVEngineSoundPlayer") << "installFFTOnMixer(): mixer is null";
		return;
	}

	[mixer installTapOnBus:0 bufferSize:2048 format:[mixer outputFormatForBus:0] block:^(AVAudioPCMBuffer *buffer, AVAudioTime * /* when */) {
		float* data = buffer.floatChannelData[0];
		fft.process(data, buffer.frameLength);
	}];

	ofLogVerbose("ofAVEngineSoundPlayer") << "installFFTOnMixer(): per-player tap installed";
}

void ofAVEngineSoundPlayer::installSystemFFTOnPlayerMixer() {
	// Install system FFT on this player's mixer (for testing)
	if (soundPlayer == NULL) {
		ofLogWarning("ofAVEngineSoundPlayer") << "installSystemFFTOnPlayerMixer(): soundPlayer is NULL";
		return;
	}

	AVAudioMixerNode* mixer = [(AVEnginePlayer*)soundPlayer mainMixer];
	if (mixer == nullptr) {
		ofLogError("ofAVEngineSoundPlayer") << "installSystemFFTOnPlayerMixer(): mixer is null";
		return;
	}

	// Make sure systemFFT is initialized
	if (systemFFT.getSpectrum().size() == 0) {
		systemFFT.setup(512);
		systemBins.resize(257, 0.0f);
	}

	int fftSize = (int)(systemFFT.getSpectrum().size() - 1) * 2;
	AVAudioFrameCount bufferSize = fftSize;

	[mixer installTapOnBus:0 bufferSize:bufferSize format:[mixer outputFormatForBus:0] block:^(AVAudioPCMBuffer *buffer, AVAudioTime * /* when */) {
		if (buffer.frameLength > 0 && buffer.floatChannelData != nullptr && buffer.format.channelCount > 0) {
			float* data = buffer.floatChannelData[0];
			int frameLength = (int)buffer.frameLength;

			// Debug: check if we're receiving audio
			static int debugCounter = 0;
			if (debugCounter++ % 100 == 0) {
				float maxVal = 0;
				for (int i = 0; i < frameLength && i < 100; i++) {
					maxVal = std::max(maxVal, std::abs(data[i]));
				}
				NSLog(@"Player FFT Tap: frameLength=%d, maxAudio=%.4f", frameLength, maxVal);
			}

			systemFFT.process(data, frameLength);
			const auto& spectrum = systemFFT.getSpectrum();
			if (systemBins.size() != spectrum.size()) {
				systemBins.resize(spectrum.size());
			}
			std::copy(spectrum.begin(), spectrum.end(), systemBins.begin());
		}
	}];

	systemFFTInstalled = true;
	ofLogVerbose("ofAVEngineSoundPlayer") << "installSystemFFTOnPlayerMixer(): tap installed on player mixer";
}

const std::vector<float>& ofAVEngineSoundPlayer::getSpectrum(int bands) const {
	if (fft.getSpectrum().size() != static_cast<size_t>(bands)) {
		const_cast<ofSoundFFT&>(fft).setup(bands); // lazy init
	}
	return fft.getSpectrum();
}

// System-wide FFT implementation
void ofAVEngineSoundPlayer::installSystemFFT() {
	if (systemFFTInstalled) {
		return;
	}

	// Engine might not exist yet - this is OK, we'll install on first load
	if (_engine == nullptr) {
		ofLogVerbose("ofAVEngineSoundPlayer") << "installSystemFFT(): engine not yet created, will install on first sound load";
		return;
	}

	// Make sure engine is running
	if (![_engine isRunning]) {
		NSError* error = nil;
		if (![_engine startAndReturnError:&error]) {
			ofLogError("ofAVEngineSoundPlayer") << "installSystemFFT(): couldn't start engine: " << [[error localizedDescription] UTF8String];
			return;
		}
	}

	AVAudioMixerNode* mainMixer = [_engine mainMixerNode];
	if (mainMixer == nullptr) {
		ofLogError("ofAVEngineSoundPlayer") << "installSystemFFT(): mainMixer is null";
		return;
	}

	// Make sure systemFFT is initialized with a default size
	if (systemFFT.getSpectrum().size() == 0) {
		systemFFT.setup(512); // Default to 512 bands
		systemBins.resize(257, 0.0f); // 512/2 + 1
	}

	// Install tap on the main mixer output to capture system-wide audio
	AVAudioFormat* format = [mainMixer outputFormatForBus:0];
	if (format == nullptr) {
		ofLogError("ofAVEngineSoundPlayer") << "installSystemFFT(): format is null";
		return;
	}

	// Calculate buffer size based on FFT size
	int fftSize = (int)(systemFFT.getSpectrum().size() - 1) * 2;
	AVAudioFrameCount bufferSize = fftSize;

	ofLogVerbose("ofAVEngineSoundPlayer") << "installSystemFFT(): installing tap with buffer size " << bufferSize;

	[mainMixer installTapOnBus:0 bufferSize:bufferSize format:format block:^(AVAudioPCMBuffer *buffer, AVAudioTime * /* when */) {
		if (buffer.frameLength > 0 && buffer.floatChannelData != nullptr && buffer.format.channelCount > 0) {
			float* data = buffer.floatChannelData[0];
			int frameLength = (int)buffer.frameLength;
			int fftSize = (int)(systemFFT.getSpectrum().size() - 1) * 2;

			// Debug: check if we're receiving audio
			static int debugCounter = 0;
			if (debugCounter++ % 100 == 0) { // Log every 100th buffer
				float maxVal = 0;
				for (int i = 0; i < frameLength && i < 100; i++) {
					maxVal = std::max(maxVal, std::abs(data[i]));
				}
				NSLog(@"FFT Tap: frameLength=%d, fftSize=%d, maxAudio=%.4f", frameLength, fftSize, maxVal);
			}

			// Only process if we have enough data
			if (frameLength >= fftSize) {
				// Process the audio through FFT (use first fftSize samples)
				systemFFT.process(data, fftSize);
				// Copy spectrum to systemBins
				const auto& spectrum = systemFFT.getSpectrum();
				if (systemBins.size() != spectrum.size()) {
					systemBins.resize(spectrum.size());
				}
				std::copy(spectrum.begin(), spectrum.end(), systemBins.begin());
			}
		}
	}];

	systemFFTInstalled = true;
	ofLogVerbose("ofAVEngineSoundPlayer") << "installSystemFFT(): tap installed successfully";
}

void ofAVEngineSoundPlayer::removeSystemFFT() {
	if (!systemFFTInstalled || _engine == nullptr) {
		return;
	}

	AVAudioMixerNode* mainMixer = [_engine mainMixerNode];
	if (mainMixer != nullptr) {
		[mainMixer removeTapOnBus:0];
	}

	systemBins.clear();
	systemFFTInstalled = false;
}

float* ofAVEngineSoundPlayer::getSystemSpectrum(int bands) {
	if (systemFFT.getSpectrum().size() != static_cast<size_t>(bands)) {
		systemFFT.setup(bands);
		systemBins.resize(bands / 2 + 1, 0.0f);
	}
	return systemBins.data();
}

#endif
