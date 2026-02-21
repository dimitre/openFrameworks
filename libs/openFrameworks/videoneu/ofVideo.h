#pragma once

#include "ofGraphicsBaseTypes.h"
#include "ofTexture.h"
#include "ofPixels.h"
#include "ofVideoBaseTypes.h"
#include "ofConstants.h"
#include <string>
#include <memory>

// Platform-specific implementations
#ifdef TARGET_OSX
    #include "ofAVFVideoPlayer.h"
    using ofVideoImpl = ofAVFVideoPlayer;
#elif defined(TARGET_WIN32)
    #include "ofMediaFoundationPlayer.h"
    using ofVideoImpl = ofMediaFoundationPlayer;
#else
    #include "ofGstVideoPlayer.h"
    using ofVideoImpl = ofGstVideoPlayer;
#endif

//---------------------------------------------
/// \class ofVideo
/// \brief A simplified video player using composition over inheritance.
///
/// Unlike ofVideoPlayer which uses multiple inheritance and abstraction layers,
/// ofVideo composes a texture internally. Use getTexture() to access the texture
/// for shaders, or draw() directly.
///
/// Usage:
/// \code
/// ofVideo video;
/// video.load("movie.mp4");
/// video.play();
///
/// void ofApp::update() {
///     video.update();  // Updates texture with new frame
/// }
///
/// void ofApp::draw() {
///     video.draw(0, 0);
///     
///     // For shaders, get the texture explicitly
///     shader.setUniformTexture("videoTex", video.getTexture(), 0);
/// }
/// \endcode
class ofVideo final : public ofBaseDraws {
public:
    ofVideo();
    ~ofVideo();
    
    // Disable copy (texture resources)
    ofVideo(const ofVideo&) = delete;
    ofVideo& operator=(const ofVideo&) = delete;
    
    // Enable move
    ofVideo(ofVideo&& other) noexcept;
    ofVideo& operator=(ofVideo&& other) noexcept;

    /// \brief Load a video file.
    bool load(const fs::path& path);
    
    /// \brief Async load (if supported by platform).
    void loadAsync(const fs::path& path);
    
    /// \brief Close video and release resources.
    void close();

    /// \brief Update texture - call this in ofApp::update().
    void update();

    /// \brief Start playback.
    void play();
    
    /// \brief Stop and return to first frame.
    void stop();
    
    /// \brief Pause/unpause playback.
    void setPaused(bool pause);

    /// \brief Check if a new frame was decoded this update().
    bool isFrameNew() const;

    /// \brief Check if video is loaded.
    bool isLoaded() const;
    
    /// \brief Check if currently playing.
    bool isPlaying() const;
    
    /// \brief Check if paused.
    bool isPaused() const;

    /// \brief Get the internal texture for drawing or shader use.
    ofTexture& getTexture();
    const ofTexture& getTexture() const;
    
    /// \brief Set pixel format (call before load).
    /// OF_PIXELS_RGBA (default), OF_PIXELS_NV12 for YUV
    bool setPixelFormat(ofPixelFormat pixelFormat);
    ofPixelFormat getPixelFormat() const;
    
    /// \brief Draw the video.
    void draw(float x, float y) const override;
    void draw(float x, float y, float w, float h) const override;
    using ofBaseDraws::draw;
    
    /// \brief Get width/height
    float getWidth() const override;
    float getHeight() const override;
    
    /// \brief Anchor point methods (delegate to texture)
    void setAnchorPercent(float xPct, float yPct) override;
    void setAnchorPoint(float x, float y) override;
    void resetAnchor() override;
    
    /// \brief Check if video is loaded and texture ready.
    bool isAllocated() const;

    /// \brief Set playback position (0.0 = start, 1.0 = end).
    void setPosition(float pct);
    
    /// \brief Get current position (0.0 to 1.0).
    float getPosition() const;

    /// \brief Set playback speed (1.0 = normal, -1.0 = reverse).
    void setSpeed(float speed);
    
    /// \brief Get current playback speed.
    float getSpeed() const;

    /// \brief Set loop mode (OF_LOOP_NONE, OF_LOOP_NORMAL, OF_LOOP_PALINDROME).
    void setLoopState(ofLoopType state);
    
    /// \brief Get current loop mode.
    ofLoopType getLoopState() const;

    /// \brief Set volume (0.0 to 1.0).
    /// \note Audio is handled automatically by the platform player.
    void setVolume(float volume);
    
    /// \brief Get current volume.
    float getVolume() const;

    /// \brief Get duration in seconds.
    float getDuration() const;

    /// \brief Get current frame number.
    int getCurrentFrame() const;
    
    /// \brief Get total number of frames.
    int getTotalNumFrames() const;

    /// \brief Jump to specific frame.
    void setFrame(int frame);

    /// \brief Go to first frame.
    void firstFrame();
    
    /// \brief Advance one frame (works when paused).
    void nextFrame();
    
    /// \brief Go back one frame (works when paused).
    void previousFrame();

    /// \brief Get the loaded file path.
    fs::path getMoviePath() const { return path; }

    /// \brief Access decoded pixels (CPU side).
    /// \note Prefer using the texture directly for better performance.
    ofPixels& getPixels();
    const ofPixels& getPixels() const;

private:
    std::unique_ptr<ofVideoImpl> player;
    fs::path path;
    ofPixels pixels;
    bool bFrameNew = false;
    bool bUsePixels = false;  // true if we need CPU pixel access
    float volume = 1.0f;      // Local tracking since player may not have getVolume()
};
