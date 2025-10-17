#pragma once

#include "ofMain.h"
//#include "ofVideoWriter.h"

class ofApp : public ofBaseApp{
public:
	void setup();
	void update();
	void draw();
	void keyPressed(int key);
	void mousePressed(ofMouseEventArgs & mouse);
	
	void drawSecondWindow(ofEventArgs & args) {
		ofBackground(ofFloatColor(1, 0, .3));
	}

//	ofVideoWriter writer;
	
	static glm::vec2 r2c (glm::vec2 am) {
		return {
			am.y * std::cos(glm::radians(am.x)),
			am.y * std::sin(glm::radians(am.x))
		};
	}
	
	class poeira {
	public:
		float margem = 50;
		float w = 1200 - margem*2;
		float h = 550 - margem*2;
		ofRectangle canvas = ofRectangle { margem, margem, w, h };
		glm::vec2 pos {
			ofRandom(canvas.x, canvas.x + canvas.width),
			ofRandom(canvas.y, canvas.y + canvas.height) };
//		glm::vec2 pos { ofRandom(0, poeiraSet.dimensions.x),
//			ofRandom(0, poeiraSet.dimensions.y) };
		glm::vec2 am { 0, 0 }; //angle mag
		glm::vec2 amEasy { 0, 0 }; //angle mag
		float qual;
		
		poeira() {
//			pos = { ofRandom(0,1000), ofRandom(0, 800) };
		}
		void update() {
//			cout << "am = " << am << endl;
//			cout << r2c(am) << endl;

			amEasy += (am-amEasy) / 30.0f;
			pos += r2c(amEasy);

			if (pos.x < canvas.x) pos.x = canvas.x + canvas.width;
			if (pos.x > (canvas.x + canvas.width)) pos.x = canvas.x;
			if (pos.y < canvas.y) pos.y = canvas.y + canvas.height;
			if (pos.y > (canvas.y + canvas.height)) pos.y = canvas.y;

		}
		void draw() {
//			ofDrawCircle(pos.x, pos.y, 2);
			ofDrawRectangle(pos.x-1, pos.y-1, 2, 2);
			ofPushMatrix();
			ofTranslate(pos.x, pos.y);
			ofRotateDeg(am.x);
			ofDrawLine(0, 0, -100 * qual, 0);
			ofPopMatrix();
		}
	};
	
	vector<poeira> poeiras;

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
