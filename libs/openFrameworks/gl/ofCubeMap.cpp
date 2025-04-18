#include "ofShader.h"
#include "ofCubeMap.h"
#include "ofImage.h"
#include "of3dUtils.h"
#include "ofGLBaseTypes.h"
#include "ofGLUtils.h"
#include "ofGLProgrammableRenderer.h"
#include "ofCubeMapShaders.h"
#include "ofFbo.h"
#include "ofTexture.h"
#include "ofFileUtils.h"
#include "ofMaterial.h"

#ifdef TARGET_ANDROID
#include "ofAppAndroidWindow.h"
#endif

#if !defined(GLM_FORCE_CTOR_INIT)
	#define GLM_FORCE_CTOR_INIT
#endif
#if !defined(GLM_ENABLE_EXPERIMENTAL)
	#define GLM_ENABLE_EXPERIMENTAL
#endif
#include <glm/gtx/transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <map>



using std::weak_ptr;
using std::vector;
using std::shared_ptr;

ofVboMesh ofCubeMap::sCubeMesh;
ofShader ofCubeMap::shaderBrdfLUT;
ofTexture ofCubeMap::sBrdfLutTex;

void ofCubeMap::setExposure(float aExposure) {
	data->exposure=ofClamp(aExposure, 0.0f, 1.0f);
}

// texture management copied from ofTexture
static std::map<GLuint,int> & getTexturesIndex(){
	static std::map<GLuint,int> * textureReferences = new std::map<GLuint,int>;
	return *textureReferences;
}

static void retain(GLuint id){
	if(id!=0){
		if(getTexturesIndex().find(id)!=getTexturesIndex().end()){
			getTexturesIndex()[id]++;
		}else{
			getTexturesIndex()[id]=1;
		}
	}
}

static void release(GLuint id){
	// try to free up the texture memory so we don't reallocate
	// http://www.opengl.org/documentation/specs/man_pages/hardcopy/GL/html/gl/deletetextures.html
	if (id != 0){
		if(getTexturesIndex().find(id)!=getTexturesIndex().end()){
			getTexturesIndex()[id]--;
			if(getTexturesIndex()[id]==0){
				
#ifdef TARGET_ANDROID
				if (!ofAppAndroidWindow::isSurfaceDestroyed())
#endif
					glDeleteTextures(1, (GLuint *)&id);
				
				getTexturesIndex().erase(id);
			}
		}else{
			ofLogError("ofCubeMap") << "release(): something's wrong here, releasing unknown texture id " << id;
			
#ifdef TARGET_ANDROID
			if (!ofAppAndroidWindow::isSurfaceDestroyed())
#endif
				glDeleteTextures(1, (GLuint *)&id);
		}
	}
}

//----------------------------------------
#ifdef TARGET_ANDROID
// TODO: Hook this up to an event
void ofCubeMap::regenerateAllTextures() {
	for(size_t i=0;i<getCubeMapsData().size(); i++) {
		if(!getCubeMapsData()[i].expired()) {
			auto cubeMap = getCubeMapsData()[i].lock();
			ofCubeMap::clearTextureData(cubeMap);
		}
	}
	sBrdfLutTex.clear();
}
#endif

//--------------------------------------------------------------
std::vector<std::weak_ptr<ofCubeMap::Data>>& ofCubeMap::getCubeMapsData() {
	static std::vector<std::weak_ptr<ofCubeMap::Data>> cubeMapsDataActive;
	return cubeMapsDataActive;
}

//--------------------------------------------------------------
bool ofCubeMap::hasActiveCubeMap() {
	for(size_t i=0;i<getCubeMapsData().size();i++){
		auto cubeMap = getCubeMapsData()[i].lock();
		if(cubeMap && cubeMap->isEnabled && cubeMap->index > -1 ){
			return true;
			break;
		}
	}
	return false;
}

//--------------------------------------------------------------
std::shared_ptr<ofCubeMap::Data> ofCubeMap::getActiveData() {
	for(size_t i=0; i < getCubeMapsData().size();i++){
		auto cubeMap = getCubeMapsData()[i].lock();
		if(cubeMap && cubeMap->isEnabled && cubeMap->index > -1 ){
			return cubeMap;
		}
	}
	return std::shared_ptr<ofCubeMap::Data>();
}

//--------------------------------------------------------------
void ofCubeMap::clearTextureData(std::shared_ptr<ofCubeMap::Data> adata) {
	if( adata ) {
		if( adata->bPreFilteredMapAllocated ) {
			adata->bPreFilteredMapAllocated=false;
			release(adata->preFilteredMapId);
		}
		
		if( adata->bIrradianceAllocated ) {
			adata->bIrradianceAllocated = false;
			release(adata->irradianceMapId);
		}
		if( adata->bCubeMapAllocated ) {
			adata->bCubeMapAllocated = false;
			release(adata->cubeMapId);
		}
	}
}

//--------------------------------------------------------------
void ofCubeMap::_checkSetup() {
	if( data->index < 0 ) {
		bool bFound = false;
		// search for the first free block
		for(size_t i=0; i< getCubeMapsData().size(); i++) {
			if(getCubeMapsData()[i].expired()) {
				data->index = i;
				getCubeMapsData()[i] = data;
				bFound = true;
				break;
			}
		}
		if(!bFound && ofIsGLProgrammableRenderer()){
			getCubeMapsData().push_back(data);
			data->index = getCubeMapsData().size() - 1;
			bFound = true;
		}
	}
	// we should remove empty slots //
}

//----------------------------------------
const ofTexture& ofCubeMap::getBrdfLutTexture() {
	return sBrdfLutTex;
}

//----------------------------------------
ofCubeMap::ofCubeMap() {
	data = std::make_shared<ofCubeMap::Data>();
	_checkSetup();
	projectionMat = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f );
}

//----------------------------------------
ofCubeMap::ofCubeMap(const ofCubeMap & mom) {
	clear();
	
	if(data) {
		data.reset();
	}
	if( mom.data ) {
		data = std::make_shared<ofCubeMap::Data>(*mom.data);
		if( data->bCubeMapAllocated ) {
			retain(data->cubeMapId);
		}
		if( data->bIrradianceAllocated ) {
			retain(data->irradianceMapId);
		}
		if( data->bPreFilteredMapAllocated ) {
			retain(data->preFilteredMapId);
		}
	}
	if( !data ) {
		data = std::make_shared<ofCubeMap::Data>();
	}
	data->index = -1;
	if( mom.data ) {
		data->settings = mom.data->settings;
	}
	_checkSetup(); // grab a new slot in ofCubeMapsData
	mGLInternalFormat = mom.mGLInternalFormat;
}

