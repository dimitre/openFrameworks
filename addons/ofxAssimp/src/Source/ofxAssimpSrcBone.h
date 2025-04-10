//
//  ofxAssimpBone.h
//  Created by Nick Hardeman on 10/19/23.
//

#pragma once
#include "ofxAssimpSrcNode.h"

namespace ofxAssimp {
class SrcBone : public ofxAssimp::SrcNode {
public:
	NodeType getType() override { return OFX_ASSIMP_BONE; }
	
	void setAiBone(aiBone* aAiBone, aiNode* aAiNode);
//	void update();
	
	std::shared_ptr<ofxAssimp::SrcBone> getBone( aiNode* aAiNode );
	void findBoneRecursive( aiNode* aAiNode, std::shared_ptr<SrcBone>& returnBone );
	
	std::string getAsString( unsigned int aLevel=0 ) override;
	
	unsigned int getNumBones();
	
	aiBone* getAiBone() { return mAiBone; }
	
	bool bRoot = false;
	
	aiMatrix4x4& getAiMatrix() { return mAiMatrix; }
//	aiMatrix4x4& getAiMatrixGlobal() { return mAiMatrixGlobal; }
	aiMatrix4x4& getAiOffsetMatrix() { return mOffsetMatrix;}
	
	std::vector< std::shared_ptr<ofxAssimp::SrcBone> > childBones;
	
protected:
	aiBone* mAiBone = nullptr;
	aiMatrix4x4 mAiMatrix;
//	aiMatrix4x4 mAiMatrixGlobal;
	aiMatrix4x4 mOffsetMatrix;
	glm::mat4 mBoneLocalTransform = glm::mat4(1.0);
};
}
