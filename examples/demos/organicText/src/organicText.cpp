#include "organicText.h"

//--------------------------------------------------------------
OrganicText::OrganicText() {
	ofLogVerbose("OrganicText") << "OrganicText()";
	
	ofAddListener(ofEvents().update, this, &OrganicText::update);
}

//--------------------------------------------------------------
OrganicText::~OrganicText() {
	ofLogNotice("OrganicText") << "~OrganicText()";
	
	ofRemoveListener(ofEvents().update, this, &OrganicText::update);
}

//--------------------------------------------------------------
void OrganicText::setTargetFPS(float fps) {
	ofLogNotice("OrganicText") << "setTargetFPS() fps:" << fps;
	
	targetFPS = fps;
}

//--------------------------------------------------------------
void OrganicText::setup(float fps) {
	ofLogNotice("OrganicText") << "setup() fps:" << fps;
	
	targetFPS = fps;
	setup();
}

//--------------------------------------------------------------
void OrganicText::setup() {
	ofLogNotice("OrganicText") << "setup()";
	
	// Default font
	//ORGANIC_TEXT_FONT_DEFAULT = "NotoSansMono-Regular.ttf";
	
	// Bundled OF fonts
	ORGANIC_TEXT_FONT_DEFAULT = OF_TTF_SANS;
	
	setupScene();
	setupParams();
	setupCallbacks();
	startup();
	
	ofLogNotice("OrganicText") << "Setup complete - Target FPS: " << targetFPS;
}

//--------------------------------------------------------------
void OrganicText::startup() {
	ofLogNotice("OrganicText") << "startup()";
	
	loadFont();
}

//--------------------------------------------------------------
void OrganicText::setupScene() {
	ofLogNotice("OrganicText") << "setupScene()";
	
	ofSetCircleResolution(100);
	
	// Initialize
	t = 0.0;
	textCenter = vec2(0, 0);
	textWidth = 0.0f;
	textHeight = 0.0f;
	mouseLocalPos = vec2(0, 0);
	bMouseInBounds = false;
}