//----------------------------------------
ofCubeMap::ofCubeMap(ofCubeMap && mom) {
	clear();
	// taking ownership of the data shared_ptr
	data = mom.data;
	mGLInternalFormat = mom.mGLInternalFormat;
}

//----------------------------------------
ofCubeMap::~ofCubeMap() {
	clear();
}

//--------------------------------------------------------------
ofCubeMap & ofCubeMap::operator=(const ofCubeMap & mom){
	if(&mom==this) return *this;
	clear();
	
	if(data) {
		data.reset();
	}
	if( mom.data ) {
		data = std::make_shared<ofCubeMap::Data>(*mom.data);
		if( data->bCubeMapAllocated ) {
			retain(data->cubeMapId);
		}
		if( data->bIrradianceAllocated ) {
			retain(data->irradianceMapId);
		}
		if( data->bPreFilteredMapAllocated ) {
			retain(data->preFilteredMapId);
		}
	}
	if( !data ) {
		data = std::make_shared<ofCubeMap::Data>();
	}
	if( mom.data ) {
		data->settings = mom.data->settings;
	}
	data->index = -1;
	_checkSetup(); // grab a new slot in ofCubeMapsData
	mGLInternalFormat = mom.mGLInternalFormat;
	
	return *this;
}

//--------------------------------------------------------------
ofCubeMap& ofCubeMap::operator=(ofCubeMap && mom) {
	clear();
	data = mom.data;
	mGLInternalFormat = mom.mGLInternalFormat;
	return *this;
}

//----------------------------------------
GLenum ofCubeMap::getTextureTarget() {
	return GL_TEXTURE_CUBE_MAP;
}

//----------------------------------------
bool isPowerOfTwo(int x) {
	/* First x in the below expression is for the case when
	 * x is 0 */
	return x && (!(x & (x - 1)));
}

//----------------------------------------
bool ofCubeMap::load( const of::filesystem::path & apath, int aFaceResolution, bool aBFlipY ) {
	return load(apath, aFaceResolution, aBFlipY, 32, 128 );
}

//----------------------------------------
bool ofCubeMap::load( const of::filesystem::path & apath, int aFaceResolution, bool aBFlipY, int aIrradianceRes, int aPreFilterRes ) {
	
	ofCubeMapSettings settings;
	settings.flipVertically = aBFlipY;
	settings.filePath = apath;
	
	settings.resolution = aFaceResolution;
	settings.irradianceRes = aIrradianceRes;
	settings.preFilterRes = aPreFilterRes;
	
	return load(settings);
}

