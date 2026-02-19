#pragma once

#include "ofGraphicsBaseTypes.h"
#include "ofPolyline.h"
#include "of3dGraphics.h"
#include "ofBitmapFont.h"
#include "ofMatrixStack.h"
#include "ofPath.h"
#include "ofGLBaseTypes.h"

class ofShapeTessellation;
class ofFbo;
class of3dPrimitive;

class ofGLRenderer: public ofBaseGLRenderer{
public:
	ofGLRenderer(const ofAppBaseWindow * window);
	~ofGLRenderer() override {}

	static const std::string TYPE;
	const std::string & getType() override { return TYPE; }

    void setup();

	void startRender() override;
	void finishRender() override;

	using ofBaseRenderer::draw;
	using ofBaseGLRenderer::draw;
	void draw(const ofMesh & vertexData, ofPolyRenderMode renderType, bool useColors, bool useTextures, bool useNormals) const override;
    void draw(const of3dPrimitive& model, ofPolyRenderMode renderType) const override;
    void draw(const ofNode& model) const override;
	void draw(const ofPolyline & poly) const override;
	void draw(const ofPath & path) const override;
	void draw(const ofImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;
	void draw(const ofFloatImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;
	void draw(const ofShortImage & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;
	void draw(const ofTexture & image, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;
	void draw(const ofBaseVideoDraws & video, float x, float y, float w, float h) const override;
	void draw(const ofVbo & vbo, GLuint drawMode, int first, int total) const override;
	void drawElements(const ofVbo & vbo, GLuint drawMode, int amt, int offsetelements = 0) const override;
	void drawInstanced(const ofVbo & vbo, GLuint drawMode, int first, int total, int primCount) const override;
	void drawElementsInstanced(const ofVbo & vbo, GLuint drawMode, int amt, int primCount) const override;
	void draw(const ofVboMesh & mesh, ofPolyRenderMode renderType) const override;
	void drawInstanced(const ofVboMesh & mesh, ofPolyRenderMode renderType, int primCount) const override;
	ofPath & getPath() override;



	//--------------------------------------------
	// transformations
	void pushView() override;
	void popView() override;

	// setup matrices and viewport (upto you to push and pop view before and after)
	// if width or height are 0, assume windows dimensions (ofGetWidth(), ofGetHeight())
	// if nearDist or farDist are 0 assume defaults (calculated based on width / height)
	void viewport(ofRectangle viewport) override;
	void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) override;
	void setOrientation(ofOrientation orientation, bool vFlip) override;
	void setupScreenPerspective(float width = -1, float height = -1, float fov = 60, float nearDist = 0, float farDist = 0) override;
	void setupScreenOrtho(float width = -1, float height = -1, float nearDist = -1, float farDist = 1) override;
	ofRectangle getCurrentViewport() const override;
	ofRectangle getNativeViewport() const override;
	int getViewportWidth() const override;
	int getViewportHeight() const override;
	bool isVFlipped() const override;
	bool texturesNeedVFlip() const;

	void setCoordHandedness(ofHandednessType handedness) override;
	ofHandednessType getCoordHandedness() const override;

	//our openGL wrappers
	void pushMatrix() override;
	void popMatrix() override;
	void translate(float x, float y, float z = 0) override;
	void translate(const glm::vec3 & p) override;
	void scale(float xAmnt, float yAmnt, float zAmnt = 1) override;
	void rotateDeg(float radians, float vecX, float vecY, float vecZ) override;
	void rotateXDeg(float radians) override;
	void rotateYDeg(float radians) override;
	void rotateZDeg(float radians) override;
	void rotateDeg(float radians) override;
	void rotateRad(float radians, float vecX, float vecY, float vecZ) override;
	void rotateXRad(float radians) override;
	void rotateYRad(float radians) override;
	void rotateZRad(float radians) override;
	void rotateRad(float radians) override;
	void matrixMode(ofMatrixMode mode) override;
	void loadIdentityMatrix (void) override;
	void loadMatrix (const glm::mat4 & m) override;
	void loadMatrix (const float * m) override;
	void multMatrix (const glm::mat4 & m) override;
	void multMatrix (const float * m) override;
	void loadViewMatrix(const glm::mat4 & m) override;
	void multViewMatrix(const glm::mat4 & m) override;

	glm::mat4 getCurrentMatrix(ofMatrixMode matrixMode_) const override;
	glm::mat4 getCurrentOrientationMatrix() const override;
	glm::mat4 getCurrentViewMatrix() const override;
	glm::mat4 getCurrentNormalMatrix() const override;
	
	// screen coordinate things / default gl values
	void setupGraphicDefaults() override;
	void setupScreen() override;

	// drawing modes
	void setFillMode(ofFillFlag fill) override;
	ofFillFlag getFillMode() override;
	void setCircleResolution(int res) override;
	void setRectMode(ofRectMode mode) override;
	ofRectMode getRectMode() override;
	void setLineWidth(float lineWidth) override;
	void setPointSize(float pointSize) override;
	void setDepthTest(bool depthTest) override;
	void setLineSmoothing(bool smooth) override;
	void setBlendMode(ofBlendMode blendMode) override;
	void enablePointSprites() override;
	void disablePointSprites() override;
	void enableAntiAliasing() override;
	void disableAntiAliasing() override;

	// color options
	void setColor(float r, float g, float b) override; // 0-1
	void setColor(float r, float g, float b, float a) override; // 0-1
	void setColor(const ofFloatColor & color) override;
	void setColor(const ofFloatColor & color, float _a) override;
	void setColor(float gray) override; // new set a color as grayscale with one argument
	void setHexColor( int hexColor ) override; // hex, like web 0xFF0033;

	void setBitmapTextMode(ofDrawBitmapMode mode) override;

	// bg color
	ofFloatColor getBackgroundColor() override;
	void setBackgroundColor(const ofFloatColor & c) override;
	void background(const ofFloatColor & c) override;
	void background(float brightness) override;
	void background(int hexColor, int _a=255) override;
	void background(float r, float g, float b, float a=1.f) override;

	void setBackgroundAuto(bool bManual) override;		// default is true
	bool getBackgroundAuto() override;

	void clear() override;
	void clear(float r, float g, float b, float a=0) override;
	void clear(float brightness, float a=0) override;
	void clearAlpha() override;

	ofStyle getStyle() const override;
	void pushStyle() override;
	void popStyle() override;
	void setStyle(const ofStyle & style) override;
	void setCurveResolution(int resolution) override;
	void setPolyMode(ofPolyWindingMode mode) override;


	// drawing
	void drawLine(float x1, float y1, float z1, float x2, float y2, float z2) const override;
	void drawRectangle(float x, float y, float z, float w, float h) const override;
	void drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) const override;
	void drawCircle(float x, float y, float z, float radius) const override;
	void drawEllipse(float x, float y, float z, float width, float height) const override;
	void drawString(std::string text, float x, float y, float z) const override;
	void drawString(const ofTrueTypeFont & font, std::string text, float x, float y) const override;


	// gl specifics
	void enableTextureTarget(const ofTexture & tex, int textureLocation) override;
	void disableTextureTarget(int textureTarget, int textureLocation) override;
	void setAlphaMaskTex(const ofTexture & tex) override;
	void disableAlphaMask() override;

	// lighting globals
	void enableLighting() override;
	void disableLighting() override;
	void enableSeparateSpecularLight() override;
	void disableSeparateSpecularLight() override;
	bool getLightingEnabled() override;
	void setSmoothLighting(bool b) override;
	void setGlobalAmbientColor(const ofFloatColor& c) override;

	// lighting per light
	void enableLight(int lightIndex) override;
	void disableLight(int lightIndex) override;
	void setLightSpotlightCutOff(int lightIndex, float spotCutOff) override;
	void setLightSpotConcentration(int lightIndex, float exponent) override;
	void setLightAttenuation(int lightIndex, float constant, float linear, float quadratic ) override;
	void setLightAmbientColor(int lightIndex, const ofFloatColor& c) override;
	void setLightDiffuseColor(int lightIndex, const ofFloatColor& c) override;
	void setLightSpecularColor(int lightIndex, const ofFloatColor& c) override;
	void setLightPosition(int lightIndex, const glm::vec4 & position) override;
	void setLightSpotDirection(int lightIndex, const glm::vec4 & direction) override;


	void bind(const ofBaseVideoDraws & video) override;
	void bind(const ofBaseMaterial & material) override;
	void bind(const ofShadow & shadow) override; // does nothing, only programmable renderer supported
	void bind(const ofShadow & shadow, GLenum aCubeFace) override;
	void bind(const ofShader & shader) override;
	void bind(const ofTexture & texture, int location) override;
	void bind(const ofCamera & camera, const ofRectangle & viewport) override;
	void unbind(const ofBaseVideoDraws & video) override;
	void unbind(const ofBaseMaterial & material) override;
	void unbind(const ofShadow & shadow) override; // does nothing, only programmable renderer supported
	void unbind(const ofShadow & shadow, GLenum aCubeFace) override;
	void unbind(const ofShader & shader) override;
	void unbind(const ofTexture & texture, int location) override;
	void unbind(const ofCamera & camera) override;

    void begin(const ofFbo & fbo, ofFboMode mode) override;
	void end(const ofFbo & fbo) override;

	void bind(const ofFbo & fbo) override;
#ifndef TARGET_OPENGLES
	void bindForBlitting(const ofFbo & fboSrc, ofFbo & fboDst, int attachmentPoint) override;
#endif
	void unbind(const ofFbo & fbo) override;

	int getGLVersionMajor() override;
	int getGLVersionMinor() override;

	void saveScreen(int x, int y, int w, int h, ofPixels & pixels) override;
	void saveFullViewport(ofPixels & pixels) override;

	const of3dGraphics & get3dGraphics() const override;
	of3dGraphics & get3dGraphics() override;
private:
	void startSmoothing();
	void endSmoothing();


	bool bBackgroundAuto;

	mutable std::vector<glm::vec3> linePoints;
	mutable std::vector<glm::vec3> rectPoints;
	mutable std::vector<glm::vec3> triPoints;
	mutable std::vector<glm::vec3> circlePoints;
	ofPolyline circlePolyline;

	ofMatrixStack matrixStack;
	bool normalsEnabled;
	bool lightingEnabled;
        bool materialBound;
	std::set<int> textureLocationsEnabled;

	int alphaMaskTextureTarget;

	ofStyle currentStyle;
	std::deque <ofStyle> styleHistory;
	of3dGraphics graphics3d;
	ofBitmapFont bitmapFont;
	ofPath path;
	const ofAppBaseWindow * window;

	std::deque<GLuint> framebufferIdStack;	///< keeps track of currently bound framebuffers
	GLuint defaultFramebufferId;		///< default GL_FRAMEBUFFER_BINDING, windowing frameworks might want to set this to their MSAA framebuffer, defaults to 0
	GLuint currentFramebufferId;		///< the framebuffer id currently bound to the GL_FRAMEBUFFER target

};
