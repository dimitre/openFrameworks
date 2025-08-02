#include "ofAppRGFWWindow.h"
#include "ofEvents.h"

#if defined(TARGET_RGFW_WINDOW)
#include "ofGLProgrammableRenderer.h"
#include "ofGLRenderer.h"

#define RGFW_IMPLEMENTATION
#include <RGFW.h>

#ifdef TARGET_LINUX
	#include "ofIcon.h"
	#include "ofImage.h"
#endif

#ifdef TARGET_OSX
#import "Cocoa/Cocoa.h"
#endif

// using std::numeric_limits;
// using std::shared_ptr;
// using std::vector;
using std::cout;
using std::endl;

//-------------------------------------------------------
ofAppRGFWWindow::ofAppRGFWWindow() : coreEvents(new ofCoreEvents){
	bEnableSetupScreen = true;
	buttonInUse = 0;
	buttonPressed = false;
	bWindowNeedsShowing = true;

	targetWindowMode = OF_WINDOW;

	ofAppPtr = nullptr;

	iconSet = false;
	windowP = nullptr;

	RGFW_setDebugCallback(error_cb);
}

ofAppRGFWWindow::~ofAppRGFWWindow() {
//	cout << "ofAppRGFWWindow::~ofAppRGFWWindow() " << settings.windowName << endl;
	close();
}

void ofAppRGFWWindow::close() {
//	cout << "ofAppRGFWWindow::close! " << settings.windowName << endl;

	if (windowP) {
		//hide the window before we destroy it stops a flicker on OS X on exit.
		RGFW_window_hide(windowP);

		// We must ensure renderer is destroyed *before* RGFW destroys the window in RGFW_window_close,
		// as `RGFW_window_close` at least on Windows has the effect of unloading OpenGL, making all
		// calls to OpenGL illegal.
		currentRenderer.reset();

		RGFW_window_close(windowP);
		windowP = nullptr;
		events().disable();
		bWindowNeedsShowing = true;
	}
}

