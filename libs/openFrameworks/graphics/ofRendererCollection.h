#pragma once

#include "ofGraphicsBaseTypes.h"
#include "of3dGraphics.h"
#include "ofPath.h" // MARK: ofPath path;

class ofRendererCollection final: public ofBaseRenderer{
public:
	 ofRendererCollection():graphics3d(this){}
	 ~ofRendererCollection(){}

	 static const std::string TYPE;
	 const std::string & getType() override { return TYPE; }

	 std::shared_ptr<ofBaseGLRenderer> getGLRenderer();

	 bool rendersPathPrimitives();

	 void startRender() override;

	 void finishRender() override;


	 using ofBaseRenderer::draw;

	 void draw(const ofPolyline & poly) const override;
	 void draw(const ofPath & shape) const override;

	 void draw(const ofMesh & vertexData, ofPolyRenderMode mode, bool useColors, bool useTextures, bool useNormals) const override;

	void draw(const  of3dPrimitive& model, ofPolyRenderMode renderType ) const override;

	void draw(const  ofNode& node) const override;

	void draw(const ofImage & img, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;

	void draw(const ofFloatImage & img, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;

	void draw(const ofShortImage & img, float x, float y, float z, float w, float h, float sx, float sy, float sw, float sh) const override;

	void draw(const ofBaseVideoDraws & video, float x, float y, float w, float h) const override;

	/*void bind(const ofBaseVideoDraws & video) const{
		for(int i=0;i<(int)renderers.size();i++){
			renderers[i]->bind(video);
		}
	}

	void unbind(const ofBaseVideoDraws & video) const{
		for(int i=0;i<(int)renderers.size();i++){
			renderers[i]->unbind(video);
		}
	}*/


	glm::mat4 getCurrentMatrix(ofMatrixMode matrixMode_) const override;


	glm::mat4 getCurrentOrientationMatrix() const override;

	glm::mat4 getCurrentNormalMatrix() const override;

	//--------------------------------------------
	// transformations
	 void pushView() override;

	 void popView() override;
	// setup matrices and viewport (upto you to push and pop view before and after)
	// if width or height are 0, assume windows dimensions (ofGetWidth(), ofGetHeight())
	// if nearDist or farDist are 0 assume defaults (calculated based on width / height)
	void viewport(ofRectangle viewport) override;

	 void viewport(float x = 0, float y = 0, float width = -1, float height = -1, bool vflip=true) override;

	 void setupScreenPerspective(float width = -1, float height = -1, float fov = 60, float nearDist = 0, float farDist = 0) override;

	 void setupScreenOrtho(float width = -1, float height = -1, float nearDist = -1, float farDist = 1) override;

	 ofRectangle getCurrentViewport() const override;

	 ofRectangle getNativeViewport() const override;

	 int getViewportWidth() const override;
	 int getViewportHeight() const override;

	 void setCoordHandedness(ofHandednessType handedness) override;
	 ofHandednessType getCoordHandedness() const override;

	//our openGL wrappers
	 void pushMatrix() override;
	 void popMatrix() override;
	 void translate(float x, float y, float z = 0) override;
	 void translate(const glm::vec3 & p) override;
	 void scale(float xAmnt, float yAmnt, float zAmnt = 1) override;

	 void rotateDeg(float degrees, float vecX, float vecY, float vecZ) override;
	 void rotateXDeg(float degrees) override;
	 void rotateYDeg(float degrees) override;
	 void rotateZDeg(float degrees) override;
	 void rotateDeg(float degrees) override;

	 void rotateRad(float radians, float vecX, float vecY, float vecZ) override;
	 void rotateXRad(float radians) override;
	 void rotateYRad(float radians) override;
	 void rotateZRad(float radians) override;
	 void rotateRad(float radians) override;
	void loadIdentityMatrix (void) override;

	void loadMatrix (const glm::mat4 & m) override;

	void loadMatrix (const float * m) override;

	void multMatrix (const glm::mat4 & m) override;

	void multMatrix (const float * m) override;

	void setOrientation(ofOrientation orientation, bool vflip) override;

	bool isVFlipped() const override;

	void matrixMode(ofMatrixMode mode) override;

	void loadViewMatrix(const glm::mat4& m) override;

	void multViewMatrix(const glm::mat4& m) override;

	glm::mat4 getCurrentViewMatrix() const override;


	// screen coordinate things / default gl values
	 void setupGraphicDefaults() override;

	 void setupScreen() override;

	// color options
	void setColor(float r, float g, float b) override;

	void setColor(float r, float g, float b, float a) override;

	void setColor(const ofFloatColor & color) override;

	void setColor(const ofFloatColor & color, float _a) override;

	void setColor(float gray) override;

	void setHexColor( int hexColor ) override;

	// bg color
	ofFloatColor getBackgroundColor() override;

	void setBackgroundColor(const ofFloatColor & color) override;

	bool getBackgroundAuto() override;

	void background(const ofFloatColor & c) override;

	void background(float brightness) override;

	void background(int hexColor, int _a=255) override;

	void background(float r, float g, float b, float a=1.f) override;

	void setBackgroundAuto(bool bManual) override;

	void clear() override;

	void clear(float r, float g, float b, float a=0) override;

	void clear(float brightness, float a=0) override;

	void clearAlpha() override;

	// drawing modes
	void setRectMode(ofRectMode mode) override;

	ofRectMode getRectMode() override;

	void setFillMode(ofFillFlag fill) override;

	ofFillFlag getFillMode() override;

	void setLineWidth(float lineWidth) override;
	void setPointSize(float pointSize) override;
	void setDepthTest(bool depthTest) override;

	void setBlendMode(ofBlendMode blendMode) override;
	void setLineSmoothing(bool smooth) override;
	void setCircleResolution(int res) override;
	void enablePointSprites();
	void disablePointSprites();

	void enableAntiAliasing() override;

	void disableAntiAliasing() override;

	void setBitmapTextMode(ofDrawBitmapMode mode) override;

	ofStyle getStyle() const override;

	void pushStyle() override;

	void popStyle() override;

	void setStyle(const ofStyle & style) override;

	void setCurveResolution(int res) override;

	void setPolyMode(ofPolyWindingMode mode) override;

	// drawing
	void drawLine(float x1, float y1, float z1, float x2, float y2, float z2) const override;

	void drawRectangle(float x, float y, float z, float w, float h) const override;

	void drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) const override;

	void drawCircle(float x, float y, float z, float radius) const override;

	void drawEllipse(float x, float y, float z, float width, float height) const override;

	void drawString(std::string text, float x, float y, float z) const override;

	void drawString(const ofTrueTypeFont & font, std::string text, float x, float y) const override;

	void bind(const ofCamera & camera, const ofRectangle & viewport) override;
	void unbind(const ofCamera & camera) override;

	const of3dGraphics & get3dGraphics() const override;

	of3dGraphics & get3dGraphics() override;

	ofPath & getPath() override;

	std::vector<std::shared_ptr<ofBaseRenderer> > renderers;
	of3dGraphics graphics3d;
	ofPath path;
};
