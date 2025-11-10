#pragma once

// ============================================================================
// ORGANIC TEXT - CONSTANTS DEFINITIONS
// ============================================================================
// All constant values used across the OrganicText class
// Separated for better code organization and maintainability
// ============================================================================

//----------------------------------------------------------------------------
// OFAPP CONSTANTS
//----------------------------------------------------------------------------

// Default text
constexpr const char * ORGANIC_TEXT_DEFAULT_STRING = "ofWorks";

// Window title
const std::string OFWORKS_DEMO_APP_TITLE = "ofWorks install is complete";

// Window size
constexpr int OFWORKS_DEMO_APP_WIDTH = 1200;
constexpr int OFWORKS_DEMO_APP_HEIGHT = 550;

//----------------------------------------------------------------------------
// MATH CONSTANTS
//----------------------------------------------------------------------------

//TODO: fix for ofWorks... when not using glm lib... ?
#ifndef TWO_PI
constexpr double TWO_PI = 6.283185307179586476925286766559; // 2 * π
#endif

//----------------------------------------------------------------------------
// DRAWING & ANIMATION CONSTANTS
//----------------------------------------------------------------------------

constexpr float ZOOM_GLOBAL_MAX = 1.1f;

// Density Constants
constexpr float DENSITY_SPACING_MIN = 1.5f;
constexpr float DENSITY_SPACING_MAX = 50.0f;

// Animation Displacement Constants
constexpr float ANIM_NOISE_MAX = 35.0f;
constexpr float ANIM_WAVE_MAX = 60.0f;
constexpr float ANIM_SPIRAL_MAX = 45.0f;
constexpr float ANIM_PULSE_MAX = 40.0f;

// Animation Frequency Constants
constexpr float ANIM_WAVE_FREQ_MIN = 0.001f;
constexpr float ANIM_WAVE_FREQ_MAX = 0.04f;
constexpr float ANIM_SPIRAL_TIGHT_MIN = 0.002f;
constexpr float ANIM_SPIRAL_TIGHT_MAX = 0.025f;
constexpr float ANIM_SPEED_MAX = 8.f;

// Time System
constexpr float BASE_TIME_STEP = 0.008f;

// Color Constants
constexpr float COLOR_DISTANCE_MAX = 250.0f;
constexpr float COLOR_DISTANCE_INNER = 0.4f;
constexpr float COLOR_DISTANCE_MIDDLE = 0.7f;
constexpr float COLOR_ALPHA_CENTER = 255.0f;
constexpr float COLOR_ALPHA_EDGE = 80.0f;

// Connection Constants
constexpr int CONNECTIONS_SEARCH_NEAR = 50;
constexpr int CONNECTIONS_SEARCH_FAR = 100;
constexpr int CONNECTIONS_MAX_PER_POINT_NEAR = 3;
constexpr int CONNECTIONS_MAX_PER_POINT_FAR = 8;
constexpr float CONNECTIONS_MAX_LINE_WIDTH = 3.f;

// Trails Constants
constexpr float TRAILS_ALPHA_MAX = 255.0f; // Full alpha range
constexpr float TRAILS_LINE_WIDTH_MAX = 5.f; // Line width
constexpr float TRAILS_LENGTH_MIN = 3.f; // Amount trails per group
constexpr float TRAILS_LENGTH_MAX = 100.f; // Amount trails per group - longer trails
constexpr float TRAILS_FADE_MIN = 0.5f; // More transparent at minimum
constexpr float TRAILS_FADE_MAX = 1.0f; // Fully opaque at maximum

// Shape Constants
constexpr float SHAPE_MAX_RADIUS = 50.0f;
constexpr float SHAPE_MIN_RADIUS = 0.0f;
constexpr float SHAPE_ROTATION_SPEED = 0.2f;
constexpr float SHAPE_SIZE_NOISE_SCALE = 0.5f;
constexpr float SHAPE_SIZE_INDEX_SCALE = 0.01f;

// Mouse Interaction Constants
constexpr float MOUSE_RADIUS_INTERACT_MAX = 100.0f;
constexpr float MOUSE_RADIUS_INTERACT_MIN = 10.0f;
constexpr float MAX_SCALE_POWER = 3.0f;

//----------------------------------------------------------------------------
// ENUMERATIONS
//----------------------------------------------------------------------------

enum ShapeType {
	SHAPE_CIRCLE = 0,
	SHAPE_RECTANGLE = 1,
	SHAPE_TRIANGLE = 2,
	SHAPE_STAR = 3,
	SHAPE_CROSS = 4,
	SHAPE_POINT = 5
};

enum ColorMode {
	COLOR_GLOBAL_1 = 0,
	COLOR_GLOBAL_2 = 1,
	COLOR_GLOBAL_3 = 2,
	COLOR_GLOBAL_MIX = 3,
	COLOR_DISTANCE = 4
};

enum AnimMode {
	ANIM_NOISE = 0,
	ANIM_WAVE = 1,
	ANIM_SPIRAL = 2,
	ANIM_PULSE = 3,
	ANIM_ORBIT = 4
};