//----------------------------------------
bool ofCubeMap::load( ofCubeMapSettings aSettings ) {
	if( !ofIsGLProgrammableRenderer() ) {
		ofLogError("ofCubeMap::load") << " cube maps only supported with programmable renderer.";
		return false;
	}
//	if( aSettings.resolution > 512 ) {
//		ofLogWarning("ofCubeMap :: load : a face resolution larger than 512 can cause issues.");
//	}
	
	if( aSettings.filePath.empty() ) {
		ofLogError("ofCubeMap :: load : must set file path");
		return false;
	}
	
	clear();
	
	auto ext = ofGetExtensionLower(aSettings.filePath);
	bool hdr = (ext == ".hdr" || ext == ".exr");
	
	if( hdr && !doesSupportHdr() ) {
		ofLogError("ofCubeMap :: load : hdr and exr not supported with this setup.");
		return false;
	}
	
	bool bLoadOk = false;
	data->settings = aSettings;
	
	ofTexture srcTex;
	bool bSrcTexIdGenerated = false;
	GLuint srcTexId;
	
	bool bArbTexEnabled = ofGetUsingArbTex();
	ofDisableArbTex();
	if( hdr ) {
		ofFloatPixels fpix;
		if( ofLoadImage(fpix, data->settings.filePath) ) {
			ofLogVerbose("ofCubeMap::load : loaded ") << ext << " image.";
			bLoadOk = true;
#if defined(TARGET_OPENGLES)
			// GL_RGB32F, GL_RGBA32F and GL_RGB16F is not supported in Emscripten opengl es, so we need to set to GL_RGBA16F or GL_RGBA32F. But GL_RGBA32F is not supported via opengl es on most mobile devices as of right now.
			#if defined(GL_RGBA16F)
			mGLInternalFormat = GL_RGBA16F;
			#endif
#elif !defined(TARGET_OPENGLES)
			#if defined(GL_RGB16F)
			mGLInternalFormat = GL_RGB16F;
			#endif
			if(aSettings.useMaximumPrecision) {
				#if defined(GL_RGB32F)
				ofLogVerbose("ofCubeMap :: load : using maximum precision: GL_RGB32F");
				mGLInternalFormat = GL_RGB32F;
				#endif
			}
#endif
			
			if(getNumPixelChannels() > 3 ) {
				// set alpha to 1.
				fpix.setImageType( OF_IMAGE_COLOR );
				fpix.setImageType( OF_IMAGE_COLOR_ALPHA );
			} else {
				fpix.setImageType( OF_IMAGE_COLOR );
			}
			
			if( isMediumPrecision() ) {
				int fw = fpix.getWidth();
				int fh = fpix.getHeight();
				
				ofLogVerbose("ofCubeMap :: loading the pixel data from hdr image as medium precision: ") << fw << " x " << fh;
				
				auto glFormat = getGLFormatFromInternalFormat();
				auto glType = getGLTypeFromInternalFormat();
				
				float* fPixData = fpix.getData();
				unsigned int numFs = fw * fh * fpix.getNumChannels();
				for( unsigned int i = 0; i < numFs; i++ ) {
					// clamp to the maximum value of float16, medium precision
					if(fPixData[i] > 65504.f) {
						fPixData[i] = 65504.f;
					}
				}
				
				
				bSrcTexIdGenerated = true;
				glGenTextures(1, &srcTexId );
				glBindTexture(GL_TEXTURE_2D, srcTexId);
				
				glTexImage2D(GL_TEXTURE_2D, 0, mGLInternalFormat, fw, fh, 0, glFormat, glType, fPixData );
				
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
				
				glBindTexture(GL_TEXTURE_2D, 0);
				
			} else {
				srcTex.loadData(fpix);
				srcTexId = srcTex.texData.textureID;
			}
		}
	} else {
		ofPixels ipix;
		if( ofLoadImage(ipix, data->settings.filePath) ) {
			bLoadOk = true;
			mGLInternalFormat = GL_RGB;
			srcTex.loadData(ipix);
			srcTexId = srcTex.texData.textureID;
		}
	}
	if( !bLoadOk ) {
		ofLogError("ofCubeMap :: failed to load image from ") << data->settings.filePath;
	} else {
#if defined(GL_TEXTURE_CUBE_MAP_SEAMLESS)
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
#endif
		
		if( !isPowerOfTwo(data->settings.resolution) ) {
			ofLogNotice("ofCubeMap :: load : changing resolution " ) << data->settings.resolution << " to next power of 2: " << ofNextPow2(data->settings.resolution);
			data->settings.resolution = ofNextPow2(data->settings.resolution);
		}
		if( !isPowerOfTwo(data->settings.irradianceRes) ) {
			ofLogNotice("ofCubeMap :: load : changing resolution " ) << data->settings.irradianceRes << " to next power of 2: " << ofNextPow2(data->settings.irradianceRes);
			data->settings.irradianceRes = ofNextPow2(data->settings.irradianceRes);
		}
		if( !isPowerOfTwo(data->settings.preFilterRes) ) {
			ofLogNotice("ofCubeMap :: load : changing resolution " ) << data->settings.preFilterRes << " to next power of 2: " << ofNextPow2(data->settings.preFilterRes);
			data->settings.preFilterRes = ofNextPow2(data->settings.preFilterRes);
		}
		
		_createCubeMap(srcTexId);
		if( isHdr() ) {
			
			int srcCubeFSize = std::max(256, data->settings.preFilterRes);
			GLuint cubeFid = _createFloatCubeMap(srcTexId, srcCubeFSize );
			// figure out the number of mip maps //
			data->maxMipLevels = log2(srcCubeFSize)+1;
			
			auto encFolder = data->settings.cacheDirectory;
			if( !encFolder.empty() ) {
				if( !ofDirectory::doesDirectoryExist( data->settings.cacheDirectory )) {
					#if !defined(TARGET_EMSCRIPTEN)
					if(!ofDirectory::createDirectory( data->settings.cacheDirectory )) {
						ofLogWarning("ofCubeMap :: load : unable to create directory: ") << data->settings.cacheDirectory;
					}
					#endif
				}
				encFolder = data->settings.cacheDirectory;
			}
			of::filesystem::path baseName = data->settings.filePath.stem(); // equivalent to getBaseName
			of::filesystem::path cacheIrrName { baseName };
			cacheIrrName += ("_irr_"+ofToString(data->settings.irradianceRes,0)+".exr");
			of::filesystem::path cachePrefilterName { baseName };
			cachePrefilterName += ("_pre_"+ofToString(data->settings.preFilterRes,0)+".exr");
			
			#if defined(TARGET_EMSCRIPTEN)
			if(data->settings.overwriteCache){
				data->settings.overwriteCache = false;
				ofLogNotice("ofCubeMap :: data->settings.overwriteCache is not supported on EMSCRIPTEN: ");
			}
			#endif
			
			bool bHasCachedIrr = false;
			bool bHasCachedPre = false;
			if( data->settings.useCache && !data->settings.overwriteCache ){
				bHasCachedIrr = _loadIrradianceMap(encFolder / cacheIrrName);
				ofLogVerbose("ofCubeMap :: _loadIrradianceMap: ") << bHasCachedIrr;
				bHasCachedPre = _loadPrefilterMap(encFolder / cachePrefilterName);
				ofLogVerbose("ofCubeMap :: _loadPrefilterMap: ") << bHasCachedPre;
			}
			
			bool bMakeCache = data->settings.useCache;
			#if defined(TARGET_EMSCRIPTEN)
			// not supporting making caches on Emscripten.
			bMakeCache = false;
			#endif
			
			if(!bHasCachedIrr){
				ofLogVerbose("ofCubeMap :: going to create irradiance map");
				_createIrradianceMap(cubeFid, bMakeCache, encFolder / cacheIrrName);
			}
			
			if(!bHasCachedPre){
				ofLogVerbose("ofCubeMap :: going to create pre filtered cube map");
				_createPrefilteredCubeMap(cubeFid, srcCubeFSize,bMakeCache,encFolder / cachePrefilterName );
			}
			
			glDeleteTextures(1, &cubeFid );
		}
	}
	
	if(bSrcTexIdGenerated) {
		// clean up the texture that was generated for the cube map if opengl es
		glDeleteTextures(1, &srcTexId);
	}
	
	if( bArbTexEnabled ) {
		ofEnableArbTex();
	}
	
	return bLoadOk;
}


//----------------------------------------
void ofCubeMap::clear() {
	clearTextureData(data);
}

//--------------------------------------------------------------
void ofCubeMap::draw() {
	drawCubeMap();
}

//--------------------------------------------------------------
void ofCubeMap::drawCubeMap() {
	if( !data->bCubeMapAllocated ) {
		ofLogWarning("ofCubeMap::drawCubeMap() : textures not allocated, not drawing");
		return;
	}
	
	_drawCubeStart(data->cubeMapId);
	shaderRender.setUniform1f("uExposure", getExposure() );
	shaderRender.setUniform1f("uRoughness", 0.0f );
	shaderRender.setUniform1f("uMaxMips", 1.0f );
	shaderRender.setUniform1f("uIsHDR", 0.0f );
	_drawCubeEnd();
}

//--------------------------------------------------------------
void ofCubeMap::drawIrradiance() {
	if( !data->bIrradianceAllocated ) {
		ofLogWarning("ofCubeMap::drawIrradiance() : textures not allocated, not drawing");
		return;
	}
	
	_drawCubeStart(data->irradianceMapId);
	shaderRender.setUniform1f("uExposure", getExposure() );
	shaderRender.setUniform1f("uRoughness", 0.0f );
	shaderRender.setUniform1f("uMaxMips", 1.0f );
	shaderRender.setUniform1f("uIsHDR", 1.0f );
	_drawCubeEnd();
}

//--------------------------------------------------------------
void ofCubeMap::drawPrefilteredCube(float aRoughness) {
	if( !data->bPreFilteredMapAllocated ) {
		ofLogWarning("ofCubeMap::drawPrefilteredCube() : textures not allocated, not drawing");
		return;
	}
	
	_drawCubeStart(data->preFilteredMapId);
	shaderRender.setUniform1f("uIsHDR", 1.0f );
	shaderRender.setUniform1f("uExposure", getExposure() );
	shaderRender.setUniform1f("uRoughness", aRoughness );
	shaderRender.setUniform1f("uMaxMips", (float)data->maxMipLevels );
	_drawCubeEnd();
}

//--------------------------------------------------------------
void ofCubeMap::_drawCubeStart(GLuint aCubeMapId) {
	_allocateCubeMesh();
	
	if( !shaderRender.isLoaded() ) {
		_loadRenderShader();
	}
	if( shaderRender.isLoaded() ) {
		glDepthFunc(GL_LEQUAL);
		shaderRender.begin();
		shaderRender.setUniformTexture("uCubeMap", getTextureTarget(), aCubeMapId, 0 );	}
}

