#pragma once

#include "ofAppBaseWindow.h"
#include "ofRectangle.h"
// MARK: Target
#include "ofConstants.h"

#include <stdint.h>
#include <stddef.h>

#if defined(TARGET_RGFW_WINDOW)
	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
typedef struct _XIM * XIM;
typedef struct _XIC * XIC;
	#endif

class ofBaseApp;
struct RGFW_window;
struct RGFW_monitor;
class ofCoreEvents;

template <typename T>
class ofPixels_;
typedef ofPixels_<unsigned char> ofPixels;

[[deprecated("In This Branch ~ use ofWindowSettings instead")]] typedef ofWindowSettings ofRGFWWindowSettings;

class ofAppRGFWWindow : public ofAppBaseWindow {
public:
	ofAppRGFWWindow();
	~ofAppRGFWWindow();

	//	ofWindowMode getWindowMode() { return settings.windowMode; }

	// Can't be copied, use shared_ptr
	ofAppRGFWWindow(ofAppRGFWWindow & w) = delete;
	ofAppRGFWWindow & operator=(ofAppRGFWWindow & w) = delete;

	static void loop() {};
	static bool doesLoop() { return false; }
	static bool allowsMultiWindow() { return true; }
	static bool needsPolling() { return true; }
	void pollEvents();

	// this functions are only meant to be called from inside OF don't call them from your code
	//	using ofAppBaseWindow::setup;

	void setup(const ofWindowSettings & settings) override;
	void update() override;
	void draw() override;
	bool getWindowShouldClose() override;
	void setWindowShouldClose() override;

	void hideCursor() override;
	void showCursor() override;

	int getHeight() override;
	int getWidth() override;

	ofCoreEvents & events() override;
//	std::shared_ptr<ofBaseRenderer> & renderer() override;

	RGFW_window * getRGFWWindow();
	void * getWindowContext() override { return getRGFWWindow(); }
	ofWindowSettings getSettings() { return settings; }

	glm::ivec2 getScreenSize() override;

	ofRectangle getWindowRect() override;
	glm::ivec2 getWindowPosition() override;
	glm::ivec2 getWindowSize() override;

	glm::ivec2 getFramebufferSize() override;

	void setWindowTitle(const std::string & title) override;

	void setWindowRect(const ofRectangle & rect) override;
	void setWindowPosition(int x, int y) override;
	void setWindowShape(int w, int h) override;

	void setFullscreen(bool fullscreen) override;
	void toggleFullscreen() override;

	// MARK: WOW not override
	void setWindowMousePassThrough(bool allowPassThrough);

	void enableSetupScreen() override;
	void disableSetupScreen() override;

	void setVerticalSync(bool bSync) override;

	void setClipboardString(const std::string & text) override;
	std::string getClipboardString() override;

	// MARK: WOW not override
	int getPixelScreenCoordScale();

	void makeCurrent() override;
	void swapBuffers() override;
	void startRender() override;
	void finishRender() override;

	static void listVideoModes();
	static void listMonitors();
	bool isWindowIconified();
	bool isWindowActive();
	bool isWindowResizeable();
	void iconify(bool bIconify);

	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	typedef struct _XIM * XIM;
	typedef struct _XIC * XIC;

	Display * getX11Display();
	Window getX11Window();
	XIC getX11XIC();

	void setWindowIcon(const of::filesystem::path & path);
	void setWindowIcon(const ofPixels & iconPixels);
	#endif

	#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
	GLXContext getGLXContext();
	#endif

	#if defined(TARGET_LINUX) && defined(TARGET_OPENGLES)
	EGLDisplay getEGLDisplay();
	EGLContext getEGLContext();
	EGLSurface getEGLSurface();
	#endif

	#if defined(TARGET_OSX)
	void * getNSGLContext() override;
	void * getCocoaWindow() override;
	#endif

