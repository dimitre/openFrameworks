#pragma once

#include "ofMain.h"

#include "organicTextConstants.h"

//----------------------------------------------------------------------------

using namespace glm;

//----

class OrganicText {
public:
	OrganicText();
	~OrganicText();
	
public:
	// Must set before setup()
	void setTargetFPS(float fps);
	
	void setup();
	void setup(float fps);
	
private:
	void setupParams();
	void setupCallbacks();
	void setupScene();
	void startup();
	
public:
	void draw();
	
private:
	void update();
	void update(ofEventArgs & args);
	
public:
	ofParameterGroup paramsFont;
	ofParameterGroup paramsPreset;
	ofParameterGroup paramsShape;
	ofParameterGroup paramsDensity;
	ofParameterGroup paramsColorModes;
	ofParameterGroup paramsColors;
	ofParameterGroup paramsAnim;
	ofParameterGroup paramsConnections;
	ofParameterGroup paramsTrails;
	ofParameterGroup paramsMouseTweaks;
	
	// Basic parameters
	ofParameter<bool> bDrawFill;
	ofParameter<bool> bDrawShapes;
	ofParameter<bool> bEnableAnimation;
	ofParameter<float> zoomGlobal;
	ofParameter<std::string> sText;
	
	// Font parameters
	ofParameter<std::string> fontPath;
	ofParameter<float> fontSize;
	ofParameter<float> letterSpacing;
	ofParameter<void> vResetFont;
	
	// Density parameters
	ofParameter<float> densitySpacing;
	
	// Shape parameters
	ofParameter<int> shapeType;
	ofParameter<std::string> shapeTypeName;
	ofParameter<bool> bShapeBack;
	ofParameter<float> shapeSize;
	ofParameter<float> shapeSizeMin;
	ofParameter<float> shapeRatio;
	ofParameter<float> shapeRotation;
	
	// Color parameters
	ofParameter<int> colorMode;
	ofParameter<std::string> colorModeName;
	ofParameter<float> colorSpeed;
	ofParameter<float> colorMixFactor;
	ofParameter<bool> bColorByDistance;
	ofParameter<float> colorInner;
	ofParameter<float> colorDistMiddle;
	ofParameter<float> colorAlphaRange;
	
	// Colors parameters
	ofParameter<ofColor> color1;
	ofParameter<ofColor> color2;
	ofParameter<ofColor> color3;
	ofParameter<ofColor> colorOutline;
	ofParameter<ofColor> colorConnection;
	ofParameter<ofColor> colorTrails;
	
	// Animation parameters
	ofParameter<int> animationMode;
	ofParameter<std::string> animationModeName;
	ofParameter<float> animSpeed;
	ofParameter<float> animPower;
	ofParameter<float> animWaveFreq;
	ofParameter<float> animIntensity;
	ofParameter<float> animSpiral;
	ofParameter<float> animPulseIntensity;
	ofParameter<float> animOriginX;
	
	// Connection parameters
	ofParameter<bool> bDrawConnections;
	ofParameter<float> connectDistance;
	ofParameter<float> connectLineWidth;
	ofParameter<float> connectAlpha;
	ofParameter<bool> bConnectNearOnly;
	ofParameter<float> connectQuality;
	
	// Trail parameters
	ofParameter<bool> bDrawTrails;
	ofParameter<float> trailLength;
	ofParameter<float> trailLineWidth;
	ofParameter<float> trailFade;
	
	// Mouse tweaks parameters
	ofParameter<float> radiusMouse { "radiusMouse", 0.1, 0, 1 };
	mutable glm::vec2 mousePos;
	ofParameter<bool> bMouseTweaks;
	ofParameter<bool> bMouseControlOrigin;
	ofParameter<bool> bMouseHighlightPoints;
	ofParameter<ofColor> colorMouseHighlight;
	ofParameter<float> mouseInfluenceStrength;
	ofParameter<bool> bMouseDisplacePoints;
	ofParameter<float> mouseDisplacePower;
	ofParameter<bool> bMouseScaleShapes;
	ofParameter<float> mouseScalePower;
	
private:
	// Event listeners
	ofEventListener e_FontPath, e_vFontSize, e_letterSpacing, e_vResetFont;
	ofEventListener e_DensitySpacing, e_sText;
	ofEventListener e_trailLength;
	ofEventListener e_bAutoZoomGlobal;
	
	// Functions
	vector<vec2> sampleStringPoints(const std::string & s, float ds);
	void drawShape(vec2 position, float size, ShapeType shape, float rotation = 0) const;
	void drawShapes();
	
	void drawConnections() const;
	
	void initTrails();
	void updateTrails();
	void drawTrails();
	
	ofColor getPointColor(int index, vec2 position, float phase) const;
	vec2 getAnimatedOffset(int index, float phase) const;
	
	float getMouseInfluence(vec2 position) const;
	
	// Font management
	void loadFont();
	void refreshFont();
	bool bFlagRefreshFont = false; // Avoids multiple calls in a single frame
	void flagRefreshFont();
	
private:
	std::string ORGANIC_TEXT_FONT_DEFAULT;
	ofTrueTypeFont font;
	vec2 textCenter;
	float textWidth;
	float textHeight;
	
	// Data
	vector<vec2> pointsString;
	vector<vector<vec2>> pointTrails;
	mutable vector<vec2> pointsAnimatedCache; // Cache animated positions for reuse
	
	mutable vec2 mouseLocalPos;
	mutable bool bMouseInBounds;
	
public:
	float t; // Time accumulator
	
private:
	void refreshPointsString();
	
	// Fps
	float frameTime;
	float fps;
	float targetFPS = 120.0f;
	
	// Cached connection count for debug display
	mutable int cachedConnectionCount = 0;
};