//--------------------------------------------------------------
void ofCubeMap::_drawCubeEnd() {
	sCubeMesh.draw();
	shaderRender.end();
	glDepthFunc(GL_LESS); // set depth function back to default
}

//--------------------------------------------------------------
bool ofCubeMap::hasCubeMap() {
	if( !data ) return false;
	return data->bCubeMapAllocated;
}

//--------------------------------------------------------------
bool ofCubeMap::hasPrefilteredMap() {
	if( !data ) return false;
	return data->bPreFilteredMapAllocated;
}

//--------------------------------------------------------------
bool ofCubeMap::hasIrradianceMap(){
	if( !data ) return false;
	return data->bIrradianceAllocated;
}

//--------------------------------------------------------------
GLuint ofCubeMap::getTextureId(){
	if( !data ) return 0;
	return data->cubeMapId;
}

//--------------------------------------------------------------
bool ofCubeMap::doesSupportHdr(){
#if !defined(GL_FLOAT)
	return false;
#endif
#if defined(GL_RGB16F) || defined(GL_RGBA16F) || defined(GL_RGB32F) || defined(GL_RGBA32F)
	return true;
#endif
	return false;
}

//--------------------------------------------------------------
bool ofCubeMap::isHdr(){
	auto glType = getGLTypeFromInternalFormat();
	#if defined(GL_FLOAT)
	if(glType == GL_FLOAT) {
		return true;
	}
	#endif
	#if defined(GL_HALF_FLOAT)
	if(glType == GL_HALF_FLOAT) {
		return true;
	}
	#endif
	
	return false;
}

//--------------------------------------------------------------
bool ofCubeMap::isMediumPrecision(){
#if defined(GL_RGB16F)
	if(mGLInternalFormat == GL_RGB16F ) {
		return true;
	}
#endif
	
#if defined(GL_RGBA16F)
	if(mGLInternalFormat == GL_RGBA16F ) {
		return true;
	}
#endif
	return false;
}

//--------------------------------------------------------------
void ofCubeMap::setUseBrdfLutTexture(bool ab){
	#ifdef TARGET_OPENGLES
	data->settings.useLutTex = false;
	ofLogWarning("ofCubeMap::setUseBrdfLutTexture") << " brdf lut texture not supported on GLES.";
	return;
	#else
	data->settings.useLutTex = ab;
	if(ab && !sBrdfLutTex.isAllocated() ) {
		_createBrdfLUT();
	}
	#endif
}

//--------------------------------------------------------------
void ofCubeMap::_createCubeMap(GLuint aSrcTexId) {
	
	if( !data->bCubeMapAllocated ) {
		data->bCubeMapAllocated = true;
		glGenTextures(1, &data->cubeMapId );
		retain(data->cubeMapId);
	}
	
	auto internalFormat = mGLInternalFormat;
	auto glFormat = getGLFormatFromInternalFormat();
	auto glType = getGLTypeFromInternalFormat();
		
	internalFormat = ofGetGLInternalFormatFromPixelFormat(OF_PIXELS_RGB);
	#ifdef TARGET_OPENGLES
	internalFormat = ofGetGLInternalFormatFromPixelFormat(OF_PIXELS_RGBA);
	#endif

	glType = GL_UNSIGNED_BYTE;
	glFormat = GL_RGB;
	#ifdef TARGET_OPENGLES
	glFormat = GL_RGBA;
	#endif
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, data->cubeMapId);
	
	for (GLint i = 0 ; i < 6 ; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, internalFormat, data->settings.resolution, data->settings.resolution, 0, glFormat, glType, NULL);
	}
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	_configureCubeTextures( data->cubeMapId, true );
	_equiRectToCubeMap( data->cubeMapId, aSrcTexId, data->settings.resolution, true );
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, data->cubeMapId);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
}

//--------------------------------------------------------------
void ofCubeMap::_configureCubeTextures(GLuint aCubeMapId, bool abLinearMipLinear){
	
	GLenum textureTarget = getTextureTarget();
	glBindTexture(textureTarget, aCubeMapId );
	
	if(abLinearMipLinear) {
		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	} else {
		glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	
	glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef GL_TEXTURE_WRAP_R
	glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
#endif
	glBindTexture(getTextureTarget(), 0);
}

//--------------------------------------------------------------
void ofCubeMap::_initEmptyTextures(GLuint aCubeMapId, int aSize){
	
	GLenum textureTarget = getTextureTarget();
	glBindTexture(textureTarget, aCubeMapId );
	auto glFormat = getGLFormatFromInternalFormat();
	auto glType = getGLTypeFromInternalFormat();
	
	for (unsigned int i = 0; i < 6; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGLInternalFormat, aSize, aSize, 0, glFormat, glType, nullptr );
	}
	
	glBindTexture(getTextureTarget(), 0);
}

//--------------------------------------------------------------
void ofCubeMap::_initEmptyTextures(GLuint aCubeMapId, GLint aInternalFormat, int aSize, int aNumMipMaps ){
	GLenum textureTarget = getTextureTarget();
	glBindTexture(textureTarget, aCubeMapId );
	auto glFormat = getGLFormatFromInternalFormat(aInternalFormat);
	auto glType = getGLTypeFromInternalFormat(aInternalFormat);
	
	for (int mip = 0; mip < data->maxMipLevels; mip++) {
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth  = static_cast<unsigned int>(aSize * std::pow(0.5, mip));
		if(mipWidth < 1 ) {
			mipWidth = 1;
		}
		for (unsigned int i = 0; i < 6; i++) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, aInternalFormat, mipWidth, mipWidth, 0, glFormat, glType, nullptr );
		}
	}
	
	glBindTexture(getTextureTarget(), 0);
}

//--------------------------------------------------------------
GLuint ofCubeMap::_createFloatCubeMap(GLuint aSrcTexId, int aSrcRes){
	GLuint cubeTexF;
	glGenTextures(1, &cubeTexF );
	
	auto glFormat = getGLFormatFromInternalFormat();
	auto glType = getGLTypeFromInternalFormat();
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexF );
	
	for (GLint i = 0 ; i < 6 ; i++) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGLInternalFormat, aSrcRes, aSrcRes, 0, glFormat, glType, NULL);
	}
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	_configureCubeTextures( cubeTexF, true );
	_equiRectToCubeMap( cubeTexF, aSrcTexId, aSrcRes, false );
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubeTexF);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE ) {
		ofLogError("ofCubeMap :: _createFloatCubeMap : fbo ERROR");
	} else {
		ofLogVerbose("ofCubeMap :: _createFloatCubeMap : fbo COMPLETE");
	}
	
	return cubeTexF;
}

