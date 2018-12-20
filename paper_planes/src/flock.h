#pragma once
#include "ofMain.h"
#include "ofxAssimpModelLoader.h"
#include "mesh_node.h"

const int MAX_RADIUS = 40;
const int POSITION_DISPERSION = MAX_RADIUS * 2;
const int VELOCITY_DISPERSION = 5;

// how many subdivisions to make (must be able to divide MAX_RADIUS*2)
// - better if even
// - set to 1 to effectively disable subdividing
const int LATTICE_SUBDIVS = 6;
// the actual size of a cell in pixels (or whatever units this thing uses)
const float LATTICE_GRID_SIZE = (MAX_RADIUS * 2) / LATTICE_SUBDIVS;

// used instead of creating a bunch of empty vectors all the time
const ofVec3f ZERO_VECTOR; 
// used to offset positions for bin calculations
const ofVec3f RADIUS_VECTOR(MAX_RADIUS); 

// fixed-size 3d grid type (sorry)
template<typename T> using Lattice = array<array<array<T, LATTICE_SUBDIVS>, LATTICE_SUBDIVS>, LATTICE_SUBDIVS>;

class Flock : public ofNode {
	// plane
	class paper_plane {
		float max_speed_ = 5.0f;
		float max_force_ = 1.0f;
	public:
		ofVec3f position;
		ofVec3f velocity;
		ofVec3f acceleration;
		void applyForce(ofVec3f force, float scale = 1.0);
		virtual float get_max_speed() { return max_speed_; }; // epic polymorphism style
		virtual float get_max_force() { return max_force_; };
	};

	// predator, a specific kind of plane
	class predator : public paper_plane {
		float max_speed_ = 8.0f;
		float max_force_ = 2.0f;
	public:
		float get_max_speed() { return max_speed_; };
		float get_max_force() { return max_force_; };
	};

	// force methods
	ofVec3f separate(paper_plane* plane, vector<paper_plane*> &cell);
	ofVec3f align(paper_plane* plane, vector<paper_plane*> &cell);
	ofVec3f cohere(paper_plane* plane, vector<paper_plane*> &cell);
	ofVec3f bound(paper_plane* plane);
	ofVec3f seek(paper_plane* plane, ofVec3f target);
	ofVec3f repel(paper_plane* plane, ofVec3f obstable, float radius);
	void wrap(paper_plane* plane);

	// 3d grid of pointer lists so we can get the actual specific instances of paper_planes to use
	Lattice<vector<paper_plane*>> bins;

	// bin-lattice spacial subdivision methods
	// indexes planes into bins
	void binRegister(paper_plane* plane);
	// returns the list of planes in neighboring cells
	vector<paper_plane*> aggregrateNeighborCells(int i, int j, int k);

	// drawing stuff
	void draw_velocity(paper_plane* plane);
	void draw_plane(paper_plane* plane);
	ofxMeshNode plane_model;
	ofLight light;

public: 
	Flock();
	~Flock();

	// populate the flock with the number of planes and predators
	void init(int n_planes, int n_predators);
	void customDraw(); // overridden virtual method

	// bound control variables
	static float desired_separation; // planes within this distance will invoke the force
	static float neighbor_search_radius; // planes within this radius are used in calculation
	static float sim_speed; // multiplied with the timestep to increase the speed of everything
	static bool wraparound; // whether or not to use a sphere or allow position looping

	// bound weight variables
	static float separation_weight;
	static float alignment_weight;
	static float cohesion_weight;
	static float bounding_weight;
	static float flee_weight;

protected:
	void update();
	vector<paper_plane*> planes;
	vector<paper_plane*> predators;
};