//--------------------------------------------------------------
void OrganicText::setupParams() {
	ofLogNotice("OrganicText") << "setupParams()";
	
	// Basic parameters
	zoomGlobal.set("Zoom", 0.2f, 0.0f, 1.0f);
	bDrawFill.set("Draw Fill", true);
	bDrawShapes.set("Draw Shapes", true);
	bEnableAnimation.set("Animate", true);
	sText.set("Text", ORGANIC_TEXT_DEFAULT_STRING);
	bMouseTweaks.set("Mouse Tweaks", true);
	
	// Font parameters
	fontPath.set("Font Path", ORGANIC_TEXT_FONT_DEFAULT); // File not required! Currently using OF bundled OF_TTF
	fontSize.set("Size", 150, 50, 500);
	letterSpacing.set("Spacing", 0, -1, 1);
	vResetFont.set("Reset");
	
	// Density group
	densitySpacing.set("Spacing", 0.2f, 0.05f, 1.f);
	
	// Shape group
	shapeType.set("Type", 0, 0, 5);
	shapeTypeName.set("Name", "Circle");
	shapeTypeName.setSerializable(false);
	bShapeBack.set("Back", false);
	shapeSize.set("Size", 0.5, 0, 1);
	shapeSizeMin.set("Min Size", 0.3, 0, 1);
	shapeRatio.set("Ratio", .5, 0, 1.0f);
	shapeRotation.set("Rotation", 0, 0, 1.0f);
	
	// Color group
	colorMode.set("Mode", 3, 0, 4);
	colorModeName.set("Mode Name", "Mix");
	colorModeName.setSerializable(false);
	colorSpeed.set("Speed", 1.0, 0.1, 5.0);
	colorMixFactor.set("Mix", 0.5, 0.0, 1.0);
	bColorByDistance.set("By Dist", false);
	colorInner.set("Inner", 0.4, 0.0, 1.0);
	colorDistMiddle.set("Dist Middle", 0.7, 0.0, 1.0);
	colorAlphaRange.set("Alpha Range", 0.7, 0.0, 1.0);
	
	// Colors group
	color1.set("Color 1", ofColor::cyan);
	color2.set("Color 2", ofColor::magenta);
	color3.set("Color 3", ofColor::yellow);
	colorOutline.set("Color Outline", ofColor::black);
	colorConnection.set("Color Connections", ofColor::white);
	colorTrails.set("Color Trails", ofColor::black);
	
	// Animation group
	animationMode.set("Mode", 0, 0, 4);
	animationModeName.set("Name", "Noise");
	animationModeName.setSerializable(false);
	animSpeed.set("Speed", 0.2f, 0.0f, 1.0f);
	animPower.set("Power", 0.1, 0.0, 1.0);
	animWaveFreq.set("Wave Freq", 0.3, 0.0, 1.0);
	animIntensity.set("Intensity", 0.2, 0.0, 1.0);
	animSpiral.set("Spiral", 0.2, 0.0, 1.0);
	animPulseIntensity.set("Pulse", 0.2, 0.0, 1.0);
	animOriginX.set("Origin X", 0.5, 0.0, 1.0);
	
	// Connection group
	bDrawConnections.set("Draw Connections", false);
	connectDistance.set("Distance", 30, 5, 100);
	connectLineWidth.set("Line Width", 1.0f, 0.1f, CONNECTIONS_MAX_LINE_WIDTH);
	connectAlpha.set("Alpha", 0.5, 0, 1);
	bConnectNearOnly.set("Near Only", true);
	connectQuality.set("Quality", 1.0, 0.1, 1.0);
	
	// Trail
	bDrawTrails.set("Draw Trails", false);
	trailLength.set("Length", 0.5f, 0, 1.f);
	trailLineWidth.set("Line Width", 1.0f, 0.1f, TRAILS_LINE_WIDTH_MAX);
	trailFade.set("Fade", 0.5f, 0, 1.f);
	
	// Mouse Tweaks
	bMouseControlOrigin.set("x Origin", false);
	bMouseHighlightPoints.set("Highlight", false);
	colorMouseHighlight.set("Color", ofColor(0, 150, 255));
	mouseInfluenceStrength.set("Influence", 0.5, 0.0, 1.0);
	bMouseDisplacePoints.set("Displace", false);
	mouseDisplacePower.set("D Power", 0.5, 0.0, 1.0);
	bMouseScaleShapes.set("Scale", false);
	mouseScalePower.set("S Power", 0.5, 0.0, 1.0);
	
	// Fonts groups
	paramsFont.setName("Font");
	paramsFont.add(fontPath);
	paramsFont.add(fontSize);
	paramsFont.add(letterSpacing);
	paramsFont.add(vResetFont);
	
	paramsShape.setName("Shape");
	paramsShape.add(bDrawFill);
	paramsShape.add(bShapeBack);
	paramsShape.add(shapeType);
	paramsShape.add(shapeTypeName);
	paramsShape.add(shapeSize);
	paramsShape.add(shapeSizeMin);
	paramsShape.add(shapeRatio);
	paramsShape.add(shapeRotation);
	
	paramsDensity.setName("Density");
	paramsDensity.add(densitySpacing);
	
	paramsColors.setName("Colors");
	paramsColors.add(color1);
	paramsColors.add(color2);
	paramsColors.add(color3);
	paramsColors.add(colorConnection);
	paramsColors.add(colorTrails);
	
	paramsColorModes.setName("Color Modes");
	paramsColorModes.add(colorMode);
	paramsColorModes.add(colorModeName);
	paramsColorModes.add(colorSpeed);
	paramsColorModes.add(colorMixFactor);
	paramsColorModes.add(bColorByDistance);
	paramsColorModes.add(colorInner);
	paramsColorModes.add(colorDistMiddle);
	paramsColorModes.add(colorAlphaRange);
	
	paramsAnim.setName("Animation");
	paramsAnim.add(bEnableAnimation);
	paramsAnim.add(animationMode);
	paramsAnim.add(animationModeName);
	paramsAnim.add(animSpeed);
	paramsAnim.add(animPower);
	paramsAnim.add(animWaveFreq);
	paramsAnim.add(animIntensity);
	paramsAnim.add(animSpiral);
	paramsAnim.add(animPulseIntensity);
	paramsAnim.add(animOriginX);
	
	paramsConnections.setName("Connections");
	paramsConnections.add(bDrawConnections);
	paramsConnections.add(connectDistance);
	paramsConnections.add(connectLineWidth);
	paramsConnections.add(connectAlpha);
	paramsConnections.add(connectQuality);
	paramsConnections.add(bConnectNearOnly);
	
	paramsTrails.setName("Trails");
	paramsTrails.add(bDrawTrails);
	paramsTrails.add(trailLineWidth);
	paramsTrails.add(trailLength);
	paramsTrails.add(trailFade);
	
	paramsMouseTweaks.setName("Mouse Tweaks");
	paramsMouseTweaks.add(bMouseControlOrigin);
	paramsMouseTweaks.add(radiusMouse);
	paramsMouseTweaks.add(bMouseHighlightPoints);
	paramsMouseTweaks.add(colorMouseHighlight);
	paramsMouseTweaks.add(mouseInfluenceStrength);
	paramsMouseTweaks.add(bMouseDisplacePoints);
	paramsMouseTweaks.add(mouseDisplacePower);
	paramsMouseTweaks.add(bMouseScaleShapes);
	paramsMouseTweaks.add(mouseScalePower);
	
	// Presets parameters
	// Group all relevant params for presets management
	paramsPreset.setName("OrganicText");
	paramsPreset.add(bDrawShapes);
	paramsPreset.add(bDrawFill);
	paramsPreset.add(bDrawConnections);
	paramsPreset.add(bDrawTrails);
	paramsPreset.add(bEnableAnimation);
	paramsPreset.add(paramsShape);
	paramsPreset.add(paramsDensity);
	paramsPreset.add(paramsColors);
	paramsPreset.add(paramsColorModes);
	paramsPreset.add(paramsAnim);
	paramsPreset.add(paramsConnections);
	paramsPreset.add(paramsTrails);
	paramsPreset.add(paramsMouseTweaks);
}