//--------------------------------------------------------------
void ofCubeMap::_equiRectToCubeMap( GLuint& aCubeTexId, GLuint aSrcTexId, int aSrcRes, bool aBConvertToNonFloat ) {
	bool bShaderLoaded = _loadEquiRectToCubeMapShader();
	if( !bShaderLoaded ) {
		ofLogError("ofCubeMap::_equiRectToCubeMap : error loading shader");
		return;
	}
	
	_allocateCubeMesh();
	std::vector<glm::mat4> views = _getViewMatrices( glm::vec3(0,0,0) );
	
	GLuint captureFBO;
	glGenFramebuffers(1, &captureFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, (GLuint)captureFBO );
	
	ofSetColor( 255 );
		
	ofPushView();
	ofViewport(0, 0, aSrcRes, aSrcRes, false);
	
	shaderEquiRectToCubeMap.begin();
	shaderEquiRectToCubeMap.setUniformTexture("uEquirectangularTex", GL_TEXTURE_2D, aSrcTexId, 0);
	shaderEquiRectToCubeMap.setUniformMatrix4f("uProjection", projectionMat );
	shaderEquiRectToCubeMap.setUniform1f("uFlipY", data->settings.flipVertically ? 1.0f : 0.0f );
	shaderEquiRectToCubeMap.setUniform1f("uConvertToNonFloat", aBConvertToNonFloat ? 1.0f : 0.0f );
		
	for (unsigned int i = 0; i < 6; i++) {
		shaderEquiRectToCubeMap.setUniformMatrix4f("uView", views[i]);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, aCubeTexId, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		sCubeMesh.draw();
	}
	shaderEquiRectToCubeMap.end();
	ofPopView();
	
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if( status != GL_FRAMEBUFFER_COMPLETE ) {
		ofLogError("ofCubeMap :: _equiRectToCubeMap : fbo ERROR");
	} else {
		ofLogVerbose("ofCubeMap :: _equiRectToCubeMap : fbo COMPLETE; resolution: ") << aSrcRes;
	}
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDeleteFramebuffers(1, &captureFBO);
}

//--------------------------------------------------------------
void ofCubeMap::_createIrradianceMap(GLuint aSrcCubeFid, bool aBMakeCache, const of::filesystem::path & aCachePath) {
	if(data->bIrradianceAllocated) {
		return;
	}
	if( !data->bIrradianceAllocated ) {
		data->bIrradianceAllocated = true;
		glGenTextures(1, &data->irradianceMapId );
		retain(data->irradianceMapId);
	}
	
	_allocateCubeMesh();
	
	std::vector<glm::mat4> views = _getViewMatrices( glm::vec3(0,0,0) );
	
	if( !shaderIrradianceMap.isLoaded() ) {
		auto isource = ofCubeMapShaders::irradianceCubeMap();
		shaderIrradianceMap.setupShaderFromSource(GL_VERTEX_SHADER, isource.vertShader );
		shaderIrradianceMap.setupShaderFromSource(GL_FRAGMENT_SHADER, isource.fragShader );
		shaderIrradianceMap.bindDefaults();
		shaderIrradianceMap.linkProgram();
	}
	
	if( aBMakeCache ) {
				
		ofLogVerbose("ofCubeMap :: _createIrradianceMap : making cache");
		
		ofFbo tfbo;
		//	fbo.clear();
		ofFboSettings fboSettings;
		fboSettings.width = data->settings.irradianceRes;
		fboSettings.height = data->settings.irradianceRes;
		fboSettings.numSamples = 0;
		//	fboSettings.numColorbuffers = 6;
		fboSettings.useDepth = false;
//		fboSettings.textureTarget = GL_TEXTURE_2D;
		fboSettings.internalformat = mGLInternalFormat;
		tfbo.allocate(fboSettings);
				
		ofSetColor( 255 );
		
		vector<ofFloatPixels> fpixels;
		fpixels.assign(6, ofFloatPixels());
		bool bAllPixelsCreated = true;
		
		for( unsigned int i = 0; i < 6; i++ ) {
			tfbo.begin();
			ofClear(0, 0, 0);
			shaderIrradianceMap.begin();
			shaderIrradianceMap.setUniformTexture("environmentMap", getTextureTarget(), aSrcCubeFid, 0 );
			shaderIrradianceMap.setUniformMatrix4f("uProjection", projectionMat );
			shaderIrradianceMap.setUniformMatrix4f("uView", views[i]);
			sCubeMesh.draw();
			shaderIrradianceMap.end();
			
			tfbo.end();
			tfbo.updateTexture(0);
			tfbo.readToPixels(fpixels[i]);
			if( fpixels[i].getWidth() < 1 || fpixels[i].getHeight() < 1 ) {
				bAllPixelsCreated = false;
			}
		}
		
		if(bAllPixelsCreated) {
			GLenum textureTarget = getTextureTarget();
			glBindTexture(textureTarget, data->irradianceMapId );
			auto glFormat = getGLFormatFromInternalFormat();
			auto glType = getGLTypeFromInternalFormat();
			// we need to create a single image //
			for (unsigned int i = 0; i < 6; i++) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mGLInternalFormat, fpixels[i].getWidth(), fpixels[i].getHeight(), 0, glFormat, glType, fpixels[i].getData() );
			}
			glBindTexture(textureTarget, 0 );
			
			_configureCubeTextures(data->irradianceMapId, false);
			
			// ok, now lets make a single fbo
			int fullWidth = data->settings.irradianceRes * 3;
			fboSettings.width = fullWidth;
			fboSettings.height = data->settings.irradianceRes * 2;
			
			int texSize = data->settings.irradianceRes;
			
			tfbo.clear();
			tfbo.allocate( fboSettings );
			ofSetColor(255);
			tfbo.begin(); {
				ofClear(0, 255);
				ofTexture ftex;
				for (unsigned int j = 0; j < 6; j++) {
					ftex.loadData( fpixels[j] );
					ftex.draw((j % 3) * texSize, std::floor(j / 3) * texSize, texSize, texSize);
				}
			} tfbo.end();
			
			ofFloatPixels fpix;
			tfbo.updateTexture(0);
			tfbo.readToPixels(fpix);
			if( fpix.getNumChannels() != 3 ) {
				fpix.setNumChannels(3);
			}
			if(!ofSaveImage(fpix, aCachePath)) {
				ofLogError("ofCubeMap :: _createIrradianceMap : ") << aCachePath;
			}
		}
		
		
	} else {
		
		_initEmptyTextures(data->irradianceMapId, data->settings.irradianceRes );
		_configureCubeTextures(data->irradianceMapId, false);
		
		GLuint captureFBO;
		glGenFramebuffers(1, &captureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO );

		ofPushView();
		ofViewport(0, 0, data->settings.irradianceRes, data->settings.irradianceRes, false);
		
		ofSetColor( 255 );
		shaderIrradianceMap.begin();
		shaderIrradianceMap.setUniformTexture("environmentMap", getTextureTarget(), aSrcCubeFid, 0 );
		shaderIrradianceMap.setUniformMatrix4f("uProjection", projectionMat );

		for( unsigned int i = 0; i < 6; i++ ) {
			shaderIrradianceMap.setUniformMatrix4f("uView", views[i]);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data->irradianceMapId, 0);
			ofClear(0, 0, 0, 255);
			sCubeMesh.draw();
		}

		shaderIrradianceMap.end();
		
		GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
		if( status != GL_FRAMEBUFFER_COMPLETE ) {
			ofLogError("ofCubeMap :: _createIrradianceMap : fbo ERROR");
		} else {
			ofLogVerbose("ofCubeMap :: _createIrradianceMap : fbo COMPLETE");
		}

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
		ofPopView();
	}
	
}

