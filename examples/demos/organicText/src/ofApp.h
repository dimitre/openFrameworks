#pragma once

#include "ofMain.h"

#include "organicText.h"
#include "organicTextPreset.h"

class ofApp : public ofBaseApp {
	
public:
	void setup();
	void update();
	void draw();
	void mousePressed(ofMouseEventArgs & mouse);
	
	OrganicText ot;
	
	void openURL(const std::string& url) {
#ifdef _WIN32
		// Windows
		system(("start " + url).c_str());
#elif __APPLE__
		// macOS
		system(("open " + url).c_str());
#elif __linux__
		// Linux (Ubuntu, etc.)
		system(("xdg-open " + url).c_str());
#else
#error "Unsupported platform"
#endif
	}
};
