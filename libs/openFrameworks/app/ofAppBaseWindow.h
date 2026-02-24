#pragma once

#define GLM_ENABLE_EXPERIMENTAL
#include "ofWindowSettings.h"
// MARK: Target
#include "ofConstants.h"
#include "ofRectangle.h"

class ofBaseApp;
class ofBaseRenderer;
class ofCoreEvents;

#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
struct __GLXcontextRec;
typedef __GLXcontextRec * GLXContext;
#endif

#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
typedef unsigned long Window;
struct _XDisplay;
typedef struct _XDisplay Display;
#endif

class ofAppBaseWindow {
public:

	ofAppBaseWindow() noexcept = default;
	virtual ~ofAppBaseWindow() noexcept = default;

	virtual void setup(const ofWindowSettings & settings)=0;
	virtual void update()=0;
	virtual void draw()=0;
	[[nodiscard]] virtual std::shared_ptr<ofBaseRenderer> & renderer() noexcept { return currentRenderer; }
	virtual ofCoreEvents & events() = 0;

	[[nodiscard]] virtual bool getWindowShouldClose() const noexcept {
		return false;
	}
	virtual void setWindowShouldClose() noexcept {}
	virtual void close() noexcept {}

	virtual void hideCursor() noexcept {}
	virtual void showCursor() noexcept {}

	virtual void setWindowPosition(int, int) noexcept {}
	virtual void setWindowShape(int, int) noexcept {}
	virtual void setWindowRect(const ofRectangle & ) noexcept {}

	[[nodiscard]] virtual glm::ivec2 getWindowPosition() const { return {}; }
	[[nodiscard]] virtual glm::ivec2 getWindowSize() const { return glm::ivec2(); }
	[[nodiscard]] virtual glm::ivec2 getFramebufferSize() const { return glm::ivec2(); }
	[[nodiscard]] virtual glm::ivec2 getScreenSize() const { return glm::ivec2(); }
	[[nodiscard]] virtual ofRectangle getWindowRect() const { return ofRectangle(); }

	virtual void setOrientation(ofOrientation orientationIn) noexcept { orientation = orientationIn; }
	[[nodiscard]] virtual ofOrientation getOrientation() const noexcept { return orientation; }
	[[nodiscard]] virtual bool doesHWOrientation() const noexcept { return false; }

	//this is used by ofGetWidth and now determines the window width based on orientation
	[[nodiscard]] virtual int getWidth() const noexcept { return 0; }
	[[nodiscard]] virtual int getHeight() const noexcept { return 0; }

	virtual void setWindowTitle(const std::string & ) {}

	[[nodiscard]] virtual ofWindowMode getWindowMode() const noexcept { return settings.windowMode; }

	virtual void setFullscreen(bool ) {}
	virtual void toggleFullscreen() {}
	virtual void setWindowMousePassthrough(bool ) {}

	virtual void enableSetupScreen() {}
	virtual void disableSetupScreen() {}

	virtual void setVerticalSync(bool ) {}
	virtual void setClipboardString(const std::string& ) {}
	[[nodiscard]] virtual std::string getClipboardString() const { return ""; }

	virtual void makeCurrent() {}
	virtual void swapBuffers() {}
	virtual void startRender() {}
	virtual void finishRender() {}

	[[nodiscard]] virtual void * getWindowContext() const noexcept { return nullptr; }

	virtual void beginDraw() {}
	virtual void endDraw() {}

#if defined(TARGET_LINUX) && !defined(TARGET_RASPBERRY_PI_LEGACY)
	[[nodiscard]] virtual Display* getX11Display() const noexcept { return nullptr; }
	[[nodiscard]] virtual Window  getX11Window() const noexcept { return 0; }
#endif

#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
	[[nodiscard]] virtual GLXContext getGLXContext() const noexcept { return 0; }
#endif

#if defined(TARGET_LINUX) && defined(TARGET_OPENGLES)
	[[nodiscard]] virtual EGLDisplay getEGLDisplay() const noexcept { return 0; }
	[[nodiscard]] virtual EGLContext getEGLContext() const noexcept { return 0; }
	[[nodiscard]] virtual EGLSurface getEGLSurface() const noexcept { return 0; }
#endif

#if defined(TARGET_OSX)
	[[nodiscard]] virtual void * getNSGLContext() const noexcept { return nullptr; }
	[[nodiscard]] virtual void * getCocoaWindow() const noexcept { return nullptr; }
#endif

#if defined(TARGET_WIN32)
	[[nodiscard]] virtual HGLRC getWGLContext() const noexcept { return 0; }
	[[nodiscard]] virtual HWND getWin32Window() const noexcept { return 0; }
#endif

	ofWindowSettings settings;

//private:
	ofOrientation orientation = OF_ORIENTATION_DEFAULT;
	ofWindowMode windowMode = OF_WINDOW;

	std::shared_ptr<ofBaseRenderer> currentRenderer;

};
