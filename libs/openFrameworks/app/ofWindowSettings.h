#pragma once

class ofAppBaseWindow;

#include <glm/vec2.hpp>

#include <string>
#include <memory>
#include <vector>

/// \brief Used to represent the available windowing modes for the application.
enum ofWindowMode{
	/// \brief A floating application window.
	OF_WINDOW 		= 0,
	/// \brief A fullscreen application window.
	OF_FULLSCREEN 	= 1,
	/// \brief A fullscreen application window with a custom width and height.
	OF_GAME_MODE	= 2,
	
	OF_WINDOWMODE_UNDEFINED	= 3
};


/// \brief Used to represent the available screen orientations.
///
/// These don't use "landscape" or "portrait", because phones typically default
/// to portrait while desktop screens are typically landscape by default.
///
/// \sa ::ofSetOrientation
/// \sa ::ofGetOrientation
enum ofOrientation: short{
	/// \brief Represents the default screen orientation.
	OF_ORIENTATION_DEFAULT = 1,
	/// \brief Represents a screen rotated 180 degrees, also known as upside-down.
	OF_ORIENTATION_180 = 2,
	/// \brief Represents a screen rotated 90 degrees clockwise.
	OF_ORIENTATION_90_LEFT = 3,
	/// \brief Represents a screen rotated 90 degrees counter-clockwise.
	OF_ORIENTATION_90_RIGHT = 4,
	/// \brief Represents an unknown orientation.
	OF_ORIENTATION_UNKNOWN = 5
};

class ofWindowSettings {
public:
	ofWindowSettings() noexcept = default;
	virtual ~ofWindowSettings() noexcept = default;
	
	std::string title;
	std::string windowName { "main" };
	ofWindowMode windowMode = OF_WINDOW;

	bool highResolutionCapable = true;

//	void setRect(const ofRectangle & rect) {
//		setPosition(rect.getPosition());
//		setSize(rect.width, rect.height);
//	}
	
	void setPosition(const glm::ivec2 & position) noexcept {
		this->position = position;
		this->positionSet = true;
	}

	void setSize(int width, int height) noexcept {
		this->width = width;
		this->height = height;
		this->sizeSet = true;
	}

	[[nodiscard]] bool isSizeSet() const noexcept {
		return sizeSet;
	}

	[[nodiscard]] int getWidth() const noexcept {
		return width;
	}

	[[nodiscard]] int getHeight() const noexcept {
		return height;
	}

	[[nodiscard]] const glm::ivec2 & getPosition() const noexcept {
		return position;
	}

	[[nodiscard]] bool isPositionSet() const noexcept {
		return positionSet;
	}

	void setGLVersion(int major, int minor) noexcept {
		glVersionMajor = major;
		glVersionMinor = minor;
	}
	
	void setGLESVersion(int version) noexcept {
		glesVersion = version;
	}

	int glVersionMajor = 2;
	int glVersionMinor = 1;
	
	int glesVersion = 1;
	
	
	// GLFW specific ones
#ifdef TARGET_RASPBERRY_PI
	int numSamples = 0;
#else
	int numSamples = 4;
#endif

	bool doubleBuffering = true;
	int redBits = 8;
	int greenBits = 8;
	int blueBits = 8;
	int alphaBits = 8;
	int depthBits = 24;
	int stencilBits = 0;
	bool stereo = false;
	bool visible = true;
	bool iconified = false;
	bool decorated = true;
	bool floating = false;
	bool resizable = true;
	bool transparent = false;
	bool mousePassThrough = false;
	bool maximized = false;
	int monitor = 0;
	bool multiMonitorFullScreen = false;
	std::shared_ptr<ofAppBaseWindow> shareContextWith;
	std::vector <int> fullscreenDisplays;
	// FIXME: Define proper variable name
	bool showOnlyInSelectedMonitor = false;
	float opacity { 1.0 }; 

protected:
	bool sizeSet { false };
	bool positionSet { false };
	int width { 1024 };
	int height { 768 };
	glm::ivec2 position { 0, 0 };
};

typedef ofWindowSettings ofGLWindowSettings;
typedef ofWindowSettings ofGLESWindowSettings;
