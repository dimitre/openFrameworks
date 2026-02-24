#pragma once

#include "ofAppBaseWindow.h"
#include "ofRectangle.h"
// MARK: Target
#include "ofConstants.h"

#if defined(TARGET_GLFW_WINDOW)
	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
typedef struct _XIM * XIM;
typedef struct _XIC * XIC;
	#endif

class ofBaseApp;
struct GLFWwindow;
struct GLFWmonitor;
class ofCoreEvents;

template <typename T>
class ofPixels_;
typedef ofPixels_<unsigned char> ofPixels;

[[deprecated("In This Branch ~ use ofWindowSettings instead")]] typedef ofWindowSettings ofGLFWWindowSettings;

class ofAppGLFWWindow final : public ofAppBaseWindow {
public:
	ofAppGLFWWindow();
	~ofAppGLFWWindow() override;

	//	ofWindowMode getWindowMode() { return settings.windowMode; }

	// Can't be copied, use shared_ptr
	ofAppGLFWWindow(ofAppGLFWWindow & w) = delete;
	ofAppGLFWWindow & operator=(ofAppGLFWWindow & w) = delete;

	static void loop() {};
	static bool doesLoop() { return false; }
	static bool allowsMultiWindow() { return true; }
	static bool needsPolling() { return true; }
	static void pollEvents();

	// this functions are only meant to be called from inside OF don't call them from your code
	//	using ofAppBaseWindow::setup;

	void setup(const ofWindowSettings & settings) override final;
	void update() override final;
	void draw() override final;
	bool getWindowShouldClose() override final;
	void setWindowShouldClose() override final;

	void hideCursor() override final;
	void showCursor() override final;

	int getHeight() override final;
	int getWidth() override final;

	ofCoreEvents & events() override final;
//	std::shared_ptr<ofBaseRenderer> & renderer() override;

	GLFWwindow * getGLFWWindow();
	void * getWindowContext() override final { return getGLFWWindow(); }
	ofWindowSettings getSettings() { return settings; }

	glm::ivec2 getScreenSize() override final;

	ofRectangle getWindowRect() override final;
	glm::ivec2 getWindowPosition() override final;
	glm::ivec2 getWindowSize() override final;

	glm::ivec2 getFramebufferSize() override final;

	void setWindowTitle(const std::string & title) override final;

	void setWindowRect(const ofRectangle & rect) override final;
	void setWindowPosition(int x, int y) override final;
	void setWindowShape(int w, int h) override final;

	void setFullscreen(bool fullscreen) override final;
	void toggleFullscreen() override final;

	// MARK: WOW not override
	void setWindowMousePassThrough(bool allowPassThrough);

	void enableSetupScreen() override final;
	void disableSetupScreen() override final;

	void setVerticalSync(bool bSync) override final;

	void setClipboardString(const std::string & text) override final;
	std::string getClipboardString() override final;

	// MARK: WOW not override
	int getPixelScreenCoordScale();

	void makeCurrent() override final;
	void swapBuffers() override final;
	void startRender() override final;
	void finishRender() override final;

	static void listVideoModes();
	static void listMonitors();
	bool isWindowIconified();
	bool isWindowActive();
	bool isWindowResizeable();
	void iconify(bool bIconify);

	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	typedef struct _XIM * XIM;
	typedef struct _XIC * XIC;

	Display * getX11Display() override;
	Window getX11Window() override;
	XIC getX11XIC();

	void setWindowIcon(const fs::path & path);
	void setWindowIcon(const ofPixels & iconPixels);
	#endif

	#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
	GLXContext getGLXContext() override;
	#endif

	#if defined(TARGET_LINUX) && defined(TARGET_OPENGLES)
	EGLDisplay getEGLDisplay() override;
	EGLContext getEGLContext() override;
	EGLSurface getEGLSurface() override;
	#endif

	#if defined(TARGET_OSX)
	void * getNSGLContext() override final;
	void * getCocoaWindow() override final;
	#endif

	#if defined(TARGET_WIN32)
	HGLRC getWGLContext() override;
	HWND getWin32Window() override;
	#endif

private:
	static ofAppGLFWWindow * setCurrent(GLFWwindow * windowP);
	static ofAppGLFWWindow * getWindow(GLFWwindow * windowP);
	static void mouse_cb(GLFWwindow * windowP_, int button, int state, int mods);
	static void motion_cb(GLFWwindow * windowP_, double x, double y);
	static void entry_cb(GLFWwindow * windowP_, int entered);
	static void keyboard_cb(GLFWwindow * windowP_, int key, int scancode, int action, int mods);
	static void char_cb(GLFWwindow * windowP_, uint32_t key);
	static void position_cb(GLFWwindow * windowP_, int x, int y);
	static void resize_cb(GLFWwindow * windowP_, int w, int h);
	static void framebuffer_size_cb(GLFWwindow * windowP_, int w, int h);
	static void exit_cb(GLFWwindow * windowP_);
	static void scroll_cb(GLFWwindow * windowP_, double x, double y);
	static void drop_cb(GLFWwindow * windowP_, int numFiles, const char ** dropString);
	static void error_cb(int errorCode, const char * errorDescription);
	static void refresh_cb(GLFWwindow * windowP_);
	static void monitor_cb(GLFWmonitor * monitor, int event);

	void close() override final;

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

	GLFWwindow * windowP;
	ofBaseApp * ofAppPtr;

	bool iconSet;

	void beginDraw() override final;
	void endDraw() override final;

	//	static ofAppGLFWWindow * thisWindow = static_cast<ofAppGLFWWindow *>(this);


// #ifdef USEDEPRECATED
// 	// window settings, this functions can only be called from main before calling ofSetupOpenGL
// 	// TODO: remove specialized version of ofSetupOpenGL when these go away
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setNumSamples(int samples);
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setDoubleBuffering(bool doubleBuff);
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setColorBits(int r, int g, int b);
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setAlphaBits(int a);
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setDepthBits(int depth);
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setStencilBits(int stencil);
// 	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setMultiDisplayFullscreen(bool bMultiFullscreen); //note this just enables the mode, you have to toggle fullscreen to activate it.
// #endif
};

	// TEMPORARY
	#include <GLFW/glfw3.h>

	// TEMP
	#include "ofUtils.h" // ofToString
	#include <vector>

inline struct ofMonitors {
public:
	ofMonitors() { }
	~ofMonitors() { }
	std::vector<ofRectangle> rects;
	ofRectangle allMonitorsRect { 0, 0, 0, 0 };
	GLFWmonitor ** monitors;

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
		int numberOfMonitors;
		monitors = glfwGetMonitors(&numberOfMonitors);
		//ofGetFrameNum() <<
		std::cout <<  "ofMonitors update numberOfMonitors " << numberOfMonitors << std::endl;
		rects.clear();
		allMonitorsRect = { 0, 0, 0, 0 };


		for (int i = 0; i < numberOfMonitors; i++) {
			glm::ivec2 pos;
			glfwGetMonitorPos(monitors[i], &pos.x, &pos.y);
			const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[i]);
			if (desktopMode != NULL) {
				ofRectangle rect = ofRectangle(pos.x, pos.y, desktopMode->width, desktopMode->height);
				rects.emplace_back(rect);
				allMonitorsRect = allMonitorsRect.getUnion(rect);
			}
		}
	}

	ofRectangle getRectFromMonitors(const std::vector<int> monitors) {
		bool first = true;
		ofRectangle r;
		std::string str { "" };
		for (auto & i : monitors) {
			str += ofToString(i) + " ";
			if (i < (int)rects.size()) {
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
