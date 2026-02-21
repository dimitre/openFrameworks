#include "ofVideo.h"
#include "ofLog.h"

//---------------------------------------------
ofVideo::ofVideo() = default;

//---------------------------------------------
ofVideo::~ofVideo() {
    close();
}

//---------------------------------------------
ofVideo::ofVideo(ofVideo&& other) noexcept
    : player(std::move(other.player))
    , path(std::move(other.path))
    , pixels(std::move(other.pixels))
    , bFrameNew(other.bFrameNew)
    , bUsePixels(other.bUsePixels)
    , volume(other.volume) {
    other.bFrameNew = false;
    other.volume = 1.0f;
}

//---------------------------------------------
ofVideo& ofVideo::operator=(ofVideo&& other) noexcept {
    if (this != &other) {
        player = std::move(other.player);
        path = std::move(other.path);
        pixels = std::move(other.pixels);
        bFrameNew = other.bFrameNew;
        bUsePixels = other.bUsePixels;
        volume = other.volume;
        other.bFrameNew = false;
        other.volume = 1.0f;
    }
    return *this;
}

//---------------------------------------------
bool ofVideo::load(const fs::path& videoPath) {
    close();
    
    player = std::make_unique<ofVideoImpl>();
    
    if (!player->load(videoPath)) {
        ofLogError("ofVideo") << "Failed to load: " << videoPath;
        player.reset();
        return false;
    }
    
    path = videoPath;
    
    return true;
}

//---------------------------------------------
void ofVideo::loadAsync(const fs::path& videoPath) {
    close();
    
    player = std::make_unique<ofVideoImpl>();
    player->loadAsync(videoPath);
    path = videoPath;
}

//---------------------------------------------
void ofVideo::close() {
    if (player) {
        player->close();
        player.reset();
    }
    path.clear();
    bFrameNew = false;
    volume = 1.0f;
}

//---------------------------------------------
void ofVideo::update() {
    if (!player || !player->isInitialized()) {
        return;
    }
    
    player->update();
    bFrameNew = player->isFrameNew();
}

//---------------------------------------------
void ofVideo::play() {
    if (player) {
        player->play();
    }
}

//---------------------------------------------
void ofVideo::stop() {
    if (player) {
        player->stop();
    }
}

//---------------------------------------------
void ofVideo::setPaused(bool pause) {
    if (player) {
        player->setPaused(pause);
    }
}

//---------------------------------------------
bool ofVideo::isFrameNew() const {
    return bFrameNew;
}

//---------------------------------------------
bool ofVideo::isLoaded() const {
    return player && player->isLoaded();
}

//---------------------------------------------
bool ofVideo::isPlaying() const {
    return player && player->isPlaying();
}

//---------------------------------------------
bool ofVideo::isPaused() const {
    return player && player->isPaused();
}

//---------------------------------------------
float ofVideo::getWidth() const {
    if (player) {
        return player->getWidth();
    }
    return 0;
}

//---------------------------------------------
float ofVideo::getHeight() const {
    if (player) {
        return player->getHeight();
    }
    return 0;
}

//---------------------------------------------
bool ofVideo::isAllocated() const {
    return player && player->getTexturePtr() && player->getTexturePtr()->isAllocated();
}

//---------------------------------------------
void ofVideo::setAnchorPercent(float xPct, float yPct) {
    if (player && player->getTexturePtr()) {
        player->getTexturePtr()->setAnchorPercent(xPct, yPct);
    }
}

//---------------------------------------------
void ofVideo::setAnchorPoint(float x, float y) {
    if (player && player->getTexturePtr()) {
        player->getTexturePtr()->setAnchorPoint(x, y);
    }
}

//---------------------------------------------
void ofVideo::resetAnchor() {
    if (player && player->getTexturePtr()) {
        player->getTexturePtr()->resetAnchor();
    }
}

//---------------------------------------------
ofTexture& ofVideo::getTexture() {
    static ofTexture nullTexture;
    if (player && player->getTexturePtr()) {
        return *player->getTexturePtr();
    }
    return nullTexture;
}

//---------------------------------------------
const ofTexture& ofVideo::getTexture() const {
    static ofTexture nullTexture;
    if (player && player->getTexturePtr()) {
        return *player->getTexturePtr();
    }
    return nullTexture;
}

//---------------------------------------------
void ofVideo::draw(float x, float y) const {
    if (player) {
        player->draw(x, y, getWidth(), getHeight());
    }
}

//---------------------------------------------
void ofVideo::draw(float x, float y, float w, float h) const {
    if (player) {
        player->draw(x, y, w, h);
    }
}

//---------------------------------------------
void ofVideo::setPosition(float pct) {
    if (player) {
        player->setPosition(pct);
    }
}

//---------------------------------------------
float ofVideo::getPosition() const {
    if (player) {
        return player->getPosition();
    }
    return 0.0f;
}

//---------------------------------------------
void ofVideo::setSpeed(float speed) {
    if (player) {
        player->setSpeed(speed);
    }
}

//---------------------------------------------
float ofVideo::getSpeed() const {
    if (player) {
        return player->getSpeed();
    }
    return 1.0f;
}

//---------------------------------------------
void ofVideo::setLoopState(ofLoopType state) {
    if (player) {
        player->setLoopState(state);
    }
}

//---------------------------------------------
ofLoopType ofVideo::getLoopState() const {
    if (player) {
        return player->getLoopState();
    }
    return OF_LOOP_NONE;
}

//---------------------------------------------
void ofVideo::setVolume(float vol) {
    volume = vol;
    if (player) {
        player->setVolume(volume);
    }
}

//---------------------------------------------
float ofVideo::getVolume() const {
    return volume;
}

//---------------------------------------------
bool ofVideo::setPixelFormat(ofPixelFormat pixelFormat) {
    if (player) {
        return player->setPixelFormat(pixelFormat);
    }
    return false;
}

//---------------------------------------------
ofPixelFormat ofVideo::getPixelFormat() const {
    if (player) {
        return player->getPixelFormat();
    }
    return OF_PIXELS_UNKNOWN;
}

//---------------------------------------------
float ofVideo::getDuration() const {
    if (player) {
        return player->getDuration();
    }
    return 0.0f;
}

//---------------------------------------------
int ofVideo::getCurrentFrame() const {
    if (player) {
        return player->getCurrentFrame();
    }
    return 0;
}

//---------------------------------------------
int ofVideo::getTotalNumFrames() const {
    if (player) {
        return player->getTotalNumFrames();
    }
    return 0;
}

//---------------------------------------------
void ofVideo::setFrame(int frame) {
    if (player) {
        player->setFrame(frame);
    }
}

//---------------------------------------------
void ofVideo::firstFrame() {
    if (player) {
        player->firstFrame();
    }
}

//---------------------------------------------
void ofVideo::nextFrame() {
    if (player) {
        player->nextFrame();
    }
}

//---------------------------------------------
void ofVideo::previousFrame() {
    if (player) {
        player->previousFrame();
    }
}

//---------------------------------------------
ofPixels& ofVideo::getPixels() {
    if (player) {
        return player->getPixels();
    }
    return pixels;
}

//---------------------------------------------
const ofPixels& ofVideo::getPixels() const {
    if (player) {
        return player->getPixels();
    }
    return pixels;
}
