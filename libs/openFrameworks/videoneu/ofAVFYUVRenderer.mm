#include "ofAVFYUVRenderer.h"
#include "ofGLProgrammableRenderer.h"
#include "ofFbo.h"
#include "ofLog.h"

// MARK: - Inline Shaders

// OpenGL 2.1 / GLSL 120
static const char* vertGL2 = R"(
    #version 120
    varying vec2 vTexCoord;
    void main() {
        gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
        vTexCoord = gl_MultiTexCoord0.xy;
    }
)";

static const char* fragGL2 = R"(
    #version 120
    varying vec2 vTexCoord;
    uniform sampler2DRect yTex;
    uniform sampler2DRect uvTex;

    // BT.709 coefficients
    const vec3 YUV_R = vec3(1.164383,  0.000000,  1.596027);
    const vec3 YUV_G = vec3(1.164383, -0.391762, -0.812968);
    const vec3 YUV_B = vec3(1.164383,  2.017232,  0.000000);
    const vec3 YUV_OFFSET = vec3(-0.0625, -0.5, -0.5);

    void main() {
        float y = texture2DRect(yTex, vTexCoord).r;
        vec2 uv = texture2DRect(uvTex, vTexCoord * 0.5).rg;

        vec3 yuv = vec3(y, uv) + YUV_OFFSET;
        vec3 rgb = vec3(
            dot(yuv, YUV_R),
            dot(yuv, YUV_G),
            dot(yuv, YUV_B)
        );

        gl_FragColor = vec4(rgb, 1.0);
    }
)";

// OpenGL 3.3+ / GLSL 150
static const char* vertGL3 = R"(
    #version 150
    uniform mat4 modelViewProjectionMatrix;
    in vec4 position;
    in vec2 texcoord;
    out vec2 vTexCoord;
    void main() {
        gl_Position = modelViewProjectionMatrix * position;
        vTexCoord = texcoord;
    }
)";

static const char* fragGL3 = R"(
    #version 150
    uniform sampler2DRect yTex;
    uniform sampler2DRect uvTex;
    in vec2 vTexCoord;
    out vec4 fragColor;

    // BT.709 coefficients
    const vec3 YUV_R = vec3(1.164383,  0.000000,  1.596027);
    const vec3 YUV_G = vec3(1.164383, -0.391762, -0.812968);
    const vec3 YUV_B = vec3(1.164383,  2.017232,  0.000000);
    const vec3 YUV_OFFSET = vec3(-0.0625, -0.5, -0.5);

    void main() {
        float y = texture(yTex, vTexCoord).r;
        vec2 uv = texture(uvTex, vTexCoord * 0.5).rg;

        vec3 yuv = vec3(y, uv) + YUV_OFFSET;
        vec3 rgb = vec3(
            dot(yuv, YUV_R),
            dot(yuv, YUV_G),
            dot(yuv, YUV_B)
        );

        fragColor = vec4(rgb, 1.0);
    }
)";

// OpenGL ES 2.0
static const char* vertGLES2 = R"(
    precision highp float;
    attribute vec4 position;
    attribute vec2 texcoord;
    varying vec2 vTexCoord;
    uniform mat4 modelViewProjectionMatrix;
    void main() {
        gl_Position = modelViewProjectionMatrix * position;
        vTexCoord = texcoord;
    }
)";

static const char* fragGLES2 = R"(
    precision highp float;
    varying vec2 vTexCoord;
    uniform sampler2D yTex;
    uniform sampler2D uvTex;
    uniform vec2 texScale;

    // BT.709 coefficients
    const vec3 YUV_R = vec3(1.164383,  0.000000,  1.596027);
    const vec3 YUV_G = vec3(1.164383, -0.391762, -0.812968);
    const vec3 YUV_B = vec3(1.164383,  2.017232,  0.000000);
    const vec3 YUV_OFFSET = vec3(-0.0625, -0.5, -0.5);

    void main() {
        vec2 uv = vTexCoord * texScale;
        float y = texture2D(yTex, uv).r;
        vec2 uvChroma = texture2D(uvTex, uv * 0.5).ra; // Use RA for RG

        vec3 yuvVec = vec3(y, uvChroma) + YUV_OFFSET;
        vec3 rgb = vec3(
            dot(yuvVec, YUV_R),
            dot(yuvVec, YUV_G),
            dot(yuvVec, YUV_B)
        );

        gl_FragColor = vec4(rgb, 1.0);
    }
)";

