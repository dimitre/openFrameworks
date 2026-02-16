#pragma once

// MARK: OF_VIDEO_CAPTURE_DIRECTSHOW
#include "ofConstants.h"
#include "ofTexture.h"
#include "ofVideoBaseTypes.h"
#include "ofPixels.h" // MARK: ofPixels pixels

// #define OF_SWITCH_TO_DSHOW_FOR_WIN_VIDCAP
// #ifdef OF_SWITCH_TO_DSHOW_FOR_WIN_VIDCAP
	#define OF_VIDEO_CAPTURE_DIRECTSHOW
	#include <videoInput.h>
// #endif

class ofDirectShowGrabber final : public ofBaseVideoGrabber{

	public :

		ofDirectShowGrabber();
		~ofDirectShowGrabber();

		std::vector<ofVideoDevice>	listDevices() const override;
		bool					setup(int w, int h) override;
		void					update() override;
		bool					isFrameNew() const override;
		bool					isInitialized() const override;

		bool					setPixelFormat(ofPixelFormat pixelFormat) override;
		ofPixelFormat			getPixelFormat() const override;

		ofPixels &				getPixels() override;
		const ofPixels &		getPixels() const override;

		void					close() override;
		void					clearMemory();

		void					videoSettings() override;

		float					getWidth() const override;
		float					getHeight() const override;

		void					setVerbose(bool bTalkToMe) override;
		void					setDeviceID(int _deviceID) override;
		void					setDesiredFrameRate(int framerate) override;




	protected:

		bool					bChooseDevice;
		int						deviceID;
		bool 					bVerbose;
		bool 					bGrabberInited;
	    ofPixels pixels;
		int						attemptFramerate;
		bool 					bIsFrameNew;

		int						width, height;
		//--------------------------------- directshow
		#ifdef OF_VIDEO_CAPTURE_DIRECTSHOW
			int 					device;
			videoInput 				VI;
			bool 					bDoWeNeedToResize;
		#endif
};
