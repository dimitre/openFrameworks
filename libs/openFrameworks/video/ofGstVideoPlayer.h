#pragma once

#include "ofGstUtils.h"


class ofGstVideoPlayer final: public ofBaseVideoPlayer, public ofGstAppSink{
public:

	ofGstVideoPlayer();
	~ofGstVideoPlayer();

	/// needs to be called before loadMovie
	bool 	setPixelFormat(ofPixelFormat pixelFormat) override;
	ofPixelFormat	getPixelFormat() const override;

	void loadAsync(const fs::path & fileName) override;
	bool load(const fs::path & fileName) override;

	void 	update() override;

	int		getCurrentFrame() const override;
	int		getTotalNumFrames() const override;

	void 	firstFrame() override;
	void 	nextFrame() override;
	void 	previousFrame() override;
	void 	setFrame(int frame) override;  // frame 0 = first frame...

	bool	isStream() const;

	void 	play() override;
	void 	stop() override;
	void 	setPaused(bool bPause) override;
	bool 	isPaused() const override;
	bool 	isLoaded() const override;
	bool 	isPlaying() const override;

	float	getPosition() const override;
	float 	getSpeed() const override;
	float 	getDuration() const override;
	bool  	getIsMovieDone() const override;

	void 	setPosition(float pct) override;
	void 	setVolume(float volume) override;
	void 	setLoopState(ofLoopType state) override;
	ofLoopType 	getLoopState() const override;
	void 	setSpeed(float speed) override;
	void 	close() override;

	void setPlaybackRange(float startPosition, float endPosition) override;
	float getPlaybackStart() const override;
	float getPlaybackEnd() const override;
	void clearPlaybackRange() override;
	void setPlaybackRangeFrames(int startFrame, int endFrame) override;
	int getPlaybackStartFrame() const override;
	int getPlaybackEndFrame() const override;

	bool 			isFrameNew() const override;

	ofPixels&		getPixels() override;
	const ofPixels& getPixels() const override;
	ofTexture * getTexturePtr() override;

	float 			getHeight() const override;
	float 			getWidth() const override;

	void setFrameByFrame(bool frameByFrame);
	void setThreadAppSink(bool threaded);
	bool isThreadedAppSink() const;
	bool isFrameByFrame() const;

	ofGstVideoUtils * getGstVideoUtils();

protected:
	bool allocate();
	bool createPipeline(std::string uri);
	void on_stream_prepared();

	// return true to set the message as attended so upstream doesn't try to process it
	bool on_message(GstMessage* msg) override {return false;}

private:
	ofPixelFormat		internalPixelFormat;
	guint64				nFrames;
	int 				fps_n, fps_d;
	bool				bIsStream;
	bool				bIsAllocated;
	bool				bAsyncLoad;
	bool				threadAppSink;
	ofGstVideoUtils		videoUtils;
};
