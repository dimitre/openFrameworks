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

class ofAppGLFWWindow : public ofAppBaseWindow {
public:
	ofAppGLFWWindow();
	~ofAppGLFWWindow();

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

	GLFWwindow * getGLFWWindow();
	void * getWindowContext() override { return getGLFWWindow(); }
	ofWindowSettings getSettings() { return settings; }

	glm::ivec2 getScreenSize() override;
	glm::ivec2 getWindowSize() override;
	glm::ivec2 getFramebufferSize() override;
	glm::ivec2 getWindowPosition() override;
	ofRectangle getWindowRect() override;

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

	void close() override;

	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	XIM xim;
	XIC xic;
	#endif

	std::unique_ptr<ofCoreEvents> coreEvents;

	ofWindowMode targetWindowMode;

	bool bEnableSetupScreen;

	ofRectangle windowRect { 20, 20, 800, 600 };
	ofRectangle windowRectFS { 0, 0, 1920, 1080 };
	void setFSTarget(ofWindowMode targetWindowMode);

	int buttonInUse;
	bool buttonPressed;

	//	int nFramesSinceWindowResized;
	bool bWindowNeedsShowing;

	GLFWwindow * windowP;
	ofBaseApp * ofAppPtr;

	bool iconSet;

	void beginDraw() override;
	void endDraw() override;

	//	static ofAppGLFWWindow * thisWindow = static_cast<ofAppGLFWWindow *>(this);


#ifdef USEDEPRECATED
	// window settings, this functions can only be called from main before calling ofSetupOpenGL
	// TODO: remove specialized version of ofSetupOpenGL when these go away
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setNumSamples(int samples);
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setDoubleBuffering(bool doubleBuff);
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setColorBits(int r, int g, int b);
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setAlphaBits(int a);
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setDepthBits(int depth);
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setStencilBits(int stencil);
	[[deprecated("use ofGLFWWindowSettings to create the window")]] void setMultiDisplayFullscreen(bool bMultiFullscreen); //note this just enables the mode, you have to toggle fullscreen to activate it.
#endif
};

	// TEMPORARY
	#include "GLFW/glfw3.h"
	// TEMP oftostring
	#include "ofUtils.h"
	#include <vector>

static struct ofMonitors {
public:
	ofMonitors() { }
	~ofMonitors() { }
	std::vector<ofRectangle> rects;
	ofRectangle allMonitorsRect { 0, 0, 0, 0 };
	GLFWmonitor ** monitors;

	ofRectangle getRectMonitorForScreenRect(const ofRectangle & rect) {
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
		rects.clear();
		allMonitorsRect = { 0, 0, 0, 0 };

		int numberOfMonitors;
		monitors = glfwGetMonitors(&numberOfMonitors);

		for (int i = 0; i < numberOfMonitors; i++) {
			glm::ivec2 pos;
			glfwGetMonitorPos(monitors[i], &pos.x, &pos.y);
			const GLFWvidmode * desktopMode = glfwGetVideoMode(monitors[i]);
			ofRectangle rect = ofRectangle(pos.x, pos.y, desktopMode->width, desktopMode->height);
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
