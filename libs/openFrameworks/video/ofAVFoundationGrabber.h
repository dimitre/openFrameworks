/*
 *  AVFoundationVideoGrabber.h
 */

#pragma once

#include "ofConstants.h"

#include "ofVideoBaseTypes.h"
#include "ofTexture.h"
#include "ofThread.h"
// MARK: Template, if 
#include "ofPixels.h"
#include <mutex>

#ifdef __OBJC__

#import <Foundation/Foundation.h>
#import <AVFoundation/AVFoundation.h>
#import <Accelerate/Accelerate.h>
#import <CoreMedia/CoreMedia.h>
#import <CoreVideo/CoreVideo.h>

class ofAVFoundationGrabber;

@interface OSXVideoGrabber : NSObject <AVCaptureVideoDataOutputSampleBufferDelegate> {

	@public
	CGImageRef currentFrame;

	int width;
	int height;

	BOOL bInitCalled;
	int deviceID;

	AVCaptureDeviceInput		*captureInput;
	AVCaptureVideoDataOutput    *captureOutput;
	AVCaptureDevice				*device;
	AVCaptureSession			*captureSession;

	ofAVFoundationGrabber * grabberPtr;
}

-(BOOL)initCapture:(int)framerate capWidth:(int)w capHeight:(int)h;
-(void)startCapture;
-(void)stopCapture;
-(void)lockExposureAndFocus;
-(std::vector <std::string>)listDevices;
-(void)setDevice:(int)_device;
-(void)eraseGrabberPtr;

// FIXME: Dmtr. think of a better API
-(void)getDevicesInfo;


-(CGImageRef)getCurrentFrame;

@end

//------

#endif


class ofAVFoundationGrabber final : virtual public ofBaseVideoGrabber{

	public:
		ofAVFoundationGrabber();
		~ofAVFoundationGrabber();

	void getDevicesInfo() const override;
		void setDeviceID(int deviceID) override;
		void setDesiredFrameRate(int capRate) override;
		bool setPixelFormat(ofPixelFormat PixelFormat) override;

        bool setup(int w, int h) override;
		void update() override;
		bool isFrameNew() const override;
		void close() override;

		ofPixels&		 		getPixels() override;
        const ofPixels&		    getPixels() const override;

		float getWidth() const override{
			return width;
		}
		float getHeight() const override{
			return height;
		}

        bool isInitialized() const override;

		void updatePixelsCB();
		std::vector <ofVideoDevice> listDevices() const override;
		ofPixelFormat getPixelFormat() const override;

	protected:
		bool newFrame = false;
		bool bHavePixelsChanged = false;
		void clear();
		int width, height;

		int device = 0;
        bool bIsInit = false;

		int fps  = -1;
		ofTexture tex;
		ofPixels pixels;

		#ifdef __OBJC__
			OSXVideoGrabber * grabber;
		#else
			void * grabber;
		#endif

	public:
		ofPixelFormat pixelFormat;
		ofPixels pixelsTmp;
		bool bLock = false;
		std::mutex capMutex;

};
