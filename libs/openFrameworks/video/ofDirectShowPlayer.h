//DirectShowVideo and ofDirectShowPlayer written by Theodore Watson, Jan 2014
//See the cpp file for the DirectShow implementation

#pragma once
#include "ofVideoBaseTypes.h"

template<typename T>
class ofPixels_;

typedef ofPixels_<unsigned char> ofPixels;

class DirectShowVideo;

class ofDirectShowPlayer final : public ofBaseVideoPlayer{
    public:
		ofDirectShowPlayer();
		ofDirectShowPlayer(const ofDirectShowPlayer&) = delete;
		ofDirectShowPlayer & operator=(const ofDirectShowPlayer&) = delete;
		ofDirectShowPlayer(ofDirectShowPlayer &&);
		ofDirectShowPlayer & operator=(ofDirectShowPlayer&&);

        bool load(const fs::path & fileName) override;
        void                update() override;

        void                close() override;

        void                play() override;
        void                stop() override;

        bool                isFrameNew() const override;

        const ofPixels &    getPixels() const override;
        ofPixels &          getPixels() override;

        float               getWidth() const override;
        float               getHeight() const override;

        bool                isPaused() const override;
        bool                isLoaded() const override;
        bool                isPlaying() const override;

        bool                setPixelFormat(ofPixelFormat pixelFormat) override;
        ofPixelFormat       getPixelFormat() const override;

        float               getPosition() const override;
        float               getSpeed() const override;
        float               getDuration() const override;
        bool                getIsMovieDone() const override;

        void                setPaused(bool bPause) override;
        void                setPosition(float pct) override;
        void                setVolume(float volume) override; // 0..1
        void                setLoopState(ofLoopType state) override;
        void                setSpeed(float speed) override;
        void                setFrame(int frame) override;  // frame 0 = first frame...

        int                 getCurrentFrame() const override;
        int                 getTotalNumFrames() const override;
        ofLoopType          getLoopState() const override;

        void                firstFrame() override;
        void                nextFrame() override;
        void                previousFrame() override;

        void                setPlaybackRange(float startPosition, float endPosition) override;
        float               getPlaybackStart() const override;
        float               getPlaybackEnd() const override;
        void                clearPlaybackRange() override;
        void                setPlaybackRangeFrames(int startFrame, int endFrame) override;
        int                 getPlaybackStartFrame() const override;
        int                 getPlaybackEndFrame() const override;

    protected:
        std::shared_ptr<DirectShowVideo>   player;
		ofPixelFormat pixelFormat;
};
