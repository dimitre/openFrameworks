#include "organicTextPreset.h"
#include "organicText.h"

//--------------------------------------------------------------
void organicTextPreset::applyPreset(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyPreset()";
	
	applyDrawFlags(ot);
	applyAnimationParams(ot);
	applyColorModesParams(ot);
	applyColorParams(ot);
	applyConnectionsParams(ot);
	applyDensityParams(ot);
	applyShapeParams(ot);
	applyMouseTweaksParams(ot);
	applyTrailsParams(ot);
}

//--------------------------------------------------------------
void organicTextPreset::applyDrawFlags(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyDrawFlags()";
	
	ot->bEnableAnimation.set(true);
	ot->bDrawConnections.set(true);
	ot->bDrawFill.set(true);
	ot->bDrawShapes.set(true);
	ot->bDrawTrails.set(true);
}

//--------------------------------------------------------------
void organicTextPreset::applyAnimationParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyAnimationParams()";
	
	ot->bEnableAnimation.set(true);
	ot->animIntensity.set(0.897959f);
	ot->animationMode.set(3);
	ot->animOriginX.set(0.72449f);
	ot->animPower.set(1.0f);
	ot->animPulseIntensity.set(1.0f);
	ot->animSpeed.set(0.510204f);
	ot->animSpiral.set(0.239944f);
	ot->animWaveFreq.set(0.586735f);
}

//--------------------------------------------------------------
void organicTextPreset::applyColorModesParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyColorModesParams()";
	
	ot->colorAlphaRange.set(0.5f);
	ot->bColorByDistance.set(true);
	ot->colorDistMiddle.set(0.729592f);
	ot->colorInner.set(0.306122f);
	ot->colorMixFactor.set(1.0f);
	ot->colorMode.set(3);
	ot->colorSpeed.set(2.5f);
}

//--------------------------------------------------------------
void organicTextPreset::applyColorParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyColorParams()";
	
	ot->color1.set(ofColor(0, 1, 255, 255));
	ot->color2.set(ofColor(7, 71, 255, 255));
	ot->color3.set(ofColor(14, 0, 255, 162));
	ot->colorConnection.set(ofColor(255, 255, 255, 255));
	ot->colorTrails.set(ofColor(255, 0, 111, 255));
}

//--------------------------------------------------------------
void organicTextPreset::applyConnectionsParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyConnectionsParams()";
	
	ot->connectAlpha.set(0.688776f);
	ot->connectDistance.set(68.4949f);
	ot->connectLineWidth.set(1.0f);
	ot->bConnectNearOnly.set(true);
	ot->connectQuality.set(0.494898f);
}

//--------------------------------------------------------------
void organicTextPreset::applyDensityParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyDensityParams()";
	
	ot->densitySpacing.set(0.195408f);
}

//--------------------------------------------------------------
void organicTextPreset::applyShapeParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyShapeParams()";
	
	ot->shapeSizeMin.set(0.102041f);
	ot->shapeRatio.set(0.266672f);
	ot->shapeRotation.set(0.54717f);
	ot->shapeSize.set(0.295937f);
	ot->shapeType.set(0);
	ot->bShapeBack.set(false);
}

//--------------------------------------------------------------
void organicTextPreset::applyMouseTweaksParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyMouseTweaksParams()";
	
	ot->colorMouseHighlight.set(ofColor(255, 0, 104, 255));
	ot->mouseDisplacePower.set(0.352041f);
	ot->bMouseDisplacePoints.set(true);
	ot->bMouseHighlightPoints.set(true);
	ot->mouseInfluenceStrength.set(0.663265f);
	ot->mouseScalePower.set(0.596939f);
	ot->bMouseScaleShapes.set(true);
	ot->radiusMouse.set(0.755102f);
	ot->bMouseControlOrigin.set(true);
}

//--------------------------------------------------------------
void organicTextPreset::applyTrailsParams(OrganicText * ot) {
	ofLogNotice("organicTextPreset") << "applyTrailsParams()";
	
	ot->trailFade.set(0.397959f);
	ot->trailLength.set(0.69898f);
	ot->trailLineWidth.set(0.1f);
}