//--------------------------------------------------------------
void OrganicText::setupCallbacks() {
	ofLogNotice("OrganicText") << "setupCallbacks()";
	
	// Font listeners
	e_FontPath = fontPath.newListener([this](std::string & s) { loadFont(); });
	e_vFontSize = fontSize.newListener([this](float & f) { loadFont(); });
	e_letterSpacing = letterSpacing.newListener([this](float & f) { flagRefreshFont(); });
	
	// Density listeners
	e_DensitySpacing = densitySpacing.newListener([this](float & v) { refreshPointsString(); });
	e_sText = sText.newListener([this](std::string & s) { refreshPointsString(); });
	
	// Trail listener
	e_trailLength = trailLength.newListener([this](float & v) { initTrails(); });
}

//--------------------------------------------------------------
void OrganicText::flagRefreshFont() {
	ofLogNotice("OrganicText") << "flagRefreshFont()";
	
	bFlagRefreshFont = true;
}

//--------------------------------------------------------------
void OrganicText::loadFont() {
	ofLogNotice("OrganicText") << "loadFont()";
	
	bool success = font.load(fontPath.get(), fontSize.get(), false, false, true);
	
	if (success) {
		ofLogNotice("OrganicText") << "Font loaded: " << fontPath.get() << " @ " << fontSize.get() << "px";
	} else {
		ofLogError("OrganicText") << "Failed to load font: " << fontPath.get();
		
		success = font.load(fontPath.get(), fontSize.get(), false, false, true);
		if (success) {
			ofLogNotice("OrganicText") << "Forced font loaded: " << fontPath.get() << " @ " << fontSize.get() << "px";
		} else {
			ofLogError("OrganicText") << "Failed to load font: " << fontPath.get();
			ofLogFatalError("OrganicText") << "Failed last try to load OF_TTF font.";
			return;
		}
	}
}

//--------------------------------------------------------------
void OrganicText::refreshFont() {
	ofLogNotice("OrganicText") << "refreshFont(): " << fontPath.get();
	
	font.setSpaceSize(font.getSpaceSize() * letterSpacing);
	
	const float spMin = 0.2f;
	const float spMax = 4.f;
	float sp = 1.f;
	if (letterSpacing < 0)
		sp = ofMap(letterSpacing, 0, -1, 1, spMin, true);
	else if (letterSpacing > 0)
		sp = ofMap(letterSpacing, 0, 1, 1, spMax, true);
	font.setLetterSpacing(sp);
	
	refreshPointsString();
}

//--------------------------------------------------------------
void OrganicText::update(ofEventArgs & args) {
	update();
}

//--------------------------------------------------------------
void OrganicText::update() {
	if (bEnableAnimation.get()) {
		float dt = ofGetLastFrameTime();
		float normalizedDt = dt / (1.0f / targetFPS);
		t += BASE_TIME_STEP * animSpeed.get() * ANIM_SPEED_MAX * normalizedDt;
	}
	
	fps = ofGetFrameRate();
	frameTime = 1000.0f / ofClamp(fps, 0.1f, 10000.0f);
	
	if (bFlagRefreshFont) {
		refreshFont();
		bFlagRefreshFont = false;
	}
	
	//--
	
	// Mouse coordinate transformation
	// Convert window coordinates to local text coordinates
	mousePos = glm::vec2(ofGetMouseX(), ofGetMouseY());
	
	// Apply inverse transformations (same as in draw())
	float zoomFactor = 1.0f + (zoomGlobal.get() * ZOOM_GLOBAL_MAX);
	float centerX = ofGetWidth() * 0.5f;
	float centerY = ofGetHeight() * 0.5f;
	
	// Step 1: Translate from window to center
	vec2 translated = mousePos - vec2(centerX, centerY);
	
	// Step 2: Inverse scale (divide by zoom)
	vec2 scaled = translated / zoomFactor;
	
	// Step 3: Translate to text origin (inverse of text centering)
	float textOffsetX = -textWidth * 0.5f;
	float textOffsetY = textHeight * 0.5f;
	mouseLocalPos = scaled - vec2(textOffsetX, textOffsetY);
	
	// Check if mouse is within text bounds
	bMouseInBounds = (mouseLocalPos.x >= 0 && mouseLocalPos.x <= textWidth && mouseLocalPos.y >= -textHeight && mouseLocalPos.y <= 0);
}

