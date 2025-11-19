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
};
