#include "ofApp.h"

void ofApp::setup() {
	ofSetWindowTitle("ofWorks install is complete");
	ofSetFrameRate(60);
	//	writer.setFps(60);
	for (int a = 0; a < 3600; a++) {
		poeiras.emplace_back();
	}

	int index = 0;
	for (auto & p : poeiras) {
		p.qual = index / (float)poeiras.size();
		index++;
	}
}

void ofApp::update() { }

void ofApp::draw() {
	ofBackground(40);

	for (auto & p : poeiras) {
		float h = p.qual * 155.0f;
		float s = 255.0f;
		float b = 255.0f;
		ofSetFloatColor(ofColor::fromHsb(h, s, b, 100.0f));
		float q = p.qual * 0.1;
		p.am.x = ofNoise(p.pos.x * .005 + q, p.pos.y * .003 + q,
					 ofGetElapsedTimef() * 0.1)
			* mouseX;
		p.am.y = ofNoise(p.pos.x * .001 + q, p.pos.y * .002 + q) * 12.0f;
		p.update();
		p.draw();
	}

	ofSetColor(255);
	//	ofDrawBitmapString(ofToString(ofGetFrameRate()), 20, 20);
	ofDrawBitmapString("Flechilhas by Dimitre Lima\n\nhttps://dmtr.org/", 30, 40);

	//	writer.addFrame();
}

void ofApp::keyPressed(int key) {
	//	if (key == ' ') {
	//		writer.toggleRecording();
	//	}
	//	if (key == 's') {
	//		ofSaveScreen("001.png");
	//	}
}

// if we had mouse cursor somewhere
void ofApp::mousePressed(ofMouseEventArgs & mouse) {
	if (mouse.x < 300 && mouse.y < 200) {
		ofOpenURL("https://dmtr.org/");
	}
}
