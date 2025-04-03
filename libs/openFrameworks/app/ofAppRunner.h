#pragma once

#include "ofMainLoop.h"
#include "ofWindowSettings.h"

// FIXME: temporario
#include "ofFileUtils.h"


class ofRectangle;
class ofAppBaseWindow;
class ofAppGLFWWindow;
class ofBaseApp;
class ofBaseRenderer;
class ofCoreEvents;

#include <chrono>

using namespace std::chrono;
using namespace std::chrono_literals;

struct fpsCounter {
protected:
	using space = duration<long double, std::nano>;
	time_point<steady_clock> lastTick;
	steady_clock::duration onesec = 1s;
	space interval = onesec / 30.0;
	space accum = onesec / 30.0;
//	bool firstTick = true;
	int nTicks = 0;

public:
	void setTargetFps(double f) {
		interval = onesec / f;
		accum = onesec / f;
	}

	void tick() {
		if (nTicks == 0) {
			nTicks ++;
			lastTick = steady_clock::now();
			return;
		} else if (nTicks == 1) {
			nTicks ++;
			accum = interval;
			lastTick = steady_clock::now();
		} else {

			interval = steady_clock::now() - lastTick;
			//		double ratio = (double)  ofGetMouseX() / (double)ofGetWindowWidth();
			double ratio = 0.1;
			accum = interval * ratio + accum * (1.0 - ratio);
			lastTick = steady_clock::now();
		}
	}

	double get() {
//		cout << "get" << endl;
//		cout << onesec << endl;
//		cout << accum << endl;
//		cout << (onesec / accum) << endl;
//		cout << "-----" << endl;
		return onesec / accum;
	}
};

using std::cout;
using std::endl;


// MARK: - CLOCK

struct ofClock {
private:
	std::chrono::time_point<std::chrono::steady_clock> timeStart = std::chrono::steady_clock::now();

public:
	ofClock() {};
	~ofClock() {};
//	using space = std::chrono::duration<long long, std::nano>;
//	space interval;

	typedef std::chrono::duration<float> float_seconds;

	float getElapsedTimef() {
		return duration_cast<float_seconds>(std::chrono::steady_clock::now() - timeStart).count();
	}

	void resetElapsedTimeCounter() {
		timeStart = std::chrono::steady_clock::now();
	}

	uint64_t getElapsedTimeMillis() {
		return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - timeStart).count();
	}

	uint64_t getElapsedTimeMicros() {
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - timeStart).count();
	}
};




struct ofCoreInternal {
private:
	// ofAppRunner
	bool initialized = false;

public:
	ofCoreInternal() {};
	~ofCoreInternal() {};

	bool exiting = false;
	ofCoreEvents noopEvents;

	ofMainLoop mainLoop;
	fpsCounter fps;

	ofClock clock;

	//--------------------------------------------------
	fs::path defaultDataPath(){
	#if defined TARGET_OSX
		try {
			return fs::canonical(ofFilePath::getCurrentExeDirFS() / "../../../data/");
		} catch(...) {
			return ofFilePath::getCurrentExeDirFS() / "../../../data/";
		}
	#elif defined TARGET_ANDROID
		return string("sdcard/");
	#else
		try {
			return fs::canonical(ofFilePath::getCurrentExeDirFS() / "data/").make_preferred();
		} catch(...) {
			return (ofFilePath::getCurrentExeDirFS() / "data/");
		}
	#endif
	}

	of::filesystem::path dataPath;
	of::filesystem::path defaultWorkingDirectory;
	bool enableDataPath = true;
	// std::shared_ptr<ofMainLoop> mainLoop { std::make_shared<ofMainLoop>() };

	// ofFileUtils

//	bool isInit() { return initialized; }

	void init() {
		if (initialized) return;
		initialized = true;
		exiting = false;

		defaultWorkingDirectory = fs::absolute(fs::current_path());
		dataPath = defaultDataPath();
	}


	std::shared_ptr<ofAppBaseWindow> getCurrentWindow(){
//		if (mainLoop) //mainLoop is always present. is it?
		{
			return mainLoop.currentWindow.lock();
		}
//		return nullptr;
	}

	std::shared_ptr<ofBaseRenderer> & getCurrentRenderer();
	std::vector <std::function<void()>> shutdownFunctions;

	void exit() {
		if(!initialized) return;

		// controlled destruction of the mainLoop before
		// any other deinitialization
		// mainLoop.exit();
//		mainLoop.exit();
		initialized = false;
		exiting = true;

		// all shutdown functions called
		for (const auto & func : shutdownFunctions) {
			func();
		}

	}
};

extern ofCoreInternal ofCore;







void ofInit();
void ofSetupOpenGL(int w, int h, ofWindowMode screenMode); // sets up the opengl context!
std::shared_ptr<ofAppBaseWindow> ofCreateWindow(const ofWindowSettings & settings); // sets up the opengl context!
std::shared_ptr<ofMainLoop> ofGetMainLoop();
//void ofSetMainLoop(const std::shared_ptr<ofMainLoop> & mainLoop);