// OpenGL ES 3.0
static const char* vertGLES3 = R"(
    #version 300 es
    precision highp float;
    in vec4 position;
    in vec2 texcoord;
    out vec2 vTexCoord;
    uniform mat4 modelViewProjectionMatrix;
    void main() {
        gl_Position = modelViewProjectionMatrix * position;
        vTexCoord = texcoord;
    }
)";

static const char* fragGLES3 = R"(
    #version 300 es
    precision highp float;
    uniform sampler2D yTex;
    uniform sampler2D uvTex;
    uniform vec2 texScale;
    in vec2 vTexCoord;
    out vec4 fragColor;

    // BT.709 coefficients
    const vec3 YUV_R = vec3(1.164383,  0.000000,  1.596027);
    const vec3 YUV_G = vec3(1.164383, -0.391762, -0.812968);
    const vec3 YUV_B = vec3(1.164383,  2.017232,  0.000000);
    const vec3 YUV_OFFSET = vec3(-0.0625, -0.5, -0.5);

    void main() {
        vec2 uv = vTexCoord * texScale;
        float y = texture(yTex, uv).r;
        vec2 uvChroma = texture(uvTex, uv * 0.5).rg;

        vec3 yuvVec = vec3(y, uvChroma) + YUV_OFFSET;
        vec3 rgb = vec3(
            dot(yuvVec, YUV_R),
            dot(yuvVec, YUV_G),
            dot(yuvVec, YUV_B)
        );

        fragColor = vec4(rgb, 1.0);
    }
)";

// MARK: - Implementation

ofAVFYUVRenderer::ofAVFYUVRenderer() {
    quad.setMode(OF_PRIMITIVE_TRIANGLE_FAN);
    quad.getVertices().resize(4);
    quad.getTexCoords().resize(4);
}

ofAVFYUVRenderer::~ofAVFYUVRenderer() {
    yTexture.clear();
    uvTexture.clear();
    shader.unload();
}

bool ofAVFYUVRenderer::setup(int w, int h) {
    if (bAllocated && width == w && height == h) {
        return true;
    }

    width = w;
    height = h;

    // Detect GL version
    #ifdef TARGET_OPENGLES
        #if defined(__VERSION__) && __VERSION__ >= 300
            glVersion = GLES3;
        #else
            glVersion = GLES2;
        #endif
    #else
        if (ofIsGLProgrammableRenderer()) {
            glVersion = GL3;
        } else {
            glVersion = GL2;
        }
    #endif

    ofLogNotice("ofAVFYUVRenderer") << "Using GL version: "
        << (glVersion == GL2 ? "GL2" : glVersion == GL3 ? "GL3" :
            glVersion == GLES2 ? "GLES2" : glVersion == GLES3 ? "GLES3" : "Unknown");

    if (!setupShader()) {
        ofLogError("ofAVFYUVRenderer") << "Failed to setup shader";
        return false;
    }

    // Allocate Y texture (full res, single channel)
    #ifdef TARGET_OPENGLES
        yTexture.allocate(width, height, GL_LUMINANCE);
        uvTexture.allocate(width / 2, height / 2, GL_LUMINANCE_ALPHA);
    #else
        yTexture.allocate(width, height, GL_R8, false, GL_RED, GL_UNSIGNED_BYTE);
        uvTexture.allocate(width / 2, height / 2, GL_RG8, false, GL_RG, GL_UNSIGNED_BYTE);
    #endif

    // Setup quad
    quad.setVertex(0, glm::vec3(0, 0, 0));
    quad.setVertex(1, glm::vec3(width, 0, 0));
    quad.setVertex(2, glm::vec3(width, height, 0));
    quad.setVertex(3, glm::vec3(0, height, 0));

    quad.setTexCoord(0, glm::vec2(0, 0));
    quad.setTexCoord(1, glm::vec2(width, 0));
    quad.setTexCoord(2, glm::vec2(width, height));
    quad.setTexCoord(3, glm::vec2(0, height));

    bAllocated = true;
    return true;
}

