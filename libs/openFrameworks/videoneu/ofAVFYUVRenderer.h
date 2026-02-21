#pragma once

#include "ofShader.h"
#include "ofTexture.h"
#include "ofVboMesh.h"

// Inline YUV to RGB shader renderer for ofAVFVideoPlayer
// Auto-detects GL version and uses appropriate shader

class ofAVFYUVRenderer {
public:
    ofAVFYUVRenderer();
    ~ofAVFYUVRenderer();
    
    // Disable copy
    ofAVFYUVRenderer(const ofAVFYUVRenderer&) = delete;
    ofAVFYUVRenderer& operator=(const ofAVFYUVRenderer&) = delete;
    
    // Setup - call once before use
    bool setup(int w, int h);
    
    // Upload YUV data
    // yPlane: full resolution Y data (w x h)
    // uvPlane: half resolution UV interleaved (w/2 x h/2, 2 bytes per pixel)
    void uploadYUV(const uint8_t* yPlane, int yStride,
                   const uint8_t* uvPlane, int uvStride);
    
    // Draw the video
    void draw(float x, float y, float w, float h) const;
    void draw(float x, float y) const;
    
    // Bind/unbind for custom rendering
    void bind() const;
    void unbind() const;
    
    // Get the output texture (after YUV->RGB conversion)
    // Note: This is the result of shader processing
    ofTexture& getTexture();
    const ofTexture& getTexture() const;
    
    bool isAllocated() const;
    float getWidth() const { return width; }
    float getHeight() const { return height; }
    
private:
    bool setupShader();
    bool setupShaderGL2();
    bool setupShaderGL3();
    bool setupShaderGLES2();
    bool setupShaderGLES3();
    
    ofShader shader;
    ofTexture yTexture;      // Luminance plane (R8)
    ofTexture uvTexture;     // Chrominance plane (RG8)
    ofTexture fboTexture;    // Output RGB texture (optional, for bind/unbind)
    ofVboMesh quad;
    
    int width = 0;
    int height = 0;
    bool bAllocated = false;
    
    // GL version detection
    enum GLVersion { UNKNOWN, GL2, GL3, GLES2, GLES3 };
    GLVersion glVersion = UNKNOWN;
};