//--------------------------------------------------------------
bool ofCubeMap::_loadIrradianceMap(const of::filesystem::path & aCachePath) {
	
	if(data->bIrradianceAllocated) {
		return false;
	}
		
	ofLogVerbose("ofCubeMap :: _loadIrradianceMap : does file exist: ") << ofFile::doesFileExist(aCachePath);
	if( !ofFile::doesFileExist(aCachePath) ) {
		return false;
	}
	
	ofFloatPixels fullPix;
	if( !ofLoadImage( fullPix, aCachePath )) {
		ofLogError("ofCubeMap :: _loadIrradianceMap : unable to load from ") << aCachePath;
		return false;
	}
	
	if( !data->bIrradianceAllocated ) {
		data->bIrradianceAllocated = true;
		glGenTextures(1, &data->irradianceMapId );
		retain(data->irradianceMapId);
	}
		
	int texSize = fullPix.getWidth() / 3;
	
	ofFloatPixels fpix;
	size_t numChannels = getNumPixelChannels();
	GLenum textureTarget = getTextureTarget();
	
	ofLogVerbose() << "ofCubeMap :: _loadIrradianceMap : num channels: " << numChannels;
	
	glBindTexture(textureTarget, data->irradianceMapId );
	
	auto loadTexGLInternalFormat = mGLInternalFormat;
	
	auto glFormat = getGLFormatFromInternalFormat(loadTexGLInternalFormat);
	auto glType = getGLTypeFromInternalFormat(loadTexGLInternalFormat);
	
	for(unsigned int j = 0; j < 6; j++ ) {
		//cropTo(ofPixels_<PixelType> &toPix, size_t x, size_t y, size_t _width, size_t _height)
		fullPix.cropTo( fpix, (j % 3) * texSize, std::floor(j / 3) * texSize, texSize, texSize );
		if( fpix.getNumChannels() != numChannels ) {
			fpix.setNumChannels(numChannels);
		}
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, loadTexGLInternalFormat, fpix.getWidth(), fpix.getHeight(), 0, glFormat, glType, fpix.getData() );
	}
	glBindTexture(textureTarget, 0 );
	
	_configureCubeTextures(data->irradianceMapId, false);
	
	return true;
}