//------------------------------------------------------------
void ofAppRGFWWindow::setup(const ofWindowSettings & _settings) {
//	cout << "yes recompile OK" << endl;
	if (windowP) {
		ofLogError() << "window already setup, probably you are mixing old and new style setup";
		ofLogError() << "call only ofCreateWindow(settings) or ofSetupOpenGL(...)";
		ofLogError() << "calling window->setup() after ofCreateWindow() is not necessary and won't do anything";
		return;
	}

	settings = _settings;
	RGFW_setHint_OpenGL(RGFW_glRed, settings.redBits);
	RGFW_setHint_OpenGL(RGFW_glGreen, settings.greenBits);
	RGFW_setHint_OpenGL(RGFW_glBlue, settings.blueBits);
	RGFW_setHint_OpenGL(RGFW_glAlpha, settings.alphaBits);
	RGFW_setHint_OpenGL(RGFW_glDepth, settings.depthBits);
	RGFW_setHint_OpenGL(RGFW_glStencil, settings.stencilBits);

	RGFW_setHint_OpenGL(RGFW_glStereo, settings.stereo);
	RGFW_setHint_OpenGL(RGFW_glDoubleBuffer, settings.doubleBuffering ? 1 : 0);
	RGFW_setHint_OpenGL(RGFW_glSamples, settings.numSamples);

#ifdef TARGET_OPENGLES
	RGFW_setHint_OpenGL(RGFW_glMajor, settings.glesVersion);
	RGFW_setHint_OpenGL(RGFW_glMinor, 0);
	RGFW_setHint_OpenGL(RGFW_glProfile, RGFW_glES);
	if (settings.glesVersion >= 2) {
		currentRenderer = std::make_shared<ofGLProgrammableRenderer>(this);
	} else {
		currentRenderer = std::make_shared<ofGLRenderer>(this);
	}
#else
	RGFW_setHint_OpenGL(RGFW_glMajor, settings.glVersionMajor);
	RGFW_setHint_OpenGL(RGFW_glMinor, settings.glVersionMinor);
	if ((settings.glVersionMajor == 3 && settings.glVersionMinor >= 2) || settings.glVersionMajor >= 4) {
		RGFW_setHint_OpenGL(RGFW_glProfile, RGFW_glCore);
	}
	if (settings.glVersionMajor >= 3) {
		/* TODO I'm not sure if this is correct */
		RGFW_setHint_OpenGL(RGFW_glProfile, RGFW_glCompatibility);
		if( settings.mousePassThrough && settings.transparent && settings.decorated) {
			ofLogError("ofAppRGFWWindow") << "window is decorated and has transparent input pass through. use floating...";
		}
		currentRenderer = std::make_shared<ofGLProgrammableRenderer>(this);
	} else {
		currentRenderer = std::make_shared<ofGLRenderer>(this);
	}
#endif

	RGFW_window* sharedContext = NULL;
	if (settings.shareContextWith) {
		sharedContext = (RGFW_window*)settings.shareContextWith->getWindowContext();
		RGFW_window_makeCurrentContext_OpenGL(sharedContext);
		RGFW_setHint_OpenGL(RGFW_glShareWithCurrentContext, RGFW_TRUE);
	}

//	allMonitors.update();
	//	ofLogNotice("ofAppRGFWWindow") << "WINDOW MODE IS " << screenMode;


	RGFW_monitor monitor;


	// FIXME: maybe use as a global variable for the window?
	bool hideWindow = false;

	int monitorIndex = 0;
	// Check to see if desired monitor is connected.

	allMonitors.update();
//	cout << "ofAppRGFWWindow allMonitors size " << allMonitors.rects.size();
	if (allMonitors.rects.size() > settings.monitor) {
		monitorIndex = settings.monitor;
	} else {
		if (settings.showOnlyInSelectedMonitor) {
			hideWindow = true;
		}
		ofLogError("ofAppRGFWWindow") << "requested monitor is: " << settings.monitor << " monitor count is: " << allMonitors.rects.size();
	}

	if (settings.windowMode == OF_GAME_MODE)
	{
		windowRect = allMonitors.rects[monitorIndex];
//		monitor = allMonitors.monitors[monitorIndex];
		monitor = RGFW_getPrimaryMonitor();
	}

//	cout << "will create window" << endl;
//	cout << "monitors size: " << allMonitors.rects.size() << endl;
	bool displayOK = false;
	if (settings.fullscreenDisplays.size()) {
		for (auto & d : settings.fullscreenDisplays) {
//			cout << "RGFW_window fullscreenDisplays " << d << " : " << allMonitors.rects.size() << endl;
			if (d < allMonitors.rects.size()) {
				displayOK = true;
//				break;
			} else {
//				return nullptr;
			}
		}
	} else {
		displayOK = true;
	}

	RGFW_windowFlags flags = RGFW_windowHide;
	if (settings.maximized) flags |= RGFW_windowMaximize;
	if (settings.resizable == 0) flags |= RGFW_windowNoResize;
	if (settings.decorated == 0) flags |= RGFW_windowNoBorder;
	if (settings.transparent) flags |= RGFW_windowTransparent;


	/* TODO GLFW_COCOA_RETINA_FRAMEBUFFER */
	if (settings.highResolutionCapable) {

	}

	RGFW_setWindowRefreshCallback(refresh_cb);
	RGFW_setMouseButtonCallback(mouse_cb);
	RGFW_setMousePosCallback(motion_cb);
	RGFW_setMouseNotifyCallback(entry_cb);
	RGFW_setKeyCallback(keyboard_cb);
	RGFW_setWindowMovedCallback(position_cb);
	RGFW_setWindowQuitCallback(exit_cb);
	RGFW_setDndCallback(drop_cb);
	RGFW_setWindowResizedCallback(resize_cb);

	windowP = RGFW_createWindow(settings.title.c_str(), RGFW_RECT(0, 0, settings.getWidth(), settings.getHeight()), flags);
	/* Do not queue events, we don't use the event queue anyway */
	RGFW_setQueueEvents(RGFW_FALSE);

	if (displayOK) {
		RGFW_window_show(windowP);
	}

	RGFW_window_setMousePassthrough(windowP, RGFW_BOOL(settings.mousePassThrough));
	RGFW_window_setFloating(windowP, settings.floating);

	if (!windowP) {
		ofLogError("ofAppRGFWWindow") << "couldn't create RGFW window";
		return;
	}

	// saves window rectangle just created.
	windowRect = getWindowRect();


	if (settings.windowMode == OF_WINDOW || settings.windowMode == OF_FULLSCREEN) {
		if (settings.isPositionSet()) {
			windowRect.x = settings.getPosition().x;
			windowRect.y = settings.getPosition().y;
		}

		if (monitorIndex > 0) {
			windowRect.x += allMonitors.rects[monitorIndex].x;
			windowRect.y += allMonitors.rects[monitorIndex].y;
			// now it will trigger isPositionSet() as true
			settings.setPosition({ windowRect.x, windowRect.y });
		}

		if (settings.isPositionSet()) {
//			cout << "OWW setWindowRect" << endl;
			setWindowRect(windowRect);
		} else {
//			cout << "OWW setWindowShape " << windowRect << endl;
			setWindowShape(windowRect.width, windowRect.height);
		}
	}




	// MARK: -
	if (settings.windowMode != OF_GAME_MODE) {


#ifdef TARGET_LINUX
//		if (!iconSet) {
//			ofPixels iconPixels;
//			// FIXME: we can have this define in ofIcon.h instead
//	#ifdef DEBUG
//			iconPixels.allocate(ofIconDebug.width, ofIconDebug.height, ofIconDebug.bytes_per_pixel);
//			GIMP_IMAGE_RUN_LENGTH_DECODE(iconPixels.getData(), ofIconDebug.rle_pixel_data, iconPixels.getWidth() * iconPixels.getHeight(), ofIconDebug.bytes_per_pixel);
//	#else
//			iconPixels.allocate(ofIcon.width, ofIcon.height, ofIcon.bytes_per_pixel);
//			GIMP_IMAGE_RUN_LENGTH_DECODE(iconPixels.getData(), ofIcon.rle_pixel_data, iconPixels.getWidth() * iconPixels.getHeight(), ofIcon.bytes_per_pixel);
//	#endif
//			setWindowIcon(iconPixels);
//		}
#endif

		if (settings.iconified) {
			iconify(true);
		}

		if (settings.maximized) {
			RGFW_window_maximize(windowP);
		}
	}

	//don't try and show a window if its been requsted to be hidden
	bWindowNeedsShowing = settings.visible;
	if (hideWindow) {
		bWindowNeedsShowing = false;
//		cout << "HIDE U" << endl;
	}

	windowP->userPtr = this;
	RGFW_window_makeCurrentWindow_OpenGL(windowP);

//	windowMode = settings.windowMode;

#ifndef TARGET_OPENGLES
	static bool inited = false;
	if (!inited) {
		glewExperimental = GL_TRUE;
		GLenum err = glewInit();
		if (GLEW_OK != err) {
			/* Problem: glewInit failed, something is seriously wrong. */
			ofLogError("ofAppRunner") << "couldn't init GLEW: " << glewGetErrorString(err);
			return;
		}
		inited = true;
	}
#endif

	ofLogVerbose() << "GL Version: " << glGetString(GL_VERSION);

	if (currentRenderer->getType() == ofGLProgrammableRenderer::TYPE) {
#ifndef TARGET_OPENGLES
		static_cast<ofGLProgrammableRenderer *>(currentRenderer.get())->setup(settings.glVersionMajor, settings.glVersionMinor);
#else
		static_cast<ofGLProgrammableRenderer *>(currentRenderer.get())->setup(settings.glesVersion, 0);
#endif
	} else {
		static_cast<ofGLRenderer *>(currentRenderer.get())->setup();
	}

//	setVerticalSync(true);

#ifdef TARGET_LINUX
	XSetLocaleModifiers("");
	xim = XOpenIM(getX11Display(), 0, 0, 0);
	if (!xim) {
		// fallback to internal input method
		XSetLocaleModifiers("@im=none");
		xim = XOpenIM(getX11Display(), 0, 0, 0);
	}
	xic = XCreateIC(xim,
		XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
		XNClientWindow, getX11Window(),
		XNFocusWindow, getX11Window(),
		NULL);
#endif

}

#ifdef TARGET_LINUX
    //------------------------------------------------------------
    void ofAppRGFWWindow::setWindowIcon(const of::filesystem::path & path) {
        ofPixels iconPixels;
        ofLoadImage(iconPixels, path);
        setWindowIcon(iconPixels);
    }

    //------------------------------------------------------------
    void ofAppRGFWWindow::setWindowIcon(const ofPixels & iconPixels) {
        iconSet = true;
        int length = 2 + iconPixels.getWidth() * iconPixels.getHeight();
        std::vector<unsigned long> buffer(length);
        buffer[0] = iconPixels.getWidth();
        buffer[1] = iconPixels.getHeight();
        for (size_t i = 0; i < iconPixels.getWidth() * iconPixels.getHeight(); i++) {
            buffer[i + 2] = iconPixels[i * 4 + 3] << 24;
            buffer[i + 2] += iconPixels[i * 4 + 0] << 16;
            buffer[i + 2] += iconPixels[i * 4 + 1] << 8;
            buffer[i + 2] += iconPixels[i * 4 + 2];
        }

        XChangeProperty(getX11Display(), getX11Window(), XInternAtom(getX11Display(), "_NET_WM_ICON", False), XA_CARDINAL, 32,
                        PropModeReplace, (const unsigned char *)buffer.data(), length);
        XFlush(getX11Display());
    }
#endif

//--------------------------------------------
ofCoreEvents & ofAppRGFWWindow::events() {
	return *coreEvents;
}

//--------------------------------------------
//shared_ptr<ofBaseRenderer> & ofAppRGFWWindow::renderer() {
//	return currentRenderer;
//}