// Samples points along the outlines of the given string at intervals of ds.		
//--------------------------------------------------------------
vector<vec2> OrganicText::sampleStringPoints(const std::string & s, float ds) {
	ofLogNotice("OrganicText") << "sampleStringPoints() s:"<< s << ", ds:" << ds;
	
	vector<vec2> points;
	if (s.empty()) return points;
	
	vector<ofPath> paths = font.getStringAsPoints(s, true, false);
	
	for (auto & path : paths) {
		vector<ofPolyline> polylines = path.getOutline();
		
		for (auto & polyline : polylines) {
			if (polyline.size() == 0) continue;
			
			float totalLength = polyline.getPerimeter();
			int numSamples = static_cast<int>(totalLength / ds);
			numSamples = ofClamp(numSamples, 3, 100000);
			
			for (int i = 0; i < numSamples; i++) {
				float position = static_cast<float>(i) / static_cast<float>(numSamples - 1);
				vec2 point = polyline.getPointAtPercent(position);
				points.push_back(point);
			}
		}
	}
	
	return points;
}

//--------------------------------------------------------------
void OrganicText::refreshPointsString() {
	ofLogNotice("OrganicText") << "refreshPointsString()";
	
	// Map spacing (0-1 normalized)
	float finalSpacing = ofMap(densitySpacing.get(), 0, 1, DENSITY_SPACING_MIN, DENSITY_SPACING_MAX, true);
	
	pointsString = sampleStringPoints(sText, finalSpacing);
	
	// Initialize trails
	initTrails();
	
	// Calculate center and width
	if (pointsString.size() > 0) {
		vec2 sum(0, 0);
		for (const auto & p : pointsString) {
			sum += p;
		}
		textCenter = sum / static_cast<float>(pointsString.size());
	}
	
	// Store text width and height for mouse coordinate transformations
	textWidth = font.stringWidth(sText);
	textHeight = font.stringHeight(sText);
}

//--------------------------------------------------------------
vec2 OrganicText::getAnimatedOffset(int index, float phase) const {
	vec2 offset(0, 0);
	
	if (!bEnableAnimation.get() || pointsString.empty()) {
		return offset;
	}
	
	float fontScale = fontSize.get() / 150.0f;
	
	// Calculate custom animation origin based on animOriginX (0=left, 0.5=center, 1=right)
	float customOriginX = textWidth * animOriginX.get();
	float customOriginY = textCenter.y;
	
	// Override with mouse position if mouse control is active
	// Now works across entire canvas, deforming the constellation
	// exclude ANIM_WAVE because no good results seen with it
	if (bMouseTweaks.get() && bMouseControlOrigin.get()&&(AnimMode)animationMode.get()!=ANIM_WAVE) {
		customOriginX = mouseLocalPos.x;
		customOriginY = mouseLocalPos.y;
	}
	
	vec2 customOrigin = vec2(customOriginX, customOriginY);
	
	switch ((AnimMode)animationMode.get()) {
		case ANIM_NOISE: {
			float maxDisp = ofMap(animPower.get(), 0, 1, 0, ANIM_NOISE_MAX * fontScale, true);
			
			// If mouse control is active, reduce noise displacement near mouse position
			if (bMouseTweaks.get() && bMouseControlOrigin.get()) {
				float mouseInfluence = getMouseInfluence(pointsString[index]);
				maxDisp *= (1.0f - mouseInfluence * 2.f); // Reduce up to % near mouse
			}
			
			offset = vec2(ofSignedNoise(phase, 0.0f), ofSignedNoise(phase, 233.0f)) * maxDisp;
			break;
		}
			
		case ANIM_WAVE: {
			float freq = ofMap(animWaveFreq.get(), 0, 1, ANIM_WAVE_FREQ_MIN, ANIM_WAVE_FREQ_MAX, true);
			float amp = ofMap(animIntensity.get(), 0, 1, 0, ANIM_WAVE_MAX * fontScale, true);
			
			// Use distance from custom origin instead of absolute x position
			float distFromOrigin = pointsString[index].x - customOriginX;
			float wave = sin(distFromOrigin * freq + t * TWO_PI) * amp;
			offset = vec2(0, wave);
			break;
		}
			
		case ANIM_SPIRAL: {
			if (pointsString.size() < 2) break;
			
			// Use custom origin for spiral center
			float angle = atan2(pointsString[index].y - customOrigin.y, pointsString[index].x - customOrigin.x);
			float distance = glm::distance(pointsString[index], customOrigin);
			float tightness = ofMap(animSpiral.get(), 0, 1, ANIM_SPIRAL_TIGHT_MIN, ANIM_SPIRAL_TIGHT_MAX, true);
			float maxDisp = ofMap(animPower.get(), 0, 1, 0, ANIM_SPIRAL_MAX * fontScale, true);
			
			float spiralPhase = distance * tightness + t * TWO_PI;
			float spiralOffset = sin(spiralPhase) * maxDisp;
			
			offset = vec2(cos(angle) * spiralOffset, sin(angle) * spiralOffset);
			break;
		}
			
		case ANIM_PULSE: {
			if (pointsString.size() < 2) break;
			
			// Use custom origin for pulse center
			float distance = glm::distance(pointsString[index], customOrigin);
			float maxPulse = ofMap(animPulseIntensity.get(), 0, 1, 0, ANIM_PULSE_MAX * fontScale, true);
			float pulsePhase = sin(t * TWO_PI * 0.5f) * maxPulse;
			
			if (distance > 0.5f) {
				vec2 direction = normalize(pointsString[index] - customOrigin);
				offset = direction * pulsePhase;
			}
			break;
		}
			
		case ANIM_ORBIT: {
			if (pointsString.size() < 2) break;
			
			// Use custom origin as pivot point for orbit
			float angle = atan2(pointsString[index].y - customOrigin.y, pointsString[index].x - customOrigin.x);
			angle += t * TWO_PI * 0.3f;
			
			float distance = glm::distance(pointsString[index], customOrigin);
			vec2 newPos = customOrigin + vec2(cos(angle), sin(angle)) * distance;
			offset = newPos - pointsString[index];
			break;
		}
	}
	
	return offset;
}