//--------------------------------------------------------------
void ofCubeMap::_createPrefilteredCubeMap(GLuint aSrcCubeFid, int aSrcRes, bool aBMakeCache, const of::filesystem::path & aCachePath) {
	if(data->bPreFilteredMapAllocated) {
		return;
	}
	
	_allocateCubeMesh();
	data->bPreFilteredMapAllocated = true;
	glGenTextures(1, &data->preFilteredMapId );
	retain(data->preFilteredMapId);
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, data->preFilteredMapId);
	
	auto glFormat = getGLFormatFromInternalFormat();
	auto glType = getGLTypeFromInternalFormat();
	
	// generate all of the textures and mip maps at once ...
	//glTexStorage2D(GL_TEXTURE_CUBE_MAP, data->maxMipLevels, mGLInternalFormat, data->settings.preFilterRes, data->settings.preFilterRes);
	// create all of the textures with mip maps //
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	_initEmptyTextures( data->preFilteredMapId, mGLInternalFormat, data->settings.preFilterRes, data->maxMipLevels );
	
	_configureCubeTextures(data->preFilteredMapId, true);
	
	if( !shaderPreFilterMap.isLoaded() ) {
		auto psource = ofCubeMapShaders::prefilter();
		shaderPreFilterMap.setupShaderFromSource(GL_VERTEX_SHADER, psource.vertShader );
		shaderPreFilterMap.setupShaderFromSource(GL_FRAGMENT_SHADER, psource.fragShader );
		shaderPreFilterMap.bindDefaults();
		shaderPreFilterMap.linkProgram();
	}
	
	std::vector<glm::mat4> views = _getViewMatrices( glm::vec3(0,0,0) );
	
	if( aBMakeCache ) {
		ofLogVerbose("ofCubeMap :: _createPrefilteredCubeMap : making cache");
		ofFboSettings fboSettings;
		ofFbo cacheFbo;
		fboSettings.width = data->settings.preFilterRes * 3;
		fboSettings.height = fboSettings.width;
		fboSettings.numSamples = 0;
		fboSettings.useDepth = false;
		fboSettings.internalformat = mGLInternalFormat;
		
		cacheFbo.allocate(fboSettings);
		cacheFbo.begin(); {
			ofClear(255, 0, 0);
		} cacheFbo.end();
		
		ofFbo tfbo;
		
		vector<ofFloatPixels> fpixels;
		fpixels.assign(6, ofFloatPixels() );
		// bool bAllPixelsCreated = true;
		
		int shiftX = 0;
		int shiftY = 0;
		
		for (int mip = 0; mip < data->maxMipLevels; mip++) {
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth  = static_cast<unsigned int>(data->settings.preFilterRes * std::pow(0.5, mip));
			if(mipWidth < 1 ) {
				mipWidth = 1;
			}
			fboSettings.width = mipWidth;
			fboSettings.height = fboSettings.width;
			tfbo.clear();
			tfbo.allocate(fboSettings);
			
			float roughness = (float)mip / (float)(data->maxMipLevels - 1);
			
			if( mip > 0 ) {
				shiftY = data->settings.preFilterRes * 2;
			}
			
			for (unsigned int i = 0; i < 6; ++i) {
				tfbo.begin();
				ofClear(0,255);
				shaderPreFilterMap.begin();
				shaderPreFilterMap.setUniformTexture("environmentMap", getTextureTarget(), aSrcCubeFid, 0 );
				shaderPreFilterMap.setUniformMatrix4f("uProjection", projectionMat );
				shaderPreFilterMap.setUniform1f("resolution", (float)aSrcRes );
				shaderPreFilterMap.setUniform1f("uroughness", roughness);
				shaderPreFilterMap.setUniformMatrix4f( "uView", views[i] );
				sCubeMesh.draw();
				shaderPreFilterMap.end();
				tfbo.end();
				
				tfbo.readToPixels(fpixels[i]);
				if( fpixels[i].getWidth() < 1 || fpixels[i].getHeight() < 1 ) {
					// bAllPixelsCreated = false;
				} else {
					cacheFbo.begin();
					tfbo.getTexture().draw( (i%3) * mipWidth + shiftX, std::floor(i/3) * mipWidth + shiftY, mipWidth, mipWidth );
					cacheFbo.end();
				}
			}
			
			if( mip > 0 ) {
				shiftX += mipWidth * 3;
			}
			
			glBindTexture(GL_TEXTURE_CUBE_MAP, data->preFilteredMapId);
			for (unsigned int i = 0; i < 6; ++i) {
				if( fpixels[i].getWidth() > 0 && fpixels[i].getHeight() > 0 ) {
//					// must use glTexSubImage with glTexStorage2D
					glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, 0, 0, mipWidth, mipWidth, glFormat, glType,fpixels[i].getData());

				}
			}
			glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
		}
		
		ofFloatPixels cachePix;
		cacheFbo.readToPixels(cachePix);
		if( cachePix.getWidth() > 0 ) {
			if( !ofSaveImage(cachePix, aCachePath) ) {
				ofLogError("ofCubeMap :: _createPrefilteredCubeMap: ") << aCachePath;
			}
		}
		
	} else {
		
		unsigned int captureFBO;
		glGenFramebuffers(1, &captureFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO );
		
		for (int mip = 0; mip < data->maxMipLevels; mip++) {
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth  = static_cast<unsigned int>(data->settings.preFilterRes * std::pow(0.5, mip));
			ofPushView();
			ofViewport(0, 0, mipWidth, mipWidth, false);
			shaderPreFilterMap.begin();
			shaderPreFilterMap.setUniformTexture("environmentMap", getTextureTarget(), aSrcCubeFid, 0 );
			shaderPreFilterMap.setUniformMatrix4f("uProjection", projectionMat );
			shaderPreFilterMap.setUniform1f("resolution", (float)aSrcRes );
			
			float roughness = (float)mip / (float)(data->maxMipLevels - 1);
			shaderPreFilterMap.setUniform1f("uroughness", roughness);
			for (unsigned int i = 0; i < 6; ++i) {
				shaderPreFilterMap.setUniformMatrix4f( "uView", views[i] );
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, data->preFilteredMapId, mip);
				ofClear(0, 0, 0);
				sCubeMesh.draw();
			}
			shaderPreFilterMap.end();
			ofPopView();
		}
		
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDeleteFramebuffers(1, &captureFBO);
	}
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

//--------------------------------------------------------------
bool ofCubeMap::_loadPrefilterMap( const of::filesystem::path & aCachePath ) {
	if(data->bPreFilteredMapAllocated) {
		return false;
	}
	
	ofLogVerbose("ofCubeMap :: _loadPrefilterMap : does file exist: ") << ofFile::doesFileExist(aCachePath);
	if( !ofFile::doesFileExist(aCachePath) ) {
		return false;
	}
	
	ofFloatPixels fullPix;
	if( !ofLoadImage( fullPix, aCachePath )) {
		ofLogError("ofCubeMap :: _loadPrefilterMap : unable to load from ") << aCachePath;
		return false;
	}
	
	_allocateCubeMesh();
	data->bPreFilteredMapAllocated = true;
	glGenTextures(1, &data->preFilteredMapId );
	retain(data->preFilteredMapId);
	
	
	auto loadTexGLInternalFormat = mGLInternalFormat;
	auto glFormat = getGLFormatFromInternalFormat(loadTexGLInternalFormat);
	auto glType = getGLTypeFromInternalFormat(loadTexGLInternalFormat);
	
	_initEmptyTextures( data->preFilteredMapId, loadTexGLInternalFormat, data->settings.preFilterRes, data->maxMipLevels );
	
	//glBindTexture(GL_TEXTURE_CUBE_MAP, data->preFilteredMapId);
	//glTexStorage2D(GL_TEXTURE_CUBE_MAP, data->maxMipLevels, loadTexFormat, data->settings.preFilterRes, data->settings.preFilterRes);
	//glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	_configureCubeTextures(data->preFilteredMapId, true);
		
	ofFloatPixels fpix;
	size_t numChannels = getNumPixelChannels();
	
	float shiftX = 0.0f;
	float shiftY = 0.0f;
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, data->preFilteredMapId );
	for (int mip = 0; mip < data->maxMipLevels; mip++) {
		// reisze framebuffer according to mip-level size.
		unsigned int mipWidth  = static_cast<unsigned int>(data->settings.preFilterRes * std::pow(0.5, mip));
		if(mipWidth < 1 ) {mipWidth = 1;}
		
		if( mip > 0 ) {
			shiftY = data->settings.preFilterRes * 2;
		}
		
		for (unsigned int i = 0; i < 6; ++i) {
			fullPix.cropTo( fpix, (i % 3) * mipWidth + shiftX, std::floor(i / 3) * mipWidth + shiftY, mipWidth, mipWidth );
			if( fpix.getNumChannels() != numChannels ) {
				fpix.setNumChannels(numChannels);
			}
			glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, mip, 0, 0, mipWidth, mipWidth, glFormat, glType, fpix.getData());
		}
		
		if( mip > 0 ) {
			shiftX += mipWidth * 3;
		}
	}
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	return true;
}

