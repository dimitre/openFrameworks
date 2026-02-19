#pragma once

#include "ofTexture.h"
#include "ofVideoBaseTypes.h"

template<typename T>
class ofPixels_;
typedef ofPixels_<unsigned char> ofPixels;

class ofVideoGrabber : public ofBaseVideoGrabber, public ofBaseVideoDraws {
public:

	ofVideoGrabber();
	virtual ~ofVideoGrabber() override;
	
	void getDevicesInfo() const override;
	
	std::vector<ofVideoDevice> listDevices() const override;
	bool				isFrameNew() const override;
	void				update() override;
	void				close() override;
	bool				setup(int w, int h) override {return setup(w,h,bUseTexture);}
	bool				setup(int w, int h, bool bTexture);

	bool				setPixelFormat(ofPixelFormat pixelFormat) override;
	ofPixelFormat 		getPixelFormat() const override;

	void				videoSettings() override;
	ofPixels& 			getPixels() override;
	const ofPixels&		getPixels() const override;
	ofTexture &			getTexture() override;
	const ofTexture &	getTexture() const override;
	std::vector<ofTexture> & getTexturePlanes() override;
	const std::vector<ofTexture> & getTexturePlanes() const override;
	void				setVerbose(bool bTalkToMe) override;
	void				setDeviceID(int _deviceID) override;
	bool setDeviceByName(const std::string & name);
	bool setDeviceByNames(const std::vector<std::string> & names);
	void				setDesiredFrameRate(int framerate) override;
	void				setUseTexture(bool bUse) override;
	bool 				isUsingTexture() const override;
	void				draw(float x, float y, float w, float h) const override;
	void				draw(float x, float y) const override;
	using ofBaseDraws::draw;

	void 				bind() const;
	void 				unbind() const;

	//the anchor is the point the image is drawn around.
	//this can be useful if you want to rotate an image around a particular point.
	void				setAnchorPercent(float xPct, float yPct) override;	//set the anchor as a percentage of the image width/height ( 0.0-1.0 range )
	void				setAnchorPoint(float x, float y) override;				//set the anchor point in pixels
	void				resetAnchor() override;								//resets the anchor to (0, 0)

	float				getHeight() const override;
	float				getWidth() const override;

	bool				isInitialized() const override;

	void					setGrabber(std::shared_ptr<ofBaseVideoGrabber> newGrabber);
	std::shared_ptr<ofBaseVideoGrabber> getGrabber();
	const std::shared_ptr<ofBaseVideoGrabber> getGrabber() const;

	template<typename GrabberType>
	std::shared_ptr<GrabberType> getGrabber(){
		return std::dynamic_pointer_cast<GrabberType>(getGrabber());
	}

	template<typename GrabberType>
	const std::shared_ptr<GrabberType> getGrabber() const{
		return std::dynamic_pointer_cast<GrabberType>(getGrabber());
	}

private:

	std::vector<ofTexture> tex;
	bool bUseTexture;
	std::shared_ptr<ofBaseVideoGrabber> grabber;
	int requestedDeviceID;

	mutable ofPixelFormat internalPixelFormat;
	int desiredFramerate;
};