//--------------------------------------------
void ofAppRGFWWindow::update() {
	events().notifyUpdate();
	//show the window right before the first draw call.
	if (bWindowNeedsShowing && windowP) {
		// not working.
#ifdef TARGET_OSX
		NSWindow * cocoaWindow = windowP->src.window;
//		[cocoaWindow setLevel:NSScreenSaverWindowLevel + 1];
		[cocoaWindow orderFrontRegardless];
#endif

		bWindowNeedsShowing = false;

		if (settings.windowMode == OF_FULLSCREEN) {
			// Meant to trigger fullscreen forced
			settings.windowMode = OF_WINDOWMODE_UNDEFINED;
			setFullscreen(true);
		} else {
			setFullscreen(false);
		}

		// FIXME: Test only
		if (settings.opacity != 1.0) {
			RGFW_window_setOpacity(windowP, settings.opacity);
		}

//		cout << "SHOW WINDOW! " << settings.windowName << endl;
		RGFW_window_show(windowP);

//		cout << "after show window rect " << getWindowRect() << endl;
	}
}

//--------------------------------------------
void ofAppRGFWWindow::pollEvents() {
	RGFW_pollEvents();
}


//--------------------------------------------
void ofAppRGFWWindow::beginDraw() {
	currentRenderer->startRender();
	if (bEnableSetupScreen) {
		currentRenderer->setupScreen();
	}
}

//--------------------------------------------
void ofAppRGFWWindow::endDraw() {
	RGFW_window_swapBuffers_OpenGL(windowP);
	glFlush();

	currentRenderer->finishRender();
}

//--------------------------------------------
void ofAppRGFWWindow::draw() {
	currentRenderer->startRender();
	if (bEnableSetupScreen) {
		currentRenderer->setupScreen();
	}

	events().notifyDraw();

	RGFW_window_swapBuffers_OpenGL(windowP);
	glFlush();

	currentRenderer->finishRender();
}

//--------------------------------------------
void ofAppRGFWWindow::swapBuffers() {
	RGFW_window_swapBuffers_OpenGL(windowP);
}

//--------------------------------------------
void ofAppRGFWWindow::startRender() {
//	renderer()->startRender();
	currentRenderer->startRender();
}

//--------------------------------------------
void ofAppRGFWWindow::finishRender() {
	currentRenderer->finishRender();
}

//--------------------------------------------
bool ofAppRGFWWindow::getWindowShouldClose() {
	return RGFW_window_shouldClose(windowP);
}

//--------------------------------------------
void ofAppRGFWWindow::setWindowShouldClose() {
	RGFW_window_setShouldClose(windowP, RGFW_TRUE);
}

//------------------------------------------------------------
void ofAppRGFWWindow::setWindowTitle(const std::string & title) {
	settings.title = title;
	RGFW_window_setName(windowP, settings.title.c_str());
}

//------------------------------------------------------------
int ofAppRGFWWindow::getPixelScreenCoordScale() {
	// FIXME: cache?
	RGFW_monitor mon = RGFW_window_getMonitor(windowP);
	return mon.scaleX;
}

//------------------------------------------------------------
ofRectangle ofAppRGFWWindow::getWindowRect() {
//	return windowRect;
	return ofRectangle(windowP->r.x, windowP->r.y, windowP->r.w, windowP->r.h);
}

//------------------------------------------------------------
glm::ivec2 ofAppRGFWWindow::getWindowSize() {
	glm::ivec2 size = {windowP->r.w, windowP->r.h};
	return size;
}

//------------------------------------------------------------
glm::ivec2 ofAppRGFWWindow::getWindowPosition() {
	glm::ivec2 pos = RGFW_POINT(windowP->r.x, windowP->r.y);
	return pos;
}

//------------------------------------------------------------
glm::ivec2 ofAppRGFWWindow::getFramebufferSize() {
	glm::ivec2 size = {windowP->r.w, windowP->r.h};
	return size;
}

//------------------------------------------------------------
glm::ivec2 ofAppRGFWWindow::getScreenSize() {
	// it will return the monitor/screen size where the windows sit.
	windowRect = getWindowRect();
	return allMonitors.getRectMonitorForScreenRect(windowRect).getSize();
}

//------------------------------------------------------------
int ofAppRGFWWindow::getWidth() {
	windowRect = getWindowRect();
	return windowRect.width;
}

//------------------------------------------------------------
int ofAppRGFWWindow::getHeight() {
	windowRect = getWindowRect();
	return windowRect.height;
}

//------------------------------------------------------------
RGFW_window * ofAppRGFWWindow::getRGFWWindow() {
	return windowP;
}

//------------------------------------------------------------
void ofAppRGFWWindow::setWindowRect(const ofRectangle & rect) {
//	cout << settings.windowName << " setWindowRect " << rect << endl;
	windowRect = rect;
	RGFW_monitor_scaleToWindow(RGFW_window_getMonitor(windowP), windowP);
}

//------------------------------------------------------------
void ofAppRGFWWindow::setWindowPosition(int x, int y) {
	RGFW_window_move(windowP, RGFW_POINT(x, y));
}

//------------------------------------------------------------
void ofAppRGFWWindow::setWindowShape(int w, int h) {
//	cout << "setWindowShape " << w << " : " <<  h << endl;
	RGFW_window_resize(windowP, RGFW_AREA(w, h));
}

//------------------------------------------------------------
void ofAppRGFWWindow::hideCursor() {
	if (settings.windowMode == OF_FULLSCREEN || settings.windowMode == OF_GAME_MODE) {
		RGFW_window_holdMouse(windowP, RGFW_AREA(windowP->r.w, windowP->r.h));
	} else {
		RGFW_window_showMouse(windowP, RGFW_FALSE);
	}
};

//------------------------------------------------------------
void ofAppRGFWWindow::showCursor() {
	RGFW_window_showMouse(windowP, RGFW_TRUE);
};

//------------------------------------------------------------
void ofAppRGFWWindow::enableSetupScreen() {
	bEnableSetupScreen = true;
};

//------------------------------------------------------------
void ofAppRGFWWindow::disableSetupScreen() {
	bEnableSetupScreen = false;
};

//------------------------------------------------------------
void ofAppRGFWWindow::setFSTarget(ofWindowMode targetWindowMode) {
	if (targetWindowMode == OF_FULLSCREEN) {

		windowRectBackup = getWindowRect(); // to restore window dimensions on FS exit
		ofRectangle windowRectFS;

		// FIXME: Maybe not needed.
		if (settings.multiMonitorFullScreen) {
			windowRectFS = allMonitors.getRectForAllMonitors();
		} else {
			windowRectFS = allMonitors.getRectMonitorForScreenRect(getWindowRect());
		}

		if (settings.fullscreenDisplays.size()) {
			windowRectFS = allMonitors.getRectFromMonitors(settings.fullscreenDisplays);
		}
		setWindowRect(windowRectFS);
	}

	else if (targetWindowMode == OF_WINDOW) {
		setWindowRect(windowRectBackup);
		setWindowTitle(settings.title);
	}
}

