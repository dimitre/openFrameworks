#include "ofMain.h"
#include "ofApp.h"

int main(){
    // Uncomment here if you need some Classif OF compatible window.
	// ofGLWindowSettings settings;
	ofWindowSettings settings;
	settings.setSize(1024, 768);
	settings.windowMode = OF_WINDOW; //can also be OF_FULLSCREEN
	auto window = ofCreateWindow(settings);
	ofRunApp(window, std::make_shared<ofApp>());
	ofRunMainLoop();
}
