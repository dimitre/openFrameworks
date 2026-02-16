#pragma once

#include "ofAppBaseWindow.h"
#include "ofEvents.h"

class ofBaseApp;
class ofBaseRenderer;

class ofAppNoWindow final : public ofAppBaseWindow {

public:
	ofAppNoWindow();
	~ofAppNoWindow() override {}

	static bool doesLoop(){ return false; }
	static bool allowsMultiWindow(){ return false; }
	static void loop(){};
	static bool needsPolling(){ return false; }
	static void pollEvents(){};

	void run(ofBaseApp * appPtr);

	static void exitApp();
	void setup(const ofWindowSettings & settings) override;
	void update() override;
	void draw() override;

	glm::ivec2 getWindowPosition() override;
	glm::ivec2 getWindowSize() override;
	glm::ivec2 getScreenSize() override;

	int			getWidth() override;
	int			getHeight() override;

	ofCoreEvents & events() override;
	std::shared_ptr<ofBaseRenderer> & renderer() override;

private:
	int width, height;

    ofBaseApp *		ofAppPtr;
	std::unique_ptr<ofCoreEvents> coreEvents;
    std::shared_ptr<ofBaseRenderer> currentRenderer;
};
