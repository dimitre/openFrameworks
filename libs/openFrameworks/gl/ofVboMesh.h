#pragma once

#include "ofMesh.h"
// MARK: vbo object only;
#include "ofVbo.h"

class ofVboMesh: public ofMesh{
public:
	using ofMesh::draw;
	ofVboMesh();
	ofVboMesh(const ofMesh & mom);
    void operator=(const ofMesh & mom);
	~ofVboMesh() override;
	void setUsage(int usage);

    void enableColors() override;
    void enableTextures() override;
    void enableNormals() override;
    void enableIndices() override;

    void disableColors() override;
    void disableTextures() override;
    void disableNormals() override;
    void disableIndices() override;

    bool usingColors() const noexcept override;
    bool usingTextures() const noexcept override;
    bool usingNormals() const noexcept override;
    bool usingIndices() const noexcept override;

	void draw(ofPolyRenderMode drawMode) const override;
	void drawInstanced(ofPolyRenderMode drawMode, int primCount) const;
	
	ofVbo & getVbo();
	const ofVbo & getVbo() const;
	
private:
	void updateVbo();
	void unloadVbo();
	ofVbo vbo;
	int usage;
	std::size_t vboNumVerts;
	std::size_t vboNumIndices;
	std::size_t vboNumNormals;
	std::size_t vboNumTexCoords;
	std::size_t vboNumColors;
};
