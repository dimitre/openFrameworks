#pragma once

#include "ofMain.h"
#include "ofxAndroid.h"

class ofApp : public ofxAndroidApp{
	
	float rotate = 0;

	public:
		
		ofApp();
		~ofApp();

		void setup();
		void exit();
		void update();
		void draw();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void windowResized(int w, int h);

		void touchDown(int x, int y, int id);
		void touchMoved(int x, int y, int id);
		void touchUp(int x, int y, int id);
		void touchDoubleTap(int x, int y, int id);
		void touchCancelled(int x, int y, int id);
		void swipe(ofxAndroidSwipeDir swipeDir, int id);

		void pause();
		void stop();
		void resume();
		void reloadTextures();

		bool backPressed();
		void okPressed();
		void cancelPressed();

		void deviceRefreshRateChanged(int refreshRate);
		void deviceHighestRefreshRateChanged(int refreshRate);
		void deviceRefreshRateChangedEvent(int &refreshRate);
		void deviceHighestRefreshRateChangedEvent(int & refreshRate);		
};
