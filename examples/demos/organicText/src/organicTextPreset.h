#pragma once

#include "ofMain.h"

// Forward declaration
class OrganicText;

/**
 * @brief Hardcoded preset based on Kit-00/05.json
 * 
 * This class applies a specific set of parameter values
 * to create a consistent visual preset.
 */
class organicTextPreset {
	
public:
	/// @brief Apply all preset values from 05.json
	static void applyPreset(OrganicText * ot);
	
private:
	// Individual parameter groups
	static void applyAnimationParams(OrganicText * ot);
	static void applyColorModesParams(OrganicText * ot);
	static void applyColorParams(OrganicText * ot);
	static void applyConnectionsParams(OrganicText * ot);
	static void applyDensityParams(OrganicText * ot);
	static void applyShapeParams(OrganicText * ot);
	static void applyMouseTweaksParams(OrganicText * ot);
	static void applyTrailsParams(OrganicText * ot);
	static void applyDrawFlags(OrganicText * ot);
};
