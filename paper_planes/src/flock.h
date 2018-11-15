#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

const int MAX_RADIUS = 20;
const int POSITION_DISPERSION = 10;
const int VELOCITY_DISPERSION = 5;

class Flock : public ofNode {
	struct paper_plane {
		ofVec3f position;
		ofVec3f velocity;
		ofVec3f acceleration;
	};
	
	ofConePrimitive cone;

	ofxAssimpModelLoader model;

public: 
	Flock();
	void init(int n_planes);
	void customDraw(); // overridden virtual method
	ofLight light;
	//ofMaterial material;

protected:
	void update();
	vector<paper_plane> planes;
};