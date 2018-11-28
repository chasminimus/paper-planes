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
		void apply_force(ofVec3f force, float scale=1.0);
	};

	ofVec3f separate(int index);
	ofVec3f bound(int index);
	
	ofConePrimitive cone;
	ofxAssimpModelLoader model;

public: 
	Flock();
	void init(int n_planes);
	void customDraw(); // overridden virtual method
	ofLight light;
	//ofMaterial material;
	static float desired_separation;
	static float max_speed;
	static float max_force;

protected:
	void update();
	vector<paper_plane> planes;
};