//--------------------------------------------------------------
float OrganicText::getMouseInfluence(vec2 position) const {
	float radiusPixels = ofMap(radiusMouse.get(), 0.f, 1.f, MOUSE_RADIUS_INTERACT_MIN, MOUSE_RADIUS_INTERACT_MAX, true);
	float distToMouse = glm::distance(position, mouseLocalPos);
	
	if (distToMouse >= radiusPixels) {
		return 0.0f; // Outside radius
	}
	
	// Calculate normalized influence (1.0 at center, 0.0 at edge)
	float influence = ofMap(distToMouse, 0, radiusPixels, 1.0f, 0.0f, true);
	
	// Apply mouseInfluenceStrength BEFORE power curve to maintain control range
	influence *= mouseInfluenceStrength.get();
	
	// Apply power curve for gradient shape
	// Power of 0.7 creates a softer, wider gradient
	float power = 0.7f;
	influence = std::pow(influence, power);
	
	return influence;
}

//--------------------------------------------------------------
ofColor OrganicText::getPointColor(int index, vec2 position, float phase) const {
	ofColor color = ofColor(ofColor::white, 255);
	
	if (pointsString.empty()) return color;
	
	switch ((ColorMode)colorMode.get()) {
		case COLOR_GLOBAL_1:
			color = color1.get();
			break;
			
		case COLOR_GLOBAL_2:
			color = color2.get();
			break;
			
		case COLOR_GLOBAL_3:
			color = color3.get();
			break;
			
		case COLOR_GLOBAL_MIX: {
			float indexFactor = static_cast<float>(index) / ofClamp(static_cast<float>(pointsString.size()), 1.0f, 100000.0f);
			float timeFactor = (sin(t * colorSpeed.get() * TWO_PI) + 1.0f) * 0.5f;
			
			ofColor c1 = color1.get();
			ofColor c2 = color2.get();
			ofColor c3 = color3.get();
			
			// Use dynamic transition points
			float innerPoint = colorInner.get();
			if (indexFactor < innerPoint) {
				color = c1.lerp(c2, indexFactor / innerPoint);
			} else {
				color = c2.lerp(c3, (indexFactor - innerPoint) / (1.0f - innerPoint));
			}
			
			color = color.lerp(c3, timeFactor * colorMixFactor.get() * 0.4f);
			
			// Apply alpha fade based on index position (left to right)
			if (colorAlphaRange.get() > 0.01f) {
				float alphaMin = ofMap(colorAlphaRange.get(), 0, 1, 255, 50, true);
				float alpha = ofMap(indexFactor, 0, 1, 255, alphaMin, true);
				color.a = alpha;
			}
			break;
		}
			
		case COLOR_DISTANCE: {
			float distance = glm::distance(position, textCenter);
			float distFactor = ofMap(distance, 0, COLOR_DISTANCE_MAX, 0, 1, true);
			
			ofColor c1 = color1.get();
			ofColor c2 = color2.get();
			ofColor c3 = color3.get();
			
			// Use dynamic transition points
			float innerPoint = colorInner.get();
			float middlePoint = colorDistMiddle.get();
			
			if (distFactor < innerPoint) {
				color = c1.lerp(c2, distFactor / innerPoint);
			} else if (distFactor < middlePoint) {
				float localT = (distFactor - innerPoint) / (middlePoint - innerPoint);
				color = c2.lerp(c3, localT);
			} else {
				color = c3;
			}
			
			// Apply alpha fade based on distance from center
			if (colorAlphaRange.get() > 0.01f) {
				float alphaCenter = 255.0f;
				float alphaEdge = ofMap(colorAlphaRange.get(), 0, 1, 255, 50, true);
				float alpha = ofMap(distance, 0, COLOR_DISTANCE_MAX, alphaCenter, alphaEdge, true);
				color.a = alpha;
			}
			break;
		}
	}
	
	if (bColorByDistance) {
		float distance = glm::distance(position, textCenter);
		// Map alpha range dynamically: 255 (center) to edge based on colorAlphaRange
		float alphaEdge = ofMap(colorAlphaRange.get(), 0, 1, 255, 0, true);
		float alpha = ofMap(distance, 0, COLOR_DISTANCE_MAX, 255, alphaEdge, true);
		color.a = alpha;
	}
	
	// Mouse highlight: override color for points within mouse radius
	if (bMouseTweaks.get() && bMouseHighlightPoints.get()) {
		float influence = getMouseInfluence(position);
		if (influence > 0.0f) {
			color = color.lerp(colorMouseHighlight.get(), influence);
		}
	}
	
	return color;
}