bool ofAVFYUVRenderer::setupShader() {
    switch (glVersion) {
        case GL2: return setupShaderGL2();
        case GL3: return setupShaderGL3();
        case GLES2: return setupShaderGLES2();
        case GLES3: return setupShaderGLES3();
        default: return setupShaderGL3(); // Default to GL3
    }
}

bool ofAVFYUVRenderer::setupShaderGL2() {
    return shader.setupShaderFromSource(GL_VERTEX_SHADER, vertGL2) &&
           shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragGL2) &&
           shader.linkProgram();
}

bool ofAVFYUVRenderer::setupShaderGL3() {
    return shader.setupShaderFromSource(GL_VERTEX_SHADER, vertGL3) &&
           shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragGL3) &&
           shader.linkProgram();
}

bool ofAVFYUVRenderer::setupShaderGLES2() {
    return shader.setupShaderFromSource(GL_VERTEX_SHADER, vertGLES2) &&
           shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragGLES2) &&
           shader.linkProgram();
}

bool ofAVFYUVRenderer::setupShaderGLES3() {
    return shader.setupShaderFromSource(GL_VERTEX_SHADER, vertGLES3) &&
           shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragGLES3) &&
           shader.linkProgram();
}

void ofAVFYUVRenderer::uploadYUV(const uint8_t* yPlane, int yStride,
                                  const uint8_t* uvPlane, int uvStride) {
    if (!bAllocated) return;

    // Upload Y plane
    yTexture.bind();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, yStride);
    glTexSubImage2D(yTexture.texData.textureTarget, 0, 0, 0, width, height,
                    #ifdef TARGET_OPENGLES
                    GL_LUMINANCE,
                    #else
                    GL_RED,
                    #endif
                    GL_UNSIGNED_BYTE, yPlane);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    yTexture.unbind();

    // Upload UV plane
    uvTexture.bind();
    glPixelStorei(GL_UNPACK_ROW_LENGTH, uvStride / 2); // 2 bytes per pixel
    glTexSubImage2D(uvTexture.texData.textureTarget, 0, 0, 0, width / 2, height / 2,
                    #ifdef TARGET_OPENGLES
                    GL_LUMINANCE_ALPHA,
                    #else
                    GL_RG,
                    #endif
                    GL_UNSIGNED_BYTE, uvPlane);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
    uvTexture.unbind();
}

void ofAVFYUVRenderer::draw(float x, float y, float w, float h) const {
    if (!bAllocated) return;

    shader.begin();
    shader.setUniformTexture("yTex", yTexture, 0);
    shader.setUniformTexture("uvTex", uvTexture, 1);

    #ifdef TARGET_OPENGLES
        shader.setUniform2f("texScale", 1.0f, 1.0f);
    #endif

    ofPushMatrix();
    ofTranslate(x, y);
    if (w != width || h != height) {
        ofScale(w / width, h / height);
    }
    quad.draw();
    ofPopMatrix();

    shader.end();
}

void ofAVFYUVRenderer::draw(float x, float y) const {
    draw(x, y, width, height);
}

void ofAVFYUVRenderer::bind() const {
    // For custom rendering - we'd need an FBO here
    // For now, just bind the shader and textures
    shader.begin();
    shader.setUniformTexture("yTex", yTexture, 0);
    shader.setUniformTexture("uvTex", uvTexture, 1);
    #ifdef TARGET_OPENGLES
        shader.setUniform2f("texScale", 1.0f, 1.0f);
    #endif
}

void ofAVFYUVRenderer::unbind() const {
    shader.end();
}

ofTexture& ofAVFYUVRenderer::getTexture() {
    // For now, return Y texture as placeholder
    // Full implementation would render to FBO and return that texture
    static ofTexture nullTex;
    if (!bAllocated) return nullTex;

    // Note: To properly support this, we'd need to render to FBO
    // For now, users should use draw() or bind()/unbind()
    ofLogWarning("ofAVFYUVRenderer") << "getTexture() not fully implemented - use draw() or bind() instead";
    return nullTex;
}

const ofTexture& ofAVFYUVRenderer::getTexture() const {
    return const_cast<ofAVFYUVRenderer*>(this)->getTexture();
}

bool ofAVFYUVRenderer::isAllocated() const {
    return bAllocated;
}
