#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_SILENT);
	ofSetLogLevel("OrganicText",OF_LOG_SILENT);
	ofSetLogLevel("organicTextPreset",OF_LOG_SILENT);
	
	const int w = OFWORKS_DEMO_APP_WIDTH;
	const int h = OFWORKS_DEMO_APP_HEIGHT;
	const int x = ofGetScreenWidth() * 0.5f - w * 0.5f;
	const int y = ofGetScreenHeight() * 0.5f - h * 0.5f;
	ofSetWindowShape(w, h);
	ofSetWindowPosition(x, y); // Center window on screen
	
	float fps = 60.f;
	ofSetFrameRate(fps);
	ot.setup(fps);
	
	organicTextPreset::applyPreset(&ot);
}

//--------------------------------------------------------------
void ofApp::update() {
	ofSetWindowTitle(OFWORKS_DEMO_APP_TITLE);
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofClear(20, 255);
	ot.draw();
	
	bool bOver = (ofGetMouseX() < 265 && ofGetMouseY() < 70);
	ofSetColor(bOver ? 255 : 200);
	ofDrawBitmapString("organicText by moebiusSurfing\n\ngithub.com/moebiusSurfing", 30, 40);
}

//--------------------------------------------------------------
void ofApp::mousePressed(ofMouseEventArgs & mouse) {
	if (mouse.x < 265 && mouse.y < 70) {
		openURL("https://github.com/moebiussurfing/");
	}
}