//------------------------------------------------------------
void ofAppRGFWWindow::setFullscreen(bool fullscreen) {
//	cout << "setFullScreen " << fullscreen << " : " << settings.windowName << endl;
	if (fullscreen) {
		targetWindowMode = OF_FULLSCREEN;
	} else {
		targetWindowMode = OF_WINDOW;
	}

	//we only want to change window mode if the requested window is different to the current one.
	if (targetWindowMode == settings.windowMode) return;

#ifdef TARGET_OSX
	NSWindow * cocoaWindow = windowP->src.window;

	if (targetWindowMode == OF_FULLSCREEN) {
		[NSApp setPresentationOptions:NSApplicationPresentationHideMenuBar | NSApplicationPresentationHideDock];
		[cocoaWindow setStyleMask:NSWindowStyleMaskBorderless];
		[cocoaWindow setHasShadow:NO];
	} else {
		// to recover correctly from a green button fullscreen
		if (([cocoaWindow styleMask] & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen) {
			[cocoaWindow toggleFullScreen:nil];
		}

		[NSApp setPresentationOptions:NSApplicationPresentationDefault];
		[cocoaWindow setStyleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskMiniaturizable | NSWindowStyleMaskResizable];
		[cocoaWindow setHasShadow:YES];
	}
	setFSTarget(targetWindowMode);

	//----------------------------------------------------
	//make sure the window is getting the mouse/key events
	[cocoaWindow makeFirstResponder:cocoaWindow.contentView];

#elif defined(TARGET_WIN32)

	HWND hwnd = windowP->src.window;

	setFSTarget(targetWindowMode);

	if (targetWindowMode == OF_FULLSCREEN) {
		SetWindowLong(hwnd, GWL_EXSTYLE, 0);
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
		SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		// TODO: Here discover windows dimensions
		// set window position, shape here.

//		SetWindowPos(hwnd, HWND_TOPMOST, xpos, ypos, fullscreenW, fullscreenH, SWP_SHOWWINDOW);

	} else if (targetWindowMode == OF_WINDOW) {

		DWORD EX_STYLE = WS_EX_OVERLAPPEDWINDOW;
		DWORD STYLE = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_SIZEBOX;

		ChangeDisplaySettings(0, 0);
		SetWindowLong(hwnd, GWL_EXSTYLE, EX_STYLE);
		SetWindowLong(hwnd, GWL_STYLE, STYLE);
		SetWindowPos(hwnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED | SWP_SHOWWINDOW);

		//not sure why this is - but if we don't do this the window shrinks by 4 pixels in x and y
		//should look for a better fix.
//		setWindowPosition(windowRect.x - 2, windowRect.y - 2);
//		setWindowShape(windowRect.width + 4, windowRect.height + 4);
	}

//	SetWindowPos(hwnd, HWND_TOPMOST, xpos, ypos, fullscreenW, fullscreenH, SWP_SHOWWINDOW);
	SetWindowPos(hwnd, HWND_TOPMOST, windowRectFS.x, windowRectFS.y, windowRectFS.width, windowRectFS.height, SWP_SHOWWINDOW);


#elif defined(TARGET_LINUX)
//	#include <X11/Xatom.h>
//
//	Window nativeWin = windowP->src.window;
//	Display * display = windowP->src.display;
//	if (targetWindowMode == OF_FULLSCREEN) {
//
//		// FIXME: Remove legacy code here
//
//		int monitorCount;
//		RGFW_monitor ** monitors = RGFW_getMonitors(&monitorCount);
//		if (settings.multiMonitorFullScreen && monitorCount > 1) {
//			// find the monitors at the edges of the virtual desktop
//			int minx = numeric_limits<int>::max();
//			int miny = numeric_limits<int>::max();
//			int maxx = numeric_limits<int>::min();
//			int maxy = numeric_limits<int>::min();
//			int x, y, w, h;
//			int monitorLeft = 0, monitorRight = 0, monitorTop = 0, monitorBottom = 0;
//			for (int i = 0; i < monitorCount; i++) {
//				x = monitors[i].x; y = monitors[i].y;
//				auto videoMode = monitors[i].mode;
//				w = videoMode->area.w;
//				h = videoMode->area.h;
//				if (x < minx) {
//					monitorLeft = i;
//					minx = x;
//				}
//				if (y < miny) {
//					monitorTop = i;
//					miny = y;
//				}
//				if (x + w > maxx) {
//					monitorRight = i;
//					maxx = x + w;
//				}
//				if (y + h > maxy) {
//					monitorBottom = i;
//					maxy = y + h;
//				}
//			}
//
//			// send fullscreen_monitors event with the edges monitors
//			Atom m_net_fullscreen_monitors = XInternAtom(display, "_NET_WM_FULLSCREEN_MONITORS", false);
//
//			XEvent xev;
//
//			xev.xclient.type = ClientMessage;
//			xev.xclient.serial = 0;
//			xev.xclient.send_event = True;
//			xev.xclient.window = nativeWin;
//			xev.xclient.message_type = m_net_fullscreen_monitors;
//			xev.xclient.format = 32;
//
//			xev.xclient.data.l[0] = monitorTop;
//			xev.xclient.data.l[1] = monitorBottom;
//			xev.xclient.data.l[2] = monitorLeft;
//			xev.xclient.data.l[3] = monitorRight;
//			xev.xclient.data.l[4] = 1;
//			XSendEvent(display, RootWindow(display, DefaultScreen(display)),
//				False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
//
//			// FIXME: review
//			windowRect.width = maxx - minx;
//			windowRect.height = maxy - minx;
////			currentW = maxx - minx;
////			currentH = maxy - minx;
//		} else {
//			auto monitor = RGFW_window_getMonitor(windowP);
//			if (monitor) {
//				auto videoMode = monitor.mode;
//				if (videoMode) {
//					windowRect.width = videoMode->area.w;
//					windowRect.height = videoMode-area.h;
////					currentW = videoMode->area.w;
////					currentH = videoMode->area.h;
//				}
//			}
//		}
//	}
//
//	// send fullscreen event
//	Atom m_net_state = XInternAtom(display, "_NET_WM_STATE", false);
//	Atom m_net_fullscreen = XInternAtom(display, "_NET_WM_STATE_FULLSCREEN", false);
//
//	XEvent xev;
//
//	xev.xclient.type = ClientMessage;
//	xev.xclient.serial = 0;
//	xev.xclient.send_event = True;
//	xev.xclient.window = nativeWin;
//	xev.xclient.message_type = m_net_state;
//	xev.xclient.format = 32;
//
//	if (fullscreen)
//		xev.xclient.data.l[0] = 1;
//	else
//		xev.xclient.data.l[0] = 0;
//
//	xev.xclient.data.l[1] = m_net_fullscreen;
//	xev.xclient.data.l[2] = 0;
//	xev.xclient.data.l[3] = 0;
//	xev.xclient.data.l[4] = 0;
//	XSendEvent(display, RootWindow(display, DefaultScreen(display)),
//		False, SubstructureRedirectMask | SubstructureNotifyMask, &xev);
//
//	// tell the window manager to bypass composition for this window in fullscreen for speed
//	// it'll probably help solving vsync issues
//	Atom m_bypass_compositor = XInternAtom(display, "_NET_WM_BYPASS_COMPOSITOR", False);
//	unsigned long value = fullscreen ? 1 : 0;
//	XChangeProperty(display, nativeWin, m_bypass_compositor, XA_CARDINAL, 32, PropModeReplace, (unsigned char *)&value, 1);
//
//	-(display);
//
//	//	setWindowShape(windowW, windowH);
	setFSTarget(targetWindowMode);

#endif
	settings.windowMode = targetWindowMode;
}

//------------------------------------------------------------
void ofAppRGFWWindow::toggleFullscreen() {
	if (settings.windowMode == OF_GAME_MODE) return;

	if (settings.windowMode == OF_WINDOW) {
		setFullscreen(true);
	} else {
		setFullscreen(false);
	}
}

//------------------------------------------------------------
void ofAppRGFWWindow::setWindowMousePassThrough(bool allowPassThrough) {
	if(settings.mousePassThrough == allowPassThrough) return;
	settings.mousePassThrough = allowPassThrough;
	RGFW_window_setMousePassthrough(getRGFWWindow(),allowPassThrough);
}

//------------------------------------------------------------
// FIXME: this can be up on base class. it is not RGFW exclusive
static void rotateMouseXY(ofOrientation orientation, int w, int h, double & x, double & y) {
	int savedY;
	switch (orientation) {
	case OF_ORIENTATION_180:
		x = w - x;
		y = h - y;
		break;

	case OF_ORIENTATION_90_RIGHT:
		savedY = y;
		y = x;
		x = w - savedY;
		break;

	case OF_ORIENTATION_90_LEFT:
		savedY = y;
		y = h - x;
		x = savedY;
		break;

	case OF_ORIENTATION_DEFAULT:
	default:
		break;
	}
}

//------------------------------------------------------------
ofAppRGFWWindow * ofAppRGFWWindow::setCurrent(RGFW_window * windowP) {
	auto instance = static_cast<ofAppRGFWWindow *>(windowP->userPtr);
//	auto mainLoop = ofGetMainLoop();
//	if (mainLoop) {
//		mainLoop->setCurrentWindow(instance);
//	}
	ofCore.mainLoop.setCurrentWindow(instance);
	instance->makeCurrent();
	return instance;
}

//------------------------------------------------------------
ofAppRGFWWindow * ofAppRGFWWindow::getWindow(RGFW_window * windowP) {
	return static_cast<ofAppRGFWWindow *>(windowP->userPtr);
//	auto instance = static_cast<ofAppRGFWWindow *>(windowP->userPtr);
//	auto mainLoop = ofGetMainLoop();
//	if (mainLoop) {
//		mainLoop->setCurrentWindow(instance);
//	}
//	instance->makeCurrent();
//	return instance;
}

namespace {
	int rgfwtToOFModifiers(int mods) {
		int modifiers = 0;
		if (mods & RGFW_modShift) {
			modifiers |= OF_KEY_SHIFT;
		}
		if (mods & RGFW_modAlt) {
			modifiers |= OF_KEY_ALT;
		}
		if (mods & RGFW_modControl) {
			modifiers |= OF_KEY_CONTROL;
		}
		if (mods & RGFW_modSuper) {
			modifiers |= OF_KEY_SUPER;
		}
		return modifiers;
	}

unsigned long keycodeToUnicode(ofAppRGFWWindow * window, int scancode, int modifier) {



#ifdef TARGET_LINUX
	XkbStateRec xkb_state = {};
	XkbGetState(window->getX11Display(), XkbUseCoreKbd, &xkb_state);
	XEvent ev = { 0 };
	ev.xkey.keycode = scancode;
	ev.xkey.state = xkb_state.mods & ~ControlMask;
	ev.xkey.display = window->getX11Display();
	ev.xkey.type = KeyPress;
	KeySym keysym = NoSymbol;
	int status;
	char buffer[32] = { 0 };
	char * chars = buffer;
	auto count = Xutf8LookupString(window->getX11XIC(), &ev.xkey, chars, sizeof(buffer) - 1, &keysym, &status);
	if ((count > 0 && (status == XLookupChars || status == XLookupBoth)) || status == XLookupKeySym) {
		char ** c = &chars;
		unsigned int ch = 0, count = 0;
		static const unsigned int offsets[] = {
			0x00000000u, 0x00003080u, 0x000e2080u,
			0x03c82080u, 0xfa082080u, 0x82082080u
		};

		do {
			ch = (ch << 6) + (unsigned char)**c;
			(*c)++;
			count++;
		} while ((**c & 0xc0) == 0x80);

		if (count > 6) {
			return 0;
		} else {
			return ch - offsets[count - 1];
		}
	} else {
		return 0;
	}
#endif
#ifdef TARGET_WIN32
	static WCHAR buf[2];
	static BYTE keyboardState[256];
	GetKeyboardState(keyboardState);

	// The way around this is to ask the operating system
	// nicely to create a virtual key for us, based on
	// the scancode and the currently bound keyboard layout.
	// https://msdn.microsoft.com/en-us/library/windows/desktop/ms646306(v=vs.85).aspx
	//
	// create a "fake" virtual key

	UINT fakeVirtualKey = MapVirtualKey(scancode, MAPVK_VSC_TO_VK_EX);

	int ret = ToUnicode(fakeVirtualKey, scancode, keyboardState, buf, 2, 0);

	if (ret == 1) {
		return buf[0];
	} else {
		return 0;
	}
#endif
#ifdef TARGET_OSX
	static UInt32 deadKeyState = 0;
	static UniChar characters[8];
	static UniCharCount characterCount = 0;

	typedef struct __TISInputSource * TISInputSourceRef;
	typedef TISInputSourceRef (*pFnGetInputSource)(void); // define function pointer that may return a input source ref, no arguments
	typedef void * (*pFnGetInputSourceProperty)(TISInputSourceRef, CFStringRef);
	typedef UInt8 (*pFnGetKeyboardType)(void);

	static const CFBundleRef tisBundle = CFBundleGetBundleWithIdentifier(CFSTR("com.apple.HIToolbox"));

	// The most important method is ```UCKeyTranslate``` - everything
	// else here is just a royal preparation party to feed it with the
	// correct parameters.
	//
	// Since these methods are hidden deep within Carbon,
	// we have to first request function pointers to make
	// them callable.
	//
	// We do this only the first time, then we're re-using them,
	// that's why these elements are marked static, and static const.
	//
	static pFnGetInputSource getInputSource = (pFnGetInputSource)CFBundleGetFunctionPointerForName(tisBundle, CFSTR("TISCopyCurrentKeyboardLayoutInputSource"));
	static pFnGetKeyboardType getKeyboardType = (pFnGetKeyboardType)CFBundleGetFunctionPointerForName(tisBundle, CFSTR("LMGetKbdType"));
	static pFnGetInputSourceProperty getInputSourceProperty = (pFnGetInputSourceProperty)CFBundleGetFunctionPointerForName(tisBundle, CFSTR("TISGetInputSourceProperty"));

	static const TISInputSourceRef sourceRef = getInputSource(); // note that for the first time, this creates a copy on the heap, then we're re-using it.

	static const CFStringRef * kPropertyUnicodeKeyLayoutData = (CFStringRef *)CFBundleGetDataPointerForName(tisBundle, CFSTR("kTISPropertyUnicodeKeyLayoutData"));
	static const CFStringRef kTISPropertyUnicodeKeyLayoutData = *kPropertyUnicodeKeyLayoutData;
	static const CFDataRef UnicodeKeyLayoutData = (CFDataRef)getInputSourceProperty(sourceRef, kTISPropertyUnicodeKeyLayoutData);

	static const UCKeyboardLayout * pKeyboardLayout = (UCKeyboardLayout *)CFDataGetBytePtr(UnicodeKeyLayoutData);

	UInt32 mod_OSX = 0;
	{
		// We have to translate the RGFW modifier bitflags back to OS X,
		// so that SHIFT, CONTROL, etc can be taken into account when
		// calculating the unicode codepoint.

		// UCKeyTranslate expects the Carbon-era modifier mask values,
		// so use these instead of the NSEventModifierFlag enums
		if (modifier & RGFW_modShift)
			mod_OSX |= 512; // Carbon shiftKey value
		if (modifier & RGFW_modControl)
			mod_OSX |= 4096; // Carbon controlKey value
		if (modifier & RGFW_modAlt)
			mod_OSX |= 2048; // Carbon optionKey value
		if (modifier & RGFW_modSuper)
			mod_OSX |= 256; // Carbon cmdKey

		// shift into 1 byte as per the Apple docs
		mod_OSX = (mod_OSX >> 8) & 0xFF;
	}

	// All this yak shaving was necessary to feed this diva of a function call:
	// https://developer.apple.com/library/mac/documentation/Carbon/Reference/Unicode_Utilities_Ref/index.html#//apple_ref/c/func/UCKeyTranslate

	if (noErr == UCKeyTranslate(pKeyboardLayout, scancode, kUCKeyActionDisplay, mod_OSX, getKeyboardType(), kUCKeyTranslateNoDeadKeysBit, &deadKeyState, sizeof(characters) / sizeof(characters[0]), &characterCount, characters)) {
		// if successful, first character contains codepoint
		return characters[0];
	} else {
		return 0;
	}

#endif
	return 0;
}
}

//------------------------------------------------------------
void ofAppRGFWWindow::mouse_cb(RGFW_window* windowP_, uint8_t button, double scroll, uint8_t state) {
	auto instance = setCurrent(windowP_);

	switch (button) {
		case RGFW_mouseLeft:
			button = OF_MOUSE_BUTTON_LEFT;
			break;
		case RGFW_mouseRight:
			button = OF_MOUSE_BUTTON_RIGHT;
			break;
		case RGFW_mouseMiddle:
			button = OF_MOUSE_BUTTON_MIDDLE;
			break;
		case RGFW_mouseScrollUp:
			ofAppRGFWWindow::scroll_cb(windowP_, 0, 1); // Positive scroll for up
			return;
		case RGFW_mouseScrollDown:
			ofAppRGFWWindow::scroll_cb(windowP_, 0, -1); // Negative scroll for down
			return;
		default: break;
	}

	instance->buttonInUse = button;

	ofMouseEventArgs::Type action;
	if (state == RGFW_TRUE) {
		action = ofMouseEventArgs::Pressed;
		instance->buttonPressed = true;
	} else {
		action = ofMouseEventArgs::Released;
		instance->buttonPressed = false;
	}

	/* TODO for some reason this wants key mods on a mouse event */
	int modifiers = 0; //rgfwtToOFModifiers(mods);

	ofMouseEventArgs args(action, instance->events().getMouseX(), instance->events().getMouseY(), button, modifiers);

	instance->events().notifyMouseEvent(args);
}

//------------------------------------------------------------
void ofAppRGFWWindow::motion_cb(RGFW_window* windowP_, struct RGFW_point point, struct RGFW_point vector) {
//	auto instance = getWindow(windowP_);
	/*  TODO send vector data when raw data is needed */
	double x = point.x;
	double y = point.y;
	ofAppRGFWWindow * instance = setCurrent(windowP_);
	rotateMouseXY(instance->orientation, instance->getWidth(), instance->getHeight(), x, y);

	ofMouseEventArgs::Type action;
	if (!instance->buttonPressed) {
		action = ofMouseEventArgs::Moved;
	} else {
		action = ofMouseEventArgs::Dragged;
	}

	ofMouseEventArgs args(action,
		x,
		y,
		instance->buttonInUse,
		instance->events().getModifiers());
	instance->events().notifyMouseEvent(args);
}

//------------------------------------------------------------
void ofAppRGFWWindow::entry_cb(RGFW_window* windowP_, RGFW_point point, uint8_t status) {
	ofAppRGFWWindow * instance = setCurrent(windowP_);
//	auto instance = getWindow(windowP_);

	ofMouseEventArgs::Type action;
	if (status) {
		action = ofMouseEventArgs::Entered;
	} else {
		action = ofMouseEventArgs::Exited;
	}

	ofMouseEventArgs args(action,
		instance->events().getMouseX(),
		instance->events().getMouseY(),
		instance->buttonInUse,
		instance->events().getModifiers());
	instance->events().notifyMouseEvent(args);
}

//------------------------------------------------------------
void ofAppRGFWWindow::scroll_cb(RGFW_window * windowP_, double x, double y) {
	ofAppRGFWWindow * instance = setCurrent(windowP_);
//	auto instance = getWindow(windowP_);
	rotateMouseXY(instance->orientation, instance->getWidth(), instance->getHeight(), x, y);

	ofMouseEventArgs args(ofMouseEventArgs::Scrolled,
		instance->events().getMouseX(),
		instance->events().getMouseY(),
		instance->buttonInUse,
		instance->events().getModifiers());
	args.scrollX = x;
	args.scrollY = y;
	instance->events().notifyMouseEvent(args);
}

//------------------------------------------------------------
void ofAppRGFWWindow::drop_cb(RGFW_window* windowP_, char**  droppedFiles, size_t numFiles) {
//	auto instance = getWindow(windowP_);
	ofAppRGFWWindow * instance = setCurrent(windowP_);
	ofDragInfo drag;
	drag.position = { instance->events().getMouseX(), instance->events().getMouseY() };
	drag.files.resize(numFiles);
	for (int i = 0; i < (int)drag.files.size(); i++) {
		drag.files[i] = droppedFiles[i];
	}
	instance->events().notifyDragEvent(drag);
}

//------------------------------------------------------------
void ofAppRGFWWindow::error_cb(RGFW_debugType type, RGFW_errorCode err, RGFW_debugContext ctx, const char* msg) {
	if (type != RGFW_typeError) return;
	ofLogError("ofAppRGFWWindow") << err << ": " << msg;
}

//------------------------------------------------------------

void ofAppRGFWWindow::keyboard_cb(RGFW_window* windowP_, uint8_t keycode, uint8_t keyChar, uint8_t mods, uint8_t repeat, uint8_t action) {
	int key = 0;
	uint32_t codepoint = 0;
	ofAppRGFWWindow * instance = setCurrent(windowP_);
//	auto instance = getWindow(windowP_);
//	cout << "keyboard_cb " << instance->settings.windowName << endl;
	uint32_t scancode = RGFW_rgfwToApiKey(keycode);
	switch (keycode) {
		case RGFW_escape:
			key = OF_KEY_ESC;
			break;
		case RGFW_F1:
			key = OF_KEY_F1;
			break;
		case RGFW_F2:
			key = OF_KEY_F2;
			break;
		case RGFW_F3:
			key = OF_KEY_F3;
			break;
		case RGFW_F4:
			key = OF_KEY_F4;
			break;
		case RGFW_F5:
			key = OF_KEY_F5;
			break;
		case RGFW_F6:
			key = OF_KEY_F6;
			break;
		case RGFW_F7:
			key = OF_KEY_F7;
			break;
		case RGFW_F8:
			key = OF_KEY_F8;
			break;
		case RGFW_F9:
			key = OF_KEY_F9;
			break;
		case RGFW_F10:
			key = OF_KEY_F10;
			break;
		case RGFW_F11:
			key = OF_KEY_F11;
			break;
		case RGFW_F12:
			key = OF_KEY_F12;
			break;
		case RGFW_left:
			key = OF_KEY_LEFT;
			break;
		case RGFW_right:
			key = OF_KEY_RIGHT;
			break;
		case RGFW_up:
			key = OF_KEY_UP;
			break;
		case RGFW_down:
			key = OF_KEY_DOWN;
			break;
		case RGFW_pageUp:
			key = OF_KEY_PAGE_UP;
			break;
		case RGFW_pageDown:
			key = OF_KEY_PAGE_DOWN;
			break;
		case RGFW_home:
			key = OF_KEY_HOME;
			break;
		case RGFW_end:
			key = OF_KEY_END;
			break;
		case RGFW_insert:
			key = OF_KEY_INSERT;
			break;
		case RGFW_shiftL:
			key = OF_KEY_LEFT_SHIFT;
			break;
		case RGFW_controlL:
			key = OF_KEY_LEFT_CONTROL;
			break;
		case RGFW_altL:
			key = OF_KEY_LEFT_ALT;
			break;
		case RGFW_superL:
			key = OF_KEY_LEFT_SUPER;
			break;
		case RGFW_shiftR:
			key = OF_KEY_RIGHT_SHIFT;
			break;
		case RGFW_controlR:
			key = OF_KEY_RIGHT_CONTROL;
			break;
		case RGFW_altR:
			key = OF_KEY_RIGHT_ALT;
			break;
		case RGFW_superR:
			key = OF_KEY_RIGHT_SUPER;
			break;
		case RGFW_backSpace:
			key = OF_KEY_BACKSPACE;
			break;
		case RGFW_delete:
			key = OF_KEY_DEL;
			break;
		case RGFW_enter:
			key = OF_KEY_RETURN;
			codepoint = '\n';
			break;
		case RGFW_KP_Return:
			key = OF_KEY_RETURN;
			codepoint = '\n';
			break;
		case RGFW_tab:
			key = OF_KEY_TAB;
			codepoint = '\t';
			break;
		case RGFW_KP_0:
			key = codepoint = '0';
			break;
		case RGFW_KP_1:
			key = codepoint = '1';
			break;
		case RGFW_KP_2:
			key = codepoint = '2';
			break;
		case RGFW_KP_3:
			key = codepoint = '3';
			break;
		case RGFW_KP_4:
			key = codepoint = '4';
			break;
		case RGFW_KP_5:
			key = codepoint = '5';
			break;
		case RGFW_KP_6:
			key = codepoint = '6';
			break;
		case RGFW_KP_7:
			key = codepoint = '7';
			break;
		case RGFW_KP_8:
			key = codepoint = '8';
			break;
		case RGFW_KP_9:
			key = codepoint = '9';
			break;
		case RGFW_KP_Slash:
			key = codepoint = '/';
			break;
		case RGFW_multiply:
			key = codepoint = '*';
			break;
		case RGFW_KP_Minus:
			key = codepoint = '-';
			break;
		case RGFW_KP_Period:
			key = codepoint = '.';
			break;
		default:
			codepoint = keycodeToUnicode(instance, scancode, mods);
			key = codepoint;
			break;
	}
	int modifiers = rgfwtToOFModifiers(mods);

	if (action == RGFW_TRUE && action) {
		ofKeyEventArgs keyE(ofKeyEventArgs::Pressed, key, keycode, scancode, codepoint, modifiers);
		instance->events().notifyKeyEvent(keyE);
	} else if (repeat && action) {
		ofKeyEventArgs keyE(ofKeyEventArgs::Pressed, key, keycode, scancode, codepoint, modifiers);
		keyE.isRepeat = true;
		instance->events().notifyKeyEvent(keyE);
	} else if (action == RGFW_FALSE) {
		ofKeyEventArgs keyE(ofKeyEventArgs::Released, key, keycode, scancode, codepoint, modifiers);
		instance->events().notifyKeyEvent(keyE);
	}

	ofAppRGFWWindow::char_cb(windowP_, keyChar);
}

//------------------------------------------------------------
void ofAppRGFWWindow::char_cb(RGFW_window * windowP_, uint32_t key) {
	ofAppRGFWWindow * instance = setCurrent(windowP_);
//	auto instance = getWindow(windowP_);
	instance->events().charEvent.notify(key);
}

//------------------------------------------------------------
void ofAppRGFWWindow::refresh_cb(RGFW_window * windowP_) {
//	auto instance = getWindow(windowP_);
	ofAppRGFWWindow * instance = setCurrent(windowP_);
	instance->draw();
//	thisWindow->draw();
}

//------------------------------------------------------------
void ofAppRGFWWindow::monitor_cb(RGFW_monitor * monitor, int event) {
	allMonitors.update();
//	cout << "monitor_cb!" << endl;
//	cout << "monitors rect size: " << allMonitors.rects.size() << endl;

	for (auto & w : ofCore.mainLoop.getWindows()) {
//	for (auto & w : ofGetMainLoop()->getWindows()) {
		if (w->settings.showOnlyInSelectedMonitor) {
//			cout << "monitor_cb windowName " << w->settings.windowName << endl;
			auto win = static_cast<ofAppRGFWWindow *>(w.get());
			if (allMonitors.rects.size() > w->settings.monitor) {
				if (w->settings.windowMode == OF_FULLSCREEN) {
					w->setWindowRect(allMonitors.rects[w->settings.monitor]);
				}
				RGFW_window_moveToMonitor(win->getRGFWWindow(), *monitor);
				RGFW_window_show(win->getRGFWWindow());
			} else {
				RGFW_window_hide(win->getRGFWWindow());
			}
		}
	}
//	ofGetMainLoop()->getWindows().size()
}

//------------------------------------------------------------
void ofAppRGFWWindow::position_cb(RGFW_window* windowP_, struct RGFW_rect r) {
	ofAppRGFWWindow * instance = setCurrent(windowP_);
//	auto instance = getWindow(windowP_);
//	if (instance->settings.windowMode == OF_WINDOW) {
//		instance->windowRect.x = x;
//		instance->windowRect.y = y;
//	}
	instance->events().notifyWindowMoved(r.x, r.y);
}
//------------------------------------------------------------
void ofAppRGFWWindow::resize_cb(RGFW_window* windowP_, struct RGFW_rect r) {
//	auto instance = getWindow(windowP_);
	ofAppRGFWWindow * instance = setCurrent(windowP_);
	int w = r.w, h = r.h;

	instance->events().notifyWindowResized(w, h);
	ofAppRGFWWindow::framebuffer_size_cb(windowP_, w, h);

#if defined(TARGET_OSX)
	if (!instance->bWindowNeedsShowing) {
//		 FIXME - only after first update
		NSWindow * cocoaWindow = windowP_->src.window;
		if (([cocoaWindow styleMask] & NSWindowStyleMaskFullScreen) == NSWindowStyleMaskFullScreen) {
			instance->settings.windowMode = OF_FULLSCREEN;
		} else {
			instance->settings.windowMode = OF_WINDOW;
		}
	}
#endif
}

//------------------------------------------------------------
void ofAppRGFWWindow::framebuffer_size_cb(RGFW_window * windowP_, int w, int h) {
	ofAppRGFWWindow * instance = setCurrent(windowP_);

	instance->currentRenderer->clear();
	instance->events().notifyFramebufferResized(w, h);
}

//--------------------------------------------
void ofAppRGFWWindow::exit_cb(RGFW_window * windowP_) {
//	cout << "exit_cb " <<  endl;
//	auto instance = getWindow(windowP_);
	ofAppRGFWWindow * instance = setCurrent(windowP_);
	instance->events().notifyExit();

	// TODO: handle window closing correctly here.
//	instance->close();
//	instance->setWindowShouldClose();
}

//------------------------------------------------------------
void ofAppRGFWWindow::setVerticalSync(bool bVerticalSync) {
	if (bVerticalSync) {
		RGFW_window_swapInterval_OpenGL(windowP, 1);
	} else {
		RGFW_window_swapInterval_OpenGL(windowP, 0);
	}
}

//------------------------------------------------------------
void ofAppRGFWWindow::setClipboardString(const std::string & text) {
	RGFW_writeClipboard(text.c_str(), text.size());
}

//------------------------------------------------------------
std::string ofAppRGFWWindow::getClipboardString() {
	size_t size;
	const char * clipboard = RGFW_readClipboard(&size);

	if (clipboard) {
		return clipboard;
	} else {
		return "";
	}
}

//------------------------------------------------------------
void ofAppRGFWWindow::listVideoModes() {
	RGFW_init();

	size_t count;
	const auto monitors = RGFW_getMonitors(&count);
	for (int i = 0; i < count; i++) {
		ofLogNotice() << monitors[i].mode.area.w << " x " << monitors[i].mode.area.h
					  << monitors[i].mode.red + monitors[i].mode.green + monitors[i].mode.blue << "bit";
	}
}

//------------------------------------------------------------
void ofAppRGFWWindow::listMonitors() {
	RGFW_init();
	size_t count;
	const auto monitors = RGFW_getMonitors(&count);
	for (int i = 0; i < count; i++) {
		auto monitor = monitors[i];
		int w = monitor.physW, h = monitors->physH, x = monitor.x, y = monitor.y;
		ofLogNotice() << i << ": " << monitor.name<< ", physical size: " << w << "x" << h << "mm at " << x << ", " << y;
	}
}

//------------------------------------------------------------
bool ofAppRGFWWindow::isWindowIconified() {
	return RGFW_window_isMinimized(windowP);
}

//------------------------------------------------------------
bool ofAppRGFWWindow::isWindowActive() {
	return RGFW_window_isInFocus(windowP);
}

//------------------------------------------------------------
bool ofAppRGFWWindow::isWindowResizeable() {
	return !(windowP->_flags & RGFW_windowNoResize);
}

//------------------------------------------------------------
void ofAppRGFWWindow::iconify(bool bIconify) {
	if (bIconify)
		RGFW_window_maximize(windowP);
	else
		RGFW_window_show(windowP);
}

void ofAppRGFWWindow::makeCurrent() {
	RGFW_window_makeCurrentWindow_OpenGL(windowP);
}

#if defined(TARGET_LINUX)
    Display * ofAppRGFWWindow::getX11Display() {
		return _RGFW->display;
	}

    Window ofAppRGFWWindow::getX11Window() {
        return windowP->src.window;
    }

    XIC ofAppRGFWWindow::getX11XIC() {
        return xic;
    }
#endif

#if defined(TARGET_LINUX) && !defined(TARGET_OPENGLES)
    GLXContext ofAppRGFWWindow::getGLXContext() {
        return windowP->src.ctx.ctx;
    }
#endif

#if defined(TARGET_LINUX) && defined(TARGET_OPENGLES)
    EGLDisplay ofAppRGFWWindow::getEGLDisplay() {
        return windowP->src.ctx.EGL_display;;
    }

    EGLContext ofAppRGFWWindow::getEGLContext() {
        return windowP->src.ctx.EGL_context;
    }

    EGLSurface ofAppRGFWWindow::getEGLSurface() {
        return   windowP->src.ctx.EGL_surface;
	}
#endif

#if defined(TARGET_OSX)
    void * ofAppRGFWWindow::getNSGLContext() {
        return (__bridge void *)windowP->src.ctx.ctx;
    }

    void * ofAppRGFWWindow::getCocoaWindow() {
        return (__bridge void *)windowP->src.window;
    }
#endif

#if defined(TARGET_WIN32)
    HGLRC ofAppRGFWWindow::getWGLContext() {
        return windowP->src.ctx.ctx;
    }

    HWND ofAppRGFWWindow::getWin32Window() {
        return windowP->src.window;
    }

#endif

#endif


// DEPRECATED - REMOVE
// #ifdef USEDEPRECATED
// void ofAppRGFWWindow::setNumSamples(int _samples) {
// 	settings.numSamples = _samples;
// }

// void ofAppRGFWWindow::setDoubleBuffering(bool doubleBuff) {
// 	settings.doubleBuffering = doubleBuff;
// }

// void ofAppRGFWWindow::setColorBits(int r, int g, int b) {
// 	settings.redBits = r;
// 	settings.greenBits = g;
// 	settings.blueBits = b;
// }

// void ofAppRGFWWindow::setAlphaBits(int a) {
// 	settings.alphaBits = a;
// }

// void ofAppRGFWWindow::setDepthBits(int depth) {
// 	settings.depthBits = depth;
// }

// void ofAppRGFWWindow::setStencilBits(int stencil) {
// 	settings.stencilBits = stencil;
// }

// void ofAppRGFWWindow::setMultiDisplayFullscreen(bool bMultiFullscreen) {
// 	settings.multiMonitorFullScreen = bMultiFullscreen;
// }
// #endif