//--------------------------------------------------------------
void OrganicText::drawShape(vec2 position, float size, ShapeType shape, float rotation) const {
	ofPushMatrix();
	ofTranslate(position);
	ofRotateDeg(rotation);
	float h = size * ofMap(shapeRatio.get(), 0.f, 1.f, 1.f, 10.f, true);
	
	switch (shape) {
		case SHAPE_CIRCLE:
			ofDrawCircle(0, 0, size);
			break;
			
		case SHAPE_RECTANGLE:
			ofDrawRectangle(-size * 0.5f, -size * 0.5f, size, h);
			break;
			
		case SHAPE_TRIANGLE: {
			ofDrawTriangle(0, -h * 0.5f, -size * 0.5f, h * 0.5f, size * 0.5f, h * 0.5f);
			break;
		}
			
		case SHAPE_STAR: {
			ofPolyline star;
			int points = 5;
			for (int i = 0; i < points * 2; i++) {
				float angle = (static_cast<float>(i) * TWO_PI) / (static_cast<float>(points) * 2.0f);
				float radius = (i % 2 == 0) ? size : size * 0.4f;
				star.addVertex(cos(angle) * radius, sin(angle) * radius);
			}
			star.close();
			
			// Convert polyline to path for fill support
			ofPath starPath;
			starPath.setFilled(bDrawFill.get()); // enable or disable fill
			starPath.setFillColor(ofGetStyle().color); // use current drawing color
			starPath.setStrokeColor(ofGetStyle().color); // outline uses same color
			starPath.setStrokeWidth(1.0f);
			
			starPath.moveTo(star[0]);
			for (std::size_t i = 1; i < star.size(); ++i) {
				starPath.lineTo(star[i]);
			}
			starPath.close();
			
			starPath.draw(); // draw filled or not depending on flag
			break;
		}
			
		case SHAPE_CROSS: {
			float thickness = size * 0.2f;
			ofDrawRectangle(-thickness * 0.5f, -size * 0.5f, thickness, size);
			ofDrawRectangle(-size * 0.5f, -thickness * 0.5f, size, thickness);
			break;
		}
			
		case SHAPE_POINT: {
			ofDrawCircle(0, 0, size * 0.05f);
			break;
		}
	}
	
	ofPopMatrix();
}

//--------------------------------------------------------------
void OrganicText::initTrails() {
	ofLogNotice("OrganicText") << "initTrails()";
	
	pointTrails.clear();
	pointTrails.resize(pointsString.size());
	
	int tl = (int)ofMap(trailLength.get(), 0.f, 1.f, TRAILS_LENGTH_MIN, TRAILS_LENGTH_MAX, true);
	for (size_t i = 0; i < pointsString.size(); i++) {
		pointTrails[i].resize(tl);
		for (int j = 0; j < tl; j++) {
			pointTrails[i][j] = pointsString[i];
		}
	}
}

//--------------------------------------------------------------
void OrganicText::updateTrails() {
	for (size_t i = 0; i < pointsString.size() && i < pointTrails.size(); i++) {
		// Use cached animated position if available, otherwise calculate
		vec2 currentPos;
		if (i < pointsAnimatedCache.size() && pointsAnimatedCache.size() == pointsString.size()) {
			currentPos = pointsAnimatedCache[i];
		} else {
			float phase = t + 0.123f * static_cast<float>(i);
			vec2 offset = getAnimatedOffset(static_cast<int>(i), phase);
			currentPos = pointsString[i] + offset;
		}
		
		for (int j = static_cast<int>(pointTrails[i].size()) - 1; j > 0; j--) {
			pointTrails[i][j] = pointTrails[i][j - 1];
		}
		pointTrails[i][0] = currentPos;
	}
}

