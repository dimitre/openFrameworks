#include "ofApp.h"
#include "ofMain.h"

int main(){
	ofWindowSettings settings;
	settings.setSize(1200, 550);
//	settings.setGLVersion(3,3); // 4, 1 max
	auto win { ofCreateWindow(settings) };
	auto app { make_shared<ofApp>() };
	
	allMonitors.update();
//	if (allMonitors.rects.size() > 1) {
//		settings.shareContextWith = win;
//		settings.windowMode = OF_FULLSCREEN;
//		settings.fullscreenDisplays = { 0 };
//
//		auto fs { ofCreateWindow(settings) };
//		ofAddListener(fs->events().draw, app.get(), &ofApp::drawSecondWindow);
//
//	}
	ofRunApp(win, app);
	ofRunMainLoop();
}
