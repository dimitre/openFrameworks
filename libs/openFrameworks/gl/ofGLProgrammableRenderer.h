#pragma once

#include "ofGLBaseTypes.h"
#include "ofShader.h"
#include "of3dGraphics.h"
// MARK: Optimization pointers in next four objects
#include "ofMatrixStack.h"
#include "ofPolyline.h"
#include "ofBitmapFont.h"
#include "ofPath.h"

class ofShapeTessellation;
class ofFbo;
class ofVbo;
static const int OF_NO_TEXTURE=-1;

class ofGLProgrammableRenderer: public ofBaseGLRenderer{
public:
    ofGLProgrammableRenderer(const ofAppBaseWindow * window);

	void setup(int glVersionMajor, int glVersionMinor);

    static const std::string TYPE;
	const std::string & getType() override { return TYPE; }

    void startRender() override;
    void finishRender() override;

	using ofBaseRenderer::draw;
	using ofBaseGLRenderer::draw;
	void draw(const ofMesh & vertexData, ofPolyRenderMode renderType, bool useColors, bool useTextures, bool useNormals) const override;
    void draw(const of3dPrimitive& model, ofPolyRenderMode renderType) const override;
    void draw(const ofNode& node) const override;
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
	void setupScreenPerspective(float width = -1, float height = -1, float fov = 60, float nearDist = 0, float farDist = 0) override;
	void setupScreenOrtho(float width = -1, float height = -1, float nearDist = -1, float farDist = 1) override;
	void setOrientation(ofOrientation orientation, bool vFlip) override;
	ofRectangle getCurrentViewport() const override;
	ofRectangle getNativeViewport() const override;
	int getViewportWidth() const override;
	int getViewportHeight() const override;
	bool isVFlipped() const override;

	void setCoordHandedness(ofHandednessType handedness) override;
	ofHandednessType getCoordHandedness() const override;

	//our openGL wrappers
	void pushMatrix() override;
	void popMatrix() override;
	void translate(float x, float y, float z = 0) override;
	void translate(const glm::vec3 & p) override;
	void scale(float xAmnt, float yAmnt, float zAmnt = 1) override;
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

    /// \brief Queries the current OpenGL matrix state
    ///
    /// Returns the specified matrix as held by the renderer's current matrix
    /// stack.
    ///
    /// You can query one of the following:
    ///
    /// [OF_MATRIX_MODELVIEW | OF_MATRIX_PROJECTION | OF_MATRIX_TEXTURE]
    ///
    /// Each query will return the state of the matrix as it was uploaded to
    /// the shader currently bound.
    ///
    /// \param	matrixMode_ Which matrix mode to query
    /// \note   If an invalid matrixMode is queried, this method will return the
    ///         identity matrix, and print an error message.
	glm::mat4 getCurrentMatrix(ofMatrixMode matrixMode_) const override;
	glm::mat4 getCurrentOrientationMatrix() const override;
	glm::mat4 getCurrentViewMatrix() const override;
	glm::mat4 getCurrentNormalMatrix() const override;
	glm::mat4 getCurrentModelMatrix() const;

	glm::vec3 getCurrentEyePosition() const;

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

	/// \brief Enable size attenuation for line widths. Width changes based on distance from camera.
	void enableLineSizeAttenuation();
	/// \brief Disable size attenuation for line widths. Consistent width based on pixels in screen space (default).
	void disableLineSizeAttenuation();
	/// \brief Enable OF's line shaders for rendering lines of varying widths, set by ofSetLineWidth( width ); (default).
	void enableLinesShaders();
	/// \brief Disable OF's line shaders to enable openGL rendering. Does not support varying line widths.
	void disableLinesShaders();
	bool areLinesShadersEnabled() const;

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

	bool getBackgroundAuto() override;
	void setBackgroundAuto(bool bManual) override;		// default is true

	void clear() override;
	void clear(float r, float g, float b, float a=0.f) override;
	void clear(float brightness, float a=0.f) override;
	void clearAlpha() override;


	// drawing
	void drawLine(float x1, float y1, float z1, float x2, float y2, float z2) const override;
	void drawRectangle(float x, float y, float z, float w, float h) const override;
	void drawTriangle(float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) const override;
	void drawCircle(float x, float y, float z, float radius) const override;
	void drawEllipse(float x, float y, float z, float width, float height) const override;
	void drawString(std::string text, float x, float y, float z) const override;
	void drawString(const ofTrueTypeFont & font, std::string text, float x, float y) const override;


	void enableTextureTarget(const ofTexture & tex, int textureLocation) override;
	void disableTextureTarget(int textureTarget, int textureLocation) override;
	void setAlphaMaskTex(const ofTexture & tex) override;
	void disableAlphaMask() override;
	GLenum getCurrentTextureTarget();

	const ofShader & getCurrentShader() const;