	#if defined(TARGET_WIN32)
	HGLRC getWGLContext();
	HWND getWin32Window();
	#endif

private:
	static ofAppRGFWWindow * setCurrent(RGFW_window * windowP);
	static ofAppRGFWWindow * getWindow(RGFW_window * windowP);
	static void mouse_cb(RGFW_window* win, uint8_t button, double scroll, uint8_t pressed);
	static void motion_cb(RGFW_window* win, struct RGFW_point point, struct RGFW_point vector);
	static void entry_cb(RGFW_window* win, RGFW_point point, uint8_t status);
	static void keyboard_cb(RGFW_window* win, uint8_t key, uint8_t keyChar, uint8_t keyMod, uint8_t repeat, uint8_t pressed);
	static void position_cb(RGFW_window* win, struct RGFW_rect r);
	static void resize_cb(RGFW_window* win, struct RGFW_rect r);
	static void exit_cb(RGFW_window * windowP_);
	static void drop_cb(RGFW_window* win, char** droppedFiles, size_t droppedFilesCount);
	static void error_cb(uint8_t type, uint8_t err, struct RGFW_debugContext ctx, const char* msg);
	static void refresh_cb(RGFW_window * windowP_);

	static void monitor_cb(RGFW_monitor * monitor, int event);
	static void char_cb(RGFW_window * windowP_, uint32_t key);
	static void scroll_cb(RGFW_window * windowP_, double x, double y);
	static void framebuffer_size_cb(RGFW_window * windowP_, int w, int h);
	void close() override;

	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	XIM xim;
	XIC xic;
	#endif

	std::unique_ptr<ofCoreEvents> coreEvents;

	ofWindowMode targetWindowMode;

	bool bEnableSetupScreen;

	ofRectangle windowRect { 20, 20, 800, 600 };
	ofRectangle windowRectBackup;
	void setFSTarget(ofWindowMode targetWindowMode);

	int buttonInUse;
	bool buttonPressed;

	//	int nFramesSinceWindowResized;
	bool bWindowNeedsShowing;

	RGFW_window * windowP;
	ofBaseApp * ofAppPtr;

	bool iconSet;

	void beginDraw() override;
	void endDraw() override;

	//	static ofAppRGFWWindow * thisWindow = static_cast<ofAppRGFWWindow *>(this);


// #ifdef USEDEPRECATED
// 	// window settings, this functions can only be called from main before calling ofSetupOpenGL
// 	// TODO: remove specialized version of ofSetupOpenGL when these go away
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setNumSamples(int samples);
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setDoubleBuffering(bool doubleBuff);
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setColorBits(int r, int g, int b);
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setAlphaBits(int a);
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setDepthBits(int depth);
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setStencilBits(int stencil);
// 	[[deprecated("use ofRGFWWindowSettings to create the window")]] void setMultiDisplayFullscreen(bool bMultiFullscreen); //note this just enables the mode, you have to toggle fullscreen to activate it.
// #endif
};

	// TEMPORARY
	#define RGFWDEF
	#include <RGFW.h>

	// TEMP oftostring
	#include "ofUtils.h"
	#include <vector>

static struct ofMonitors {
public:
	ofMonitors() { }
	~ofMonitors() { }
	std::vector<ofRectangle> rects;
	ofRectangle allMonitorsRect { 0, 0, 0, 0 };
	RGFW_monitor* monitors;

	ofRectangle getRectMonitorForScreenRect(const ofRectangle & rect) {
		update();
		for (unsigned int a = 0; a < rects.size(); a++) {
			if (rects[a].inside(rect.getCenter())) {
				return rects[a];
				break;
			}
		}
		return { 0, 0, 0, 0 };
	}

	ofRectangle getRectForAllMonitors() {
		return allMonitorsRect;
	}

	void update() {
		size_t numberOfMonitors;
		monitors = RGFW_getMonitors(&numberOfMonitors);

		std::cout << "ofMonitors update numberOfMonitors " << numberOfMonitors << std::endl;
		rects.clear();
		allMonitorsRect = { 0, 0, 0, 0 };


		for (int i = 0; i < numberOfMonitors; i++) {
			glm::ivec2 pos = {monitors[i].x, monitors[i].x};
			ofRectangle rect = ofRectangle(pos.x, pos.y, monitors[i].mode.area.w, monitors[i].mode.area.h);
			rects.emplace_back(rect);
			allMonitorsRect = allMonitorsRect.getUnion(rect);
		}
	}

	ofRectangle getRectFromMonitors(const std::vector<int> monitors) {
		bool first = true;
		ofRectangle r;
		std::string str { "" };
		for (auto & i : monitors) {
			str += ofToString(i) + " ";
			if (i < rects.size()) {
				if (first) {
					first = false;
					r = rects[i];
				} else {
					r = r.getUnion(rects[i]);
				}
			}
		}
		return r;
	}
} allMonitors;

#endif