//--------------------------------------------------------------
void OrganicText::drawConnections() const {
	if (!bDrawConnections.get() || pointsString.size() < 2) return;
	
	float maxDist = connectDistance.get();
	float quality = ofClamp(connectQuality.get(), 0.1f, 1.0f);
	int skipFactor = ofClamp(static_cast<int>(1.0f / quality), 1, 100);
	
	int maxConPerPoint = bConnectNearOnly ? CONNECTIONS_MAX_PER_POINT_NEAR : CONNECTIONS_MAX_PER_POINT_FAR;
	
	// Reset cached count
	cachedConnectionCount = 0;
	ofPushStyle();
	ofSetLineWidth(connectLineWidth);
	
	// Use ofMesh for maximum performance - batch all connection lines into single draw call
	ofMesh connectionMesh;
	connectionMesh.setMode(OF_PRIMITIVE_LINES);
	
	// Collect all connection segments into a single mesh
	for (size_t i = 0; i < pointsString.size(); i += skipFactor) {
		// Use cached animated position if available, otherwise calculate
		vec2 pos1;
		if (i < pointsAnimatedCache.size() && pointsAnimatedCache.size() == pointsString.size()) {
			pos1 = pointsAnimatedCache[i];
		} else {
			float phase1 = t + 0.123f * static_cast<float>(i);
			vec2 offset1 = getAnimatedOffset(static_cast<int>(i), phase1);
			pos1 = pointsString[i] + offset1;
		}
		
		int connectionsDrawn = 0;
		int searchLimit = bConnectNearOnly ? ofClamp(CONNECTIONS_SEARCH_NEAR, 1, static_cast<int>(pointsString.size()) - static_cast<int>(i)) : ofClamp(CONNECTIONS_SEARCH_FAR, 1, static_cast<int>(pointsString.size()) - static_cast<int>(i));
		
		for (int offset = 1; offset < searchLimit && connectionsDrawn < maxConPerPoint; offset += skipFactor) {
			size_t j = i + offset;
			if (j >= pointsString.size()) break;
			
			// Use cached animated position if available, otherwise calculate
			vec2 pos2;
			if (j < pointsAnimatedCache.size() && pointsAnimatedCache.size() == pointsString.size()) {
				pos2 = pointsAnimatedCache[j];
			} else {
				float phase2 = t + 0.123f * static_cast<float>(j);
				vec2 offset2 = getAnimatedOffset(static_cast<int>(j), phase2);
				pos2 = pointsString[j] + offset2;
			}
			
			float dist = glm::distance(pos1, pos2);
			
			if (dist < maxDist) {
				float alpha = ofMap(dist, 0, maxDist, 255, 0, true);
				float acolor = ofMap(colorConnection.get().a, 0, 255, 0.f, 1.f, true);
				float o = 2.f; //power
				alpha = ofMap(alpha * (acolor * o * connectAlpha.get()), 0, 255, 0, 255, true);
				
				ofColor connectionColor = colorConnection.get();
				connectionColor.a = alpha;
				
				// Add line segment vertices
				glm::vec3 p1(pos1.x, pos1.y, 0.0f);
				glm::vec3 p2(pos2.x, pos2.y, 0.0f);
				
				connectionMesh.addVertex(p1);
				connectionMesh.addColor(connectionColor);
				connectionMesh.addVertex(p2);
				connectionMesh.addColor(connectionColor);
				
				connectionsDrawn++;
				cachedConnectionCount++;
			}
		}
	}
	
	// Single draw call for all connections - MUCH faster than individual lines
	if (connectionMesh.getNumVertices() > 0) {
		connectionMesh.draw();
	}
	
	ofPopStyle();
}

//--------------------------------------------------------------
void OrganicText::drawTrails() {
	
	ofPushStyle();
	ofSetLineWidth(trailLineWidth);
	float tf = ofMap(trailFade, 0.f, 1.f, TRAILS_FADE_MIN, TRAILS_FADE_MAX, true);
	
	// Use ofMesh for maximum performance - batch all trail lines into single draw call
	ofMesh trailMesh;
	trailMesh.setMode(OF_PRIMITIVE_LINES);
	
	// Collect all trail segments into a single mesh
	for (size_t i = 0; i < pointTrails.size(); i++) {
		if (pointTrails[i].size() < 2) continue; // Skip if not enough points
		
		// Add line segments for this trail
		for (size_t j = 1; j < pointTrails[i].size(); j++) {
			float fadeAmount = pow(tf, static_cast<float>(j));
			float alpha = fadeAmount * TRAILS_ALPHA_MAX;
			
			ofColor segmentColor = colorTrails.get();
			segmentColor.a = alpha;
			
			// Add line segment vertices
			glm::vec3 p1(pointTrails[i][j - 1].x, pointTrails[i][j - 1].y, 0.0f);
			glm::vec3 p2(pointTrails[i][j].x, pointTrails[i][j].y, 0.0f);
			
			trailMesh.addVertex(p1);
			trailMesh.addColor(segmentColor);
			trailMesh.addVertex(p2);
			trailMesh.addColor(segmentColor);
		}
	}
	
	// Single draw call for all trails - MUCH faster than individual lines
	if (trailMesh.getNumVertices() > 0) {
		trailMesh.draw();
	}
	
	ofPopStyle();
}

