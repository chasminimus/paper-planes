#pragma once
#include <ofMain.h>

// a subclass of ofNode that contains a mesh for drawing
class ofxMeshNode : public ofNode {
	ofMesh mesh_;
public:
	ofxMeshNode();
	ofxMeshNode(ofMesh mesh);
	virtual void customDraw();
};