template <typename Window>
void ofSetupOpenGL(const std::shared_ptr<Window> & windowPtr, int w, int h, ofWindowMode screenMode) {
	ofInit();
	ofWindowSettings settings;
	settings.setSize(w, h);
	settings.windowMode = screenMode;
	ofCore.mainLoop.addWindow(windowPtr);
//	ofGetMainLoop()->addWindow(windowPtr);
	windowPtr->setup(settings);
}

//special case so we preserve supplied settngs
//TODO: remove me when we remove the ofSetupOpenGL legacy approach.
void ofSetupOpenGL(const std::shared_ptr<ofAppGLFWWindow> & windowPtr, int w, int h, ofWindowMode screenMode);

template <typename Window>
static void noopDeleter(Window *) { }

template <typename Window>
void ofSetupOpenGL(Window * windowPtr, int w, int h, ofWindowMode screenMode) {
	std::shared_ptr<Window> window = std::shared_ptr<Window>(windowPtr, std::function<void(Window *)>(noopDeleter<Window>));
	ofSetupOpenGL(window, w, h, screenMode);
}

int ofRunApp(std::shared_ptr<ofBaseApp> && OFSA);
int ofRunApp(ofBaseApp * OFSA = nullptr); // will be deprecated
void ofRunApp(const std::shared_ptr<ofAppBaseWindow> & window, std::shared_ptr<ofBaseApp> && app);
int ofRunMainLoop();

ofBaseApp * ofGetAppPtr();

void ofExit(int status = 0);

bool ofIsCurrentThreadTheMainThread();
std::thread::id ofGetMainThreadId();

//-------------------------- time
float ofGetFrameRate();
float ofGetTargetFrameRate();
bool ofGetTargetFrameRateEnabled();
uint64_t ofGetFrameNum();
void ofSetFrameRate(int targetRate);
double ofGetLastFrameTime();
void ofSetTimeModeSystem();
ofTimeMode ofGetTimeMode();
uint64_t ofGetFixedStepForFps(double fps);
void ofSetTimeModeFixedRate(uint64_t stepNanos = ofGetFixedStepForFps(60)); //default nanos for 1 frame at 60fps
void ofSetTimeModeFiltered(float alpha = 0.9);

void ofSetOrientation(ofOrientation orientation, bool vFlip = true);
ofOrientation ofGetOrientation();

//-------------------------- cursor
void ofHideCursor();
void ofShowCursor();
//-------------------------- window / screen
glm::ivec2 ofGetWindowPosition();
int ofGetWindowPositionX();
int ofGetWindowPositionY();
int ofGetScreenWidth();
int ofGetScreenHeight();
glm::ivec2 ofGetScreenSize();
int ofGetWindowMode();
int ofGetWidth();
int ofGetHeight();
int ofGetWindowWidth();
int ofGetWindowHeight();

std::string ofGetClipboardString();
void ofSetClipboardString(const std::string & str);

/// \returns a random number between 0 and the width of the window.
float ofRandomWidth();

/// \returns a random number between 0 and the height of the window.
float ofRandomHeight();
bool ofDoesHWOrientation();
glm::ivec2 ofGetWindowSize();
ofRectangle ofGetWindowRect();
ofAppBaseWindow * ofGetWindowPtr();
std::shared_ptr<ofAppBaseWindow> ofGetCurrentWindow();

void ofSetWindowPosition(int x, int y);
void ofSetWindowShape(int width, int height);
void ofSetWindowRect(const ofRectangle & rect);
void ofSetWindowTitle(std::string title);
void ofEnableSetupScreen();
void ofDisableSetupScreen();
void ofSetFullscreen(bool fullscreen);
void ofToggleFullscreen();
void ofSetWindowMousePassThrough(bool allowPassThrough);
//-------------------------- sync
void ofSetVerticalSync(bool bSync);

ofCoreEvents & ofEvents();
void ofSetCurrentRenderer(std::shared_ptr<ofBaseRenderer> renderer, bool setDefaults = false);
std::shared_ptr<ofBaseRenderer> & ofGetCurrentRenderer();
void ofSetEscapeQuitsApp(bool bQuitOnEsc);

//-------------------------- native window handles
#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
typedef unsigned long Window;
struct _XDisplay;
typedef struct _XDisplay Display;
Display * ofGetX11Display();
Window ofGetX11Window();
#endif

#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
struct __GLXcontextRec;
typedef __GLXcontextRec * GLXContext;
GLXContext ofGetGLXContext();
#endif

#if defined(TARGET_LINUX) && defined(TARGET_OPENGLES)
EGLDisplay ofGetEGLDisplay();
EGLContext ofGetEGLContext();
EGLSurface ofGetEGLSurface();
#endif

#if defined(TARGET_OSX)
void * ofGetNSGLContext();
void * ofGetCocoaWindow();
#endif

#if defined(TARGET_WIN32)
HGLRC ofGetWGLContext();
HWND ofGetWin32Window();
#endif
