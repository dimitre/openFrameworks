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
struct GLFWcursor;
class ofCoreEvents;

template <typename T>
class ofPixels_;
typedef ofPixels_<unsigned char> ofPixels;

[[deprecated("In This Branch ~ use ofWindowSettings instead")]] typedef ofWindowSettings ofGLFWWindowSettings;

class ofAppGLFWWindow final : public ofAppBaseWindow {
public:
	ofAppGLFWWindow() noexcept;
	~ofAppGLFWWindow() override;

	//	ofWindowMode getWindowMode() { return settings.windowMode; }

	// Can't be copied, use shared_ptr
	ofAppGLFWWindow(const ofAppGLFWWindow & w) = delete;
	ofAppGLFWWindow & operator=(const ofAppGLFWWindow & w) = delete;

	static void loop() noexcept {}
	[[nodiscard]] static bool doesLoop() noexcept { return false; }
	[[nodiscard]] static bool allowsMultiWindow() noexcept { return true; }
	[[nodiscard]] static bool needsPolling() noexcept { return true; }
	static void pollEvents();

	// this functions are only meant to be called from inside OF don't call them from your code
	//	using ofAppBaseWindow::setup;

	void setup(const ofWindowSettings & settings) override final;
	void update() override final;
	void draw() override final;
	[[nodiscard]] bool getWindowShouldClose() const noexcept override final;
	void setWindowShouldClose() noexcept override final;

	void hideCursor() noexcept override final;
	void showCursor() noexcept override final;

	[[nodiscard]] int getHeight() const noexcept override final;
	[[nodiscard]] int getWidth() const noexcept override final;

	ofCoreEvents & events() override final;
//	std::shared_ptr<ofBaseRenderer> & renderer() override;

	[[nodiscard]] GLFWwindow * getGLFWWindow() const;
	[[nodiscard]] void * getWindowContext() const noexcept override final { return getGLFWWindow(); }
	[[nodiscard]] ofWindowSettings getSettings() const { return settings; }

	[[nodiscard]] glm::ivec2 getScreenSize() const override final;

	[[nodiscard]] ofRectangle getWindowRect() const override final;
	[[nodiscard]] glm::ivec2 getWindowPosition() const override final;
	[[nodiscard]] glm::ivec2 getWindowSize() const override final;

	[[nodiscard]] glm::ivec2 getFramebufferSize() const override final;

	void setWindowTitle(const std::string & title) override final;

	void setWindowRect(const ofRectangle & rect) noexcept override final;
	void setWindowPosition(int x, int y) noexcept override final;
	void setWindowShape(int w, int h) noexcept override final;

	void setFullscreen(bool fullscreen) override final;
	void toggleFullscreen() override final;

	// MARK: WOW not override
	void setWindowMousePassThrough(bool allowPassThrough);

	void enableSetupScreen() override final;
	void disableSetupScreen() override final;

	void setVerticalSync(bool bSync) override final;

	void setClipboardString(const std::string & text) override final;
	[[nodiscard]] std::string getClipboardString() const override final;

	// MARK: WOW not override
	[[nodiscard]] int getPixelScreenCoordScale() const;

	void makeCurrent() override final;
	void swapBuffers() override final;
	void startRender() override final;
	void finishRender() override final;

	static void listVideoModes();
	static void listMonitors();
	[[nodiscard]] bool isWindowIconified() const;
	[[nodiscard]] bool isWindowActive() const;
	[[nodiscard]] bool isWindowResizeable() const;
	void iconify(bool bIconify);

	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	typedef struct _XIM * XIM;
	typedef struct _XIC * XIC;

	[[nodiscard]] Display * getX11Display() const noexcept override;
	[[nodiscard]] Window getX11Window() const noexcept override;
	[[nodiscard]] XIC getX11XIC() const;

	void setWindowIcon(const fs::path & path);
	void setWindowIcon(const ofPixels & iconPixels);
	#endif

	#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
	[[nodiscard]] GLXContext getGLXContext() const noexcept override;
	#endif

	#if defined(TARGET_LINUX) && defined(TARGET_OPENGLES)
	[[nodiscard]] EGLDisplay getEGLDisplay() const noexcept override;
	[[nodiscard]] EGLContext getEGLContext() const noexcept override;
	[[nodiscard]] EGLSurface getEGLSurface() const noexcept override;
	#endif

	#if defined(TARGET_OSX)
	[[nodiscard]] void * getNSGLContext() const noexcept override final;
	[[nodiscard]] void * getCocoaWindow() const noexcept override final;
	#endif

	#if defined(TARGET_WIN32)
	[[nodiscard]] HGLRC getWGLContext() const noexcept override;
	[[nodiscard]] HWND getWin32Window() const noexcept override;
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

	void close() noexcept override final;

	#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	XIM xim = nullptr;
	XIC xic = nullptr;
	#endif

	std::unique_ptr<ofCoreEvents> coreEvents;

	ofWindowMode targetWindowMode = OF_WINDOW;

	bool bEnableSetupScreen = true;

	mutable ofRectangle windowRect { 20, 20, 800, 600 };
	ofRectangle windowRectBackup;
	void setFSTarget(ofWindowMode targetWindowMode);

	int buttonInUse = 0;
	bool buttonPressed = false;

	//	int nFramesSinceWindowResized;
	bool bWindowNeedsShowing = false;

	GLFWwindow * windowP = nullptr;
	ofBaseApp * ofAppPtr = nullptr;
	GLFWcursor * standardCursor = nullptr;

	bool iconSet = false;

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
	ofMonitors() = default;
	~ofMonitors() = default;
	
	// Delete copy to prevent accidental copying of monitor handles
	ofMonitors(const ofMonitors&) = delete;
	ofMonitors& operator=(const ofMonitors&) = delete;
	ofMonitors(ofMonitors&&) = default;
	ofMonitors& operator=(ofMonitors&&) = default;
	
	std::vector<ofRectangle> rects;
	ofRectangle allMonitorsRect { 0, 0, 0, 0 };
	GLFWmonitor ** monitors = nullptr;

	[[nodiscard]] ofRectangle getRectMonitorForScreenRect(const ofRectangle & rect) {
		update();
		for (unsigned int a = 0; a < rects.size(); a++) {
			if (rects[a].inside(rect.getCenter())) {
				return rects[a];
				break;
			}
		}
		return { 0, 0, 0, 0 };
	}

	[[nodiscard]] ofRectangle getRectForAllMonitors() const {
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

	[[nodiscard]] ofRectangle getRectFromMonitors(const std::vector<int>& monitors) {
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
