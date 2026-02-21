#pragma once

#include "ofTexture.h"
#include "ofPixels.h"
#include "ofVideoBaseTypes.h"
#include "ofConstants.h"
#include "ofAVFYUVRenderer.h"
#include <memory>

// Modern C++ wrapper for AVFoundation video playback
// Replaces the older ofAVFoundationPlayer with cleaner code

class ofAVFVideoPlayer;

//---------------------------------------------
/// \class ofAVFVideoPlayer
/// \brief Modern AVFoundation video player for macOS/iOS.
///
/// Clean rewrite of the older ofAVFoundationPlayer with:
/// - Simpler state management
/// - Better async loading
/// - Direct CVPixelBuffer to ofTexture path
/// - No unnecessary pixel copies
///
class ofAVFVideoPlayer : public ofBaseVideoPlayer {
public:
    ofAVFVideoPlayer();
    ~ofAVFVideoPlayer();
    
    // Disable copy, enable move
    ofAVFVideoPlayer(const ofAVFVideoPlayer&) = delete;
    ofAVFVideoPlayer& operator=(const ofAVFVideoPlayer&) = delete;
    ofAVFVideoPlayer(ofAVFVideoPlayer&& other) noexcept;
    ofAVFVideoPlayer& operator=(ofAVFVideoPlayer&& other) noexcept;

    // Loading
    bool load(const fs::path& fileName) override;
    void loadAsync(const fs::path& fileName) override;
    void close() override;

    // Playback
    void play() override;
    void stop() override;
    void setPaused(bool bPause) override;
    void setPosition(float pct) override;
    void setSpeed(float speed) override;
    void setVolume(float volume) override;
    void setLoopState(ofLoopType state) override;
    void setFrame(int frame) override;

    // Update - call this in ofApp::update()
    void update() override;

    // State queries
    bool isFrameNew() const override;
    bool isLoaded() const override;
    bool isPlaying() const override;
    bool isPaused() const override;
    bool isInitialized() const override { return isLoaded(); }

    // Properties
    float getPosition() const override;
    float getSpeed() const override;
    float getDuration() const override;
    float getWidth() const override;
    float getHeight() const override;
    int getCurrentFrame() const override;
    int getTotalNumFrames() const override;
    ofLoopType getLoopState() const override;

    // Frame stepping
    void firstFrame() override;
    void nextFrame() override;
    void previousFrame() override;

    // Pixel/Texture access
    bool setPixelFormat(ofPixelFormat pixelFormat) override;
    ofPixelFormat getPixelFormat() const override;
    ofPixels& getPixels() override;
    const ofPixels& getPixels() const override;
    ofTexture* getTexturePtr() override;
    
    // Drawing (needed for YUV path)
    void draw(float x, float y) const;
    void draw(float x, float y, float w, float h) const;

private:
    class Impl;
    std::unique_ptr<Impl> pImpl;
};
