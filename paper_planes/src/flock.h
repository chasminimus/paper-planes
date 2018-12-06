#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"

const int MAX_RADIUS = 50;
const int POSITION_DISPERSION = MAX_RADIUS * 2;
const int VELOCITY_DISPERSION = 5;

// how many subdivisions to make (must be able to divide MAX_RADIUS*2
// better if even
const int LATTICE_SUBDIVS = 4;
// the actual size of a cell in pixels (or whatever units this thing uses)
const float LATTICE_GRID_SIZE = (MAX_RADIUS * 2) / LATTICE_SUBDIVS;

const ofVec3f ZERO_VECTOR;
const ofVec3f RADIUS_VECTOR(MAX_RADIUS); // used to offset positions for bin calculations

// 3d grid type (sorry)
template<typename T> using Lattice = array<array<array<T, LATTICE_SUBDIVS>, LATTICE_SUBDIVS>, LATTICE_SUBDIVS>;

class Flock : public ofNode {
	struct paper_plane {
		ofVec3f position;
		ofVec3f velocity;
		ofVec3f acceleration;
		void applyForce(ofVec3f force, float scale=1.0);
	};

	ofVec3f separate(paper_plane* plane, vector<paper_plane*> &cell);
	ofVec3f align(paper_plane* plane, vector<paper_plane*> &cell);
	ofVec3f cohere(paper_plane* plane, vector<paper_plane*> &cell);
	ofVec3f bound(int index);
	void wrap(int index);
	ofVec3f seek(paper_plane* plane, ofVec3f target);
	
	// this unholy type represents the 3d lattice of bins in which
	// paper_planes exist in. it's a 3d grid of pointers so we can
	// get the actual specific instances of paper_planes to use
	// each cell in the lattice has a list of paper_plane pointers
	Lattice<vector<paper_plane*>> bins;

	void binRegister(int index);

	vector<paper_plane*> aggregrateNeighborCells(int i, int j, int k);

	int n_planes_;

	ofConePrimitive cone;
	ofxAssimpModelLoader model;

public: 
	Flock();
	void init(int n_planes);
	void customDraw(); // overridden virtual method
	ofLight light;

	static float desired_separation;
	static float max_speed;
	static float max_force;
	static float neighbor_search_radius;
	static float sim_speed;
	static bool wraparound;

	static float separation_weight;
	static float alignment_weight;
	static float cohesion_weight;
	static float bounding_weight;

protected:
	void update();
	vector<paper_plane> planes;
};