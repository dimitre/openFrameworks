//
//  ofAVFoundationPlayer.h
//  Created by Lukasz Karluk on 06/07/14.
//	Merged with code by Sam Kronick, James George and Elie Zananiri.
//

#pragma once

#include "ofVideoBaseTypes.h"
#include "ofTexture.h"
#include "ofThread.h"
// MARK: Template if pixels is changed to unique_ptr
#include "ofPixels.h"

#ifdef __OBJC__
#import "ofAVFoundationVideoPlayer.h"
#endif

#if defined TARGET_OF_IOS || defined TARGET_OSX
#import <CoreVideo/CoreVideo.h>
#endif

class ofAVFoundationPlayer final : public ofBaseVideoPlayer {

public:

	ofAVFoundationPlayer();
	~ofAVFoundationPlayer();

	bool load(const fs::path & fileName) override;
	void loadAsync(const fs::path & fileName) override;
	void close() override;
	void update() override;

	void draw();
	void draw(float x, float y);
	void draw(const ofRectangle & rect);
	void draw(float x, float y, float w, float h);

	bool setPixelFormat(ofPixelFormat pixelFormat) override;
	ofPixelFormat getPixelFormat() const override;

	void play() override;
	void stop() override;

	bool isFrameNew() const override;
	const ofPixels & getPixels() const override;
	ofPixels & getPixels() override;
	ofTexture * getTexturePtr() override;
	void initTextureCache();
	void killTexture();
	void killTextureCache();

	float getWidth() const override;
	float getHeight() const override;

	bool isPaused() const override;
	bool isLoaded() const override;
	bool isPlaying() const override;

	float getPosition() const override;
	float getSpeed() const override;
	float getDuration() const override;
	bool getIsMovieDone() const override;

	void setPaused(bool bPause) override;
	void setPosition(float pct) override;
	void setVolume(float volume) override; // 0..1
	void setLoopState(ofLoopType state) override;
	void setSpeed(float speed) override;
	void setFrame(int frame) override;  // frame 0 = first frame...

	int	getCurrentFrame() const override;
	int	getTotalNumFrames() const override;
	ofLoopType getLoopState() const override;

	void firstFrame() override;
	void nextFrame() override;
	void previousFrame() override;

	void setPlaybackRange(float startPosition, float endPosition) override;
	float getPlaybackStart() const override;
	float getPlaybackEnd() const override;
	void clearPlaybackRange() override;
	void setPlaybackRangeFrames(int startFrame, int endFrame) override;
	int getPlaybackStartFrame() const override;
	int getPlaybackEndFrame() const override;

	ofAVFoundationPlayer& operator=(ofAVFoundationPlayer other);

#ifdef __OBJC__
	ofAVFoundationVideoPlayer * getAVFoundationVideoPlayer();
#else
	void * getAVFoundationVideoPlayer();
#endif


protected:

	bool loadPlayer(const fs::path & fileName, bool bAsync);
	void disposePlayer();
	bool isReady() const;

#ifdef __OBJC__
	ofAVFoundationVideoPlayer * videoPlayer;
#else
	void * videoPlayer;
#endif

	bool bFrameNew;
	bool bResetPixels;
	bool bUpdatePixels;
	bool bUpdateTexture;
	bool bUseTextureCache;

	ofPixels pixels;
	ofPixelFormat pixelFormat;
	ofTexture videoTexture;

#ifdef TARGET_OF_IOS
	CVOpenGLESTextureCacheRef _videoTextureCache = nullptr;
	CVOpenGLESTextureRef _videoTextureRef = nullptr;
#endif

#ifdef TARGET_OSX
	CVOpenGLTextureCacheRef _videoTextureCache = nullptr;
	CVOpenGLTextureRef _videoTextureRef = nullptr;
#endif
};