//--------------------------------------------------------------
void OrganicText::drawShapes() {
	// Resize cache if needed
	if (pointsAnimatedCache.size() != pointsString.size()) {
		pointsAnimatedCache.resize(pointsString.size());
	}
	
	for (size_t i = 0; i < pointsString.size(); i++) {
		ofPushStyle();
		
		float phase = t + 0.123f * static_cast<float>(i);
		vec2 offset = getAnimatedOffset(static_cast<int>(i), phase);
		vec2 finalPos = pointsString[i] + offset;
		
		// Calculate mouse influence for this point
		float mouseInfluence = getMouseInfluence(finalPos);
		
		// Apply mouse displacement effect (bidirectional)
		// < 0.5 = attract, 0.5 = neutral, > 0.5 = repel
		if (bMouseTweaks.get() && bMouseDisplacePoints.get() && mouseInfluence > 0.0f) {
			// Calculate direction from mouse to point
			vec2 direction = glm::normalize(finalPos - mouseLocalPos);
			
			// Map mouseDisplacePower: 0.5 = no effect, < 0.5 = attract, > 0.5 = repel
			float powerCentered = (mouseDisplacePower.get() - 0.5f) * 2.0f; // Maps [0,1] to [-1,1]
			
			float maxDisplacement = 50.0f; // Maximum displacement in pixels
			float displacement = mouseInfluence * powerCentered * maxDisplacement;
			
			// If powerCentered is negative (attract), direction is inverted
			finalPos += direction * displacement;
		}
		
		// Cache the final animated position for reuse in connections and trails
		pointsAnimatedCache[i] = finalPos;
		
		ofColor color = getPointColor(static_cast<int>(i), finalPos, phase);
		
		ofSetColor(color);
		
		if (bDrawFill.get())
			ofFill();
		else
			ofNoFill();
		
		float maxSize = ofMap(shapeSize.get(), 0, 1, SHAPE_MIN_RADIUS, SHAPE_MAX_RADIUS, true);
		float minSize = ofMap(shapeSizeMin.get(), 0, 1, 0, maxSize, true);
		
		float sizeNoise = ofNoise(phase * SHAPE_SIZE_NOISE_SCALE, static_cast<float>(i) * SHAPE_SIZE_INDEX_SCALE);
		float pointSize = ofLerp(minSize, maxSize, sizeNoise);
		
		// Apply mouse scale effect (bidirectional)
		// < 0.5 = shrink, 0.5 = neutral, > 0.5 = grow
		if (bMouseTweaks.get() && bMouseScaleShapes.get() && mouseInfluence > 0.0f) {
			// Map mouseScalePower: 0.5 = no effect, < 0.5 = shrink, > 0.5 = grow
			float powerCentered = (mouseScalePower.get() - 0.5f) * 2.0f; // Maps [0,1] to [-1,1]
			
			// Calculate scale multiplier
			// Positive: grows up to MAX_SCALE_POWER times
			// Negative: shrinks down to near 0
			float scaleMultiplier = 1.0f + (mouseInfluence * powerCentered * MAX_SCALE_POWER);
			pointSize *= scaleMultiplier;
		}
		
		float rotation = ofMap(shapeRotation.get(), 0, 1, 0, 360, true);
		
		drawShape(finalPos, pointSize, (ShapeType)shapeType.get(), rotation);
		
		ofPopStyle();
	}
}

//--------------------------------------------------------------
void OrganicText::draw() {
	float zoomFactor = 1.0f + (zoomGlobal.get() * ZOOM_GLOBAL_MAX);
	
	ofPushMatrix();
	{
		float centerX = ofGetWidth() * 0.5f;
		float centerY = ofGetHeight() * 0.5f;
		ofTranslate(centerX, centerY);
		ofScale(zoomFactor, zoomFactor);
		
		ofTranslate(-font.stringWidth(sText) * 0.5f, font.stringHeight(sText) * 0.5f);
		
		//--
		
		// Layer 0: Shapes
		if (bDrawShapes && bShapeBack) {
			drawShapes();
		}
		
		// Layer 1: Connections
		if (bDrawConnections) {
			drawConnections();
		}
		
		// Layer 2: Trails
		if (bDrawTrails) {
			updateTrails();
			
			drawTrails();
		}
		
		// Layer 3: Shapes
		if (bDrawShapes && !bShapeBack) {
			drawShapes();
		}
	}
	ofPopMatrix();
}