//--------------------------------------------------------------
void ofCubeMap::_createBrdfLUT() {
	#ifndef TARGET_OPENGLES
	int lutWidth = 512;
	int lutHeight = 512;
	
	ofFbo lutFbo;
	
	if( !shaderBrdfLUT.isLoaded() ) {
		auto bsource = ofCubeMapShaders::brdfLUT();
		shaderBrdfLUT.setupShaderFromSource(GL_VERTEX_SHADER, bsource.vertShader );
		shaderBrdfLUT.setupShaderFromSource(GL_FRAGMENT_SHADER, bsource.fragShader );
		shaderBrdfLUT.bindDefaults();
		shaderBrdfLUT.linkProgram();
	}
	
	ofMesh quadMesh;
	quadMesh.setMode(OF_PRIMITIVE_TRIANGLES);
	quadMesh.addVertices({
		glm::vec3(0,0,0),
		glm::vec3(lutWidth, 0.0, 0.0),
		glm::vec3(lutWidth, lutHeight, 0.0),
		glm::vec3(0, lutHeight, 0.0f)
		
	});
	
	quadMesh.addTexCoords( {
		glm::vec2(0.0f, 1.0f),
		glm::vec2(1.0f, 1.0f),
		glm::vec2(1.0f, 0.0f),
		glm::vec2(0.0f, 0.0f)
	});
	
	quadMesh.addIndices({
		0, 1, 3,
		1, 2, 3
	});
	quadMesh.disableColors();
	quadMesh.disableNormals();
	lutFbo.allocate(lutWidth, lutHeight, GL_RG32F );
	
	ofSetColor(255);
	ofPushView();
	lutFbo.begin();
	ofClear(0, 0, 0);
	shaderBrdfLUT.begin();
	quadMesh.draw();
	ofSetColor(255);
	shaderBrdfLUT.end();
	lutFbo.end();
	ofPopView();
	
	lutFbo.updateTexture(0);
	sBrdfLutTex = lutFbo.getTexture(0);
	lutFbo.clear();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	#else
	ofLogWarning("ofCubeMap::_createBrdfLUT") << " brdf lut texture not supported on GLES";
	#endif
}

//--------------------------------------------------------------
void ofCubeMap::_allocateCubeMesh() {
	if( sCubeMesh.getNumVertices() > 0 ) {
		return;
	}
	sCubeMesh = ofMesh::box( 1, 1, 1, 4, 4, 4);
	sCubeMesh.disableColors();
	sCubeMesh.disableTextures();
	sCubeMesh.disableNormals();
	sCubeMesh.getTexCoords().clear();
}

//--------------------------------------------------------------
std::vector<glm::mat4> ofCubeMap::_getViewMatrices(const glm::vec3& apos ) {
	// eye, target and up vector
	// +x, -x, +y, -y, +z and -z direction
	vector<glm::mat4> views = {
		glm::lookAt( apos, apos+glm::vec3(1,0,0), glm::vec3(0, -1, 0) ),
		glm::lookAt( apos, apos+glm::vec3(-1,0,0), glm::vec3(0, -1, 0) ),
		glm::lookAt( apos, apos+glm::vec3(0,1,0), glm::vec3(0, 0, 1) ),
		glm::lookAt( apos, apos+glm::vec3(0,-1,0), glm::vec3(0, 0, -1) ),
		glm::lookAt( apos, apos+glm::vec3(0,0,1), glm::vec3(0, -1, 0) ),
		glm::lookAt( apos, apos+glm::vec3(0,0,-1), glm::vec3(0, -1, 0) )
	};
	return views;
}

//--------------------------------------------------------------
GLenum ofCubeMap::getGLFormatFromInternalFormat() {
	return getGLFormatFromInternalFormat(mGLInternalFormat);
}

//--------------------------------------------------------------
GLenum ofCubeMap::getGLFormatFromInternalFormat(GLint aInternalFormat) {
#if defined(GL_RGBA32F)
	if( aInternalFormat == GL_RGBA32F ) {
		return GL_RGBA;
	}
#endif
#if defined(GL_RGBA16F)
	if( aInternalFormat == GL_RGBA16F ) {
		return GL_RGBA;
	}
#endif
	
#ifdef GL_RGBA32F_EXT
	if( aInternalFormat == GL_RGBA32F_EXT ) {
		return GL_RGBA;
	}
#endif
	return GL_RGB;
}

//--------------------------------------------------------------
GLenum ofCubeMap::getGLTypeFromInternalFormat(){
	return getGLTypeFromInternalFormat(mGLInternalFormat);
}

//--------------------------------------------------------------
GLenum ofCubeMap::getGLTypeFromInternalFormat(GLint aInternalFormat){
	#if defined(GL_RGB32F)
	if( aInternalFormat == GL_RGB32F ) {
		return GL_FLOAT;
	}
	#endif
	#if defined(GL_RGBA32F)
	if( aInternalFormat == GL_RGBA32F ) {
		return GL_FLOAT;
	}
	#endif
	#if defined(GL_RGBA16F)
	if( aInternalFormat == GL_RGBA16F ) {
		return GL_FLOAT;
	}
	#endif
	#if defined(GL_RGB16F)
	if( aInternalFormat == GL_RGB16F ) {
		return GL_FLOAT;
	}
	#endif
	
	#ifdef GL_RGBA32F_EXT
	if( aInternalFormat == GL_RGBA32F_EXT ) {
		return GL_FLOAT;
	}
	#endif
	#ifdef GL_RGB32F_EXT
	if( aInternalFormat == GL_RGB32F_EXT ) {
		return GL_FLOAT;
	}
	#endif
	
	return GL_UNSIGNED_BYTE;
}

//--------------------------------------------------------------
int ofCubeMap::getNumPixelChannels(){
	GLenum glType = getGLFormatFromInternalFormat();
	if( glType == GL_RGBA ) {
		return 4;
	}
	return 3;
}

//--------------------------------------------------------------
bool ofCubeMap::_loadRenderShader() {
	shaderRender.unload();
	
	auto rsource = ofCubeMapShaders::renderShader();
	shaderRender.setupShaderFromSource(GL_VERTEX_SHADER, rsource.vertShader );
	shaderRender.setupShaderFromSource(GL_FRAGMENT_SHADER, rsource.fragShader );
	shaderRender.bindDefaults();
	return shaderRender.linkProgram();
}

//--------------------------------------------------------------
bool ofCubeMap::_loadEquiRectToCubeMapShader() {
	if( !shaderEquiRectToCubeMap.isLoaded() ) {
		auto esource = ofCubeMapShaders::equiRectToCubeMap();
		shaderEquiRectToCubeMap.setupShaderFromSource(GL_VERTEX_SHADER, esource.vertShader );
		shaderEquiRectToCubeMap.setupShaderFromSource(GL_FRAGMENT_SHADER, esource.fragShader );
		shaderEquiRectToCubeMap.bindDefaults();
		if(shaderEquiRectToCubeMap.linkProgram()) {
			return true;
		} else {
			shaderEquiRectToCubeMap.unload();
			ofLogError("ofCubeMap::_loadEquiRectToCubeMapShader : unable to create shaderEquiRectToCubeMap shader ");
			return false;
		}
	}
	return true;
}

