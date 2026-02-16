#pragma once

#include "ofGstUtils.h"


struct ofGstFramerate{
  int numerator;
  int denominator;
};

struct ofGstVideoFormat{
  std::string mimetype;
  std::string format_name;
  int    width;
  int    height;
  std::vector<ofGstFramerate> framerates;
  ofGstFramerate choosen_framerate;
};

struct ofGstDevice{
  std::string video_device;
  std::string gstreamer_src;
  std::string product_name;
  std::string serial_id;
  std::vector<ofGstVideoFormat> video_formats;
  int current_format;
};

struct ofGstCamData{
    std::vector<ofGstDevice> webcam_devices;
  bool bInited;
};

class ofGstVideoGrabber final: public ofBaseVideoGrabber{
public:
	ofGstVideoGrabber();
	~ofGstVideoGrabber();

	/// needs to be called before initGrabber
	bool setPixelFormat(ofPixelFormat pixelFormat) override;
	ofPixelFormat	getPixelFormat() const override;
	
	void videoSettings() override {};//TODO: what is this??

	std::vector<ofVideoDevice> listDevices() const override;
	void setDeviceID(int id) override;
	void setDesiredFrameRate(int framerate) override;
	bool setup(int w, int h) override;

	void 			update() override;
	bool 			isFrameNew() const override;

	ofPixels&		getPixels() override;
	const ofPixels &		getPixels() const override;
	ofTexture * getTexturePtr() override;

	float 			getHeight() const override;
	float 			getWidth() const override;
	void 			close() override;

	void			setVerbose(bool bVerbose) override;
	bool			isInitialized() const override;

	ofGstVideoUtils *	getGstVideoUtils();
private:
	ofGstVideoFormat&	selectFormat(int w, int h, int desired_framerate, ofPixelFormat desiredPixelFormat);

	mutable ofGstCamData camData;
	bool				bIsCamera;
	int					attemptFramerate;
	int					deviceID;
	ofPixelFormat		internalPixelFormat;
	ofGstVideoUtils		videoUtils;
};
