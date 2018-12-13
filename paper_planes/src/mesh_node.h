#pragma once
#include <ofMain.h>

// a subclass of ofNode that contains a mesh for drawing
class meshNode : public ofNode {
	ofMesh mesh_;
public:
	meshNode();
	meshNode(ofMesh mesh);
	virtual void customDraw();
};