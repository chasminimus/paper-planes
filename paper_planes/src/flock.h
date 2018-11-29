#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

const int MAX_RADIUS = 40;
const int POSITION_DISPERSION = 70;
const int VELOCITY_DISPERSION = 1;
const ofVec3f ORIGIN;

class Flock : public ofNode {
	struct paper_plane {
		ofVec3f position;
		ofVec3f velocity;
		ofVec3f acceleration;
		void apply_force(ofVec3f force, float scale=1.0);
	};

	ofVec3f separate(int index);
	ofVec3f align(int index);
	ofVec3f cohere(int index);
	ofVec3f bound(int index);
	void wrap(int index);
	ofVec3f seek(int index, ofVec3f target);
	
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
	static float neighbor_search_radius;

protected:
	void update();
	vector<paper_plane> planes;
};