	void bind(const ofBaseMaterial & material) override;
	void bind(const ofShadow & shadow) override;
	void bind(const ofShadow & shadow, GLenum aCubeFace) override;
	void bind(const ofShader & shader) override;
	void bind(const ofTexture & texture, int location) override;
	void bind(const ofBaseVideoDraws & video) override;
	void bind(const ofCamera & camera, const ofRectangle & viewport) override;
	void unbind(const ofBaseMaterial & material) override;
	void unbind(const ofShadow & shadow) override;
	void unbind(const ofShadow & shadow, GLenum aCubeFace) override;
	void unbind(const ofShader & shader) override;
	void unbind(const ofTexture & texture, int location) override;
	void unbind(const ofBaseVideoDraws & video) override;
	void unbind(const ofCamera & camera) override;

	void bind(const ofFbo & fbo) override;
#ifndef TARGET_OPENGLES
	void bindForBlitting(const ofFbo & fboSrc, ofFbo & fboDst, int attachmentPoint) override;
#endif
	void unbind(const ofFbo & fbo) override;

    void begin(const ofFbo & fbo, ofFboMode mode) override;
	void end(const ofFbo & fbo) override;

	ofStyle getStyle() const override;
	void pushStyle() override;
	void popStyle() override;
	void setStyle(const ofStyle & style) override;
	void setCurveResolution(int resolution) override;
	void setPolyMode(ofPolyWindingMode mode) override;

	const ofShader * getVideoShader(const ofBaseVideoDraws & video) const;
	void setVideoShaderUniforms(const ofBaseVideoDraws & video, const ofShader & shader) const;

    void enableLighting() override;
    void disableLighting() override;
    bool getLightingEnabled() override;

    void enableLight(int lightIndex) override;
    void disableLight(int lightIndex) override;

    //    void enableSeparateSpecularLight(){}
    //    void disableSeparateSpecularLight(){}
	// void setSmoothLighting(bool ){}
	// void setGlobalAmbientColor(const ofFloatColor& ){}
	// FIXME: base class is pure virtual exige implementation and we have no implementation here.
	// void setLightSpotlightCutOff(int , float ){}
	// void setLightSpotConcentration(int , float ){}
	// void setLightAttenuation(int lightIndex, float constant, float linear, float quadratic ){}
	// void setLightAmbientColor(int lightIndex, const ofFloatColor& c){}
	// void setLightDiffuseColor(int lightIndex, const ofFloatColor& c){}
	// void setLightSpecularColor(int lightIndex, const ofFloatColor& c){}
	// void setLightPosition(int lightIndex, const glm::vec4 & position){}
	// void setLightSpotDirection(int lightIndex, const glm::vec4 & direction){}

	std::string defaultVertexShaderHeader(GLenum textureTarget);
	std::string defaultFragmentShaderHeader(GLenum textureTarget);

	int getGLVersionMajor() override;
	int getGLVersionMinor() override;

	void saveScreen(int x, int y, int w, int h, ofPixels & pixels) override;
	void saveFullViewport(ofPixels & pixels) override;

	const of3dGraphics & get3dGraphics() const override;
	of3dGraphics & get3dGraphics() override;

private:


	ofPolyline circlePolyline;
	ofPolyline circleOutlinePolyline;
	mutable ofMesh circleMesh, circleOutlineMesh;
	mutable ofMesh triangleMesh;
	mutable ofMesh rectMesh;
	mutable ofMesh lineMesh;
	mutable ofVbo meshVbo;
	mutable ofMesh polylineMesh;

	// Static unit quad VBO for efficient texture drawing
	struct StaticQuadVBO {
		ofVbo vbo;
		bool initialized = false;
		
		void init();
	};
	mutable StaticQuadVBO staticQuadVBO;

	// when adding more draw modes, POINTS, LINES, etc.
	// store in a structure so we don't have to create a lot of variables
	// this structure if based on the one from ofMaterial
	class ShaderCollection {
	public:
		void bindAttribute( GLuint location, const std::string & name );
		void bindDefaults();
		void linkPrograms();
		void setupAllVertexShaders(const std::string &aShaderSrc);

		ofShader texRectColor;
		ofShader texRectNoColor;
		ofShader tex2DColor;
		ofShader tex2DNoColor;
		ofShader noTexColor;
		ofShader noTexNoColor;
	};

	// useful for lines //
	class LinesBundle {
	public:
		void setMeshDataToVbo();
		std::vector<glm::vec4> lineMeshNextVerts;
		std::vector<glm::vec4> lineMeshPrevVerts;
		ofVbo vbo;
		ofMesh mesh;
		int vertAttribPrev = 4;
		int vertAttribNext = 5;
	};

	struct TextureUniform {
		ofTextureData texData;
		// nh: not going to store a texture since we don't want to retain the texture here
		// ofTexture texture;
		int textureLocation;
		std::string uniformName;
	};

	void uploadCurrentMatrix();


	void startSmoothing();
	void endSmoothing();

	void beginDefaultShader();
	std::shared_ptr<ShaderCollection>& getShaderCollectionForMode(GLuint drawMode);
	void uploadMatrices();
	void setDefaultUniforms();

	// adding a drawMode variable that will switch shaders based on GL_TRIANGLES, GL_POINTS or GL_LINES
	void setAttributes(bool vertices, bool color, bool tex, bool normals, GLuint drawMode);
//	void setAttributes(bool vertices, bool color, bool tex, bool normals);
	void setAlphaBitmapText(bool bitmapText);


	// LINES
	void configureMeshToMatchWithNewVertsAndIndices(const ofMesh& aSrcMesh, ofMesh& aDstMesh, std::size_t aTargetNumVertices, std::size_t aTargetNumIndices);
	void configureLinesBundleFromMesh(LinesBundle& aLinesBundle, GLuint drawMode, const ofMesh& amesh);


	ofMatrixStack matrixStack;

	bool bBackgroundAuto;
	int major, minor;

	const ofShader * currentShader;

	bool verticesEnabled, colorsEnabled, texCoordsEnabled, normalsEnabled, bitmapStringEnabled;
	bool pointSpritesEnabled;
	bool usingCustomShader, settingDefaultShader, usingVideoShader;
	int currentTextureTarget;

	bool wrongUseLoggedOnce;
	bool uniqueShader;

	const ofBaseMaterial * currentMaterial;
	int alphaMaskTextureTarget;

	const ofShadow* currentShadow;
	bool bIsShadowDepthPass;
	GLenum shadowCubeFace;
	bool bCustomShadowShader = false;

	ofStyle currentStyle;
	std::deque <ofStyle> styleHistory;
	of3dGraphics graphics3d;
	ofBitmapFont bitmapFont;
	ofPath path;
	const ofAppBaseWindow * window;

	mutable GLuint mDrawMode = GL_TRIANGLES;
	std::unordered_map<GLuint, LinesBundle> mLinesBundleMap;
	mutable bool mBRenderingLines = false;
	mutable bool mBLineSizeAttenutation = false; // screen space
	mutable bool mBEnableLinesShaders = true;

	//	the index GL_TRIANGLES store everything that is not GL_POINTS or GL_LINES, GL_LINE_STRIP
	std::unordered_map<GLuint, std::shared_ptr<ShaderCollection> > mDefaultShadersMap;
	std::vector<TextureUniform> mUniformsTex;

	//	ofShader defaultTexRectColor;
	//	ofShader defaultTexRectNoColor;
	//	ofShader defaultTex2DColor;
	//	ofShader defaultTex2DNoColor;
	//	ofShader defaultNoTexColor;
	//	ofShader defaultNoTexNoColor;
	ofShader defaultUniqueShader;
#ifdef TARGET_ANDROID
	ofShader defaultOESTexColor;
	ofShader defaultOESTexNoColor;
#endif

	ofShader alphaMaskRectShader;
	ofShader alphaMask2DShader;

	ofShader bitmapStringShader;

	ofShader shaderPlanarYUY2;
	ofShader shaderNV12;
	ofShader shaderNV21;
	ofShader shaderPlanarYUV;
	ofShader shaderPlanarYUY2Rect;
	ofShader shaderNV12Rect;
	ofShader shaderNV21Rect;
	ofShader shaderPlanarYUVRect;

	glm::vec3 currentEyePos;

	//void setDefaultFramebufferId(const GLuint& fboId_); ///< windowing systems might use this to set the default framebuffer for this renderer.

	//void pushFramebufferId(); // pushes currentFramebuffer onto framebufferStack
	//const GLuint& popFramebufferId(); /// returns topmost element in framebufferIdStack or 0, removes topmost element from stack.
	//const GLuint& getFramebufferId(); ///< returns current target bound to GL_FRAMEBUFFER_BINDING, initially set to defaultFramebufferId
	//void setFramebufferId(const GLuint& fboId_); // sets the current framebuffer id

	// framebuffer binding state
	std::deque<GLuint> framebufferIdStack;	///< keeps track of currently bound framebuffers
	GLuint defaultFramebufferId;		///< default GL_FRAMEBUFFER_BINDING, windowing frameworks might want to set this to their MSAA framebuffer, defaults to 0
    GLuint currentFramebufferId;		///< the framebuffer id currently bound to the GL_FRAMEBUFFER target

	// UNUSED / commented out;
//	uint64_t beginDefaultShaderFrame;



	struct matricesBuffer {
		glm::mat4 modelMatrix { 1.0f };
		glm::mat4 viewMatrix { 1.0f };
		glm::mat4 modelViewMatrix { 1.0f };
		glm::mat4 modelViewProjectionMatrix { 1.0f };
		glm::mat4 projectionMatrix { 1.0f };
		glm::mat4 textureMatrix { 1.0f };
	} matrices;
	ofBufferObject buffer;
};
