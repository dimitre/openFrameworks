/*
* ofxCvContourFinder.h
*
* Finds white blobs in binary images and identifies
* centroid, bounding box, area, length and polygonal contour
* The result is placed in a vector of ofxCvBlob objects.
*
*/

#pragma once


#include "ofxCvConstants.h"
#include "ofxCvBlob.h"
#include "ofxCvGrayscaleImage.h"
#include <algorithm>

class ofxCvContourFinder : public ofBaseDraws {
public:

	std::vector<ofxCvBlob>  blobs;
	int nBlobs;    // DEPRECATED: use blobs.size() instead
		

	ofxCvContourFinder();
	virtual  ~ofxCvContourFinder();
	
	virtual float getWidth() const override { return _width; };    //set after first findContours call
	virtual float getHeight() const override { return _height; };  //set after first findContours call
		
	virtual int  findContours( ofxCvGrayscaleImage& input,
								int minArea, int maxArea,
								int nConsidered, bool bFindHoles,
								bool bUseApproximation = true);
								// approximation = don't do points for all points 
								// of the contour, if the contour runs
								// along a straight line, for example...

	virtual void draw() const { draw(0,0, _width, _height); };
	virtual void draw( float x, float y ) const override { draw(x,y, _width, _height); };
	virtual void draw( float x, float y, float w, float h ) const override;
	virtual void draw(const glm::vec2 & point) const override;
	virtual void draw(const ofRectangle & rect) const override;
	virtual void setAnchorPercent(float xPct, float yPct) override;
	virtual void setAnchorPoint(float x, float y) override;
	virtual void resetAnchor() override ;
		//virtual ofxCvBlob  getBlob(int num);



	protected:

		int  _width;
		int  _height;
		ofxCvGrayscaleImage     inputCopy;
		CvMemStorage*           contour_storage;
		CvMemStorage*           storage;
		CvMoments*              myMoments;
		std::vector<CvSeq*>     cvSeqBlobs;  //these will become blobs
		
		glm::vec2 anchor;
		bool  bAnchorIsPct;      

		virtual void reset();

};
