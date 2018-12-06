#include "flock.h"

Flock::Flock() {
	light.setAmbientColor(ofColor(0, 0, 0));
	cone.set(.5f, 1.0f);
	//cone.tiltDeg(90.0f);
	//cone.rollDeg(180.0f);
	//cone.panDeg(90.0f);
}

//flock system variables
float Flock::desired_separation = 5;
float Flock::max_speed = 10;
float Flock::max_force = 0.5;
float Flock::neighbor_search_radius = 10;
float Flock::sim_speed = 5;
bool Flock::wraparound = false;

//flock rule weights
float Flock::separation_weight = 1.5;
float Flock::alignment_weight = 1.0;
float Flock::cohesion_weight = 1.0;
float Flock::bounding_weight = 1.15;

void Flock::init(int n_planes) {
	n_planes_ = n_planes;
	if (planes.size() != 0) {
		ofLogWarning("Flock") << "Flock already initialized!?";
		planes.clear();
	}

	ofSeedRandom();
	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration;

	for (int i = 0; i < n_planes; i++) {
		
		position.x = (ofRandom(1.0f) - 0.5f) * POSITION_DISPERSION;
		position.y = (ofRandom(1.0f) - 0.5f) * POSITION_DISPERSION;
		position.z = (ofRandom(1.0f) - 0.5f) * POSITION_DISPERSION;
		
		velocity.x = (ofRandom(1.0f) - 0.5f) * VELOCITY_DISPERSION;
		velocity.y = (ofRandom(1.0f) - 0.5f) * VELOCITY_DISPERSION;
		velocity.z = (ofRandom(1.0f) - 0.5f) * VELOCITY_DISPERSION;
		
		paper_plane plane;
		plane.position = position;
		plane.velocity = velocity;
		plane.acceleration = acceleration;

		planes.push_back(plane);
	}
}

void Flock::customDraw() {
	// manually update
	update();

	// drawing paper planes
	ofPushStyle();
	light.enable();
	light.setPosition(ofVec3f(0, 0, 0));

	ofVec3f arrowTail, arrowHead;
	for (unsigned int i = 0; i < planes.size(); i++) {
		arrowTail = planes[i].position;
		arrowHead = arrowTail + planes[i].velocity.normalize();
		
		ofDrawArrow(arrowTail, arrowHead, 0.0f);
		cone.setPosition(planes[i].position);
		cone.lookAt(planes[i].position + planes[i].velocity.normalize());

		cone.draw();
	}

	light.disable();
	ofDisableLighting();
	ofPopStyle();
}

void Flock::update() {
	// time differential used for updating vectors
	ofVec3f bounding;
	ofVec3f separation;
	ofVec3f alignment;
	ofVec3f cohesion;
	float dt = ofGetLastFrameTime() * sim_speed;
	
	// get in deep into the lattice and clear out the bin lists
	for (int i = 0; i < LATTICE_SUBDIVS; i++) {
		for (int j = 0; j < LATTICE_SUBDIVS; j++) {
			for (int k = 0; k < LATTICE_SUBDIVS; k++) {
				bins[i][j][k].clear();
			}
		}
	}

	for (int i = 0; i < planes.size(); i++) {
		// put each paper_plane in the correct bin
		binRegister(i);
	}

	// for each cell...
	for (int i = 0; i < LATTICE_SUBDIVS; i++) {
		for (int j = 0; j < LATTICE_SUBDIVS; j++) {
			for (int k = 0; k < LATTICE_SUBDIVS; k++) {
				vector<paper_plane*> cell = bins[i][j][k]; // the list of planes in the cell
				for (paper_plane* plane : cell) {
					separation = separate(plane, cell);
					alignment = align(plane, cell);
					cohesion = cohere(plane, cell);

					plane->applyForce(separation, separation_weight);
					plane->applyForce(alignment, alignment_weight);
					plane->applyForce(cohesion, cohesion_weight);
				}
			}
		}
	}

	for (unsigned int i = 0; i < planes.size(); i++) {
		if (wraparound) {
			wrap(i);
		}
		else {
			bounding = bound(i);
			planes[i].applyForce(bounding, bounding_weight);
		}
		// ok so basically numerically integrate
		// a = dv / dt
		// dv = a * dt
		// v_new = v_old + dv
		planes[i].velocity += planes[i].acceleration * dt;
		// cap the speed so they don't get outta control
		planes[i].velocity.limit(max_speed);
		planes[i].position += planes[i].velocity * dt;
		// reset the acceleration each frame
		planes[i].acceleration *= 0;
	}
}

void Flock::paper_plane::applyForce(ofVec3f force, float scale) {
	acceleration += force.limit(max_force).scale(scale);
}

void Flock::binRegister(int index) {
	paper_plane* this_plane = &planes[index];
	// shift the positions to all be positive to make bin calculation easier
	// e.g. with a radius of 50, something at -50 will become 0 and something
	// at 50 will become 100
	ofVec3f offset_position = this_plane->position + RADIUS_VECTOR;
	
	int x = (int)offset_position.x / LATTICE_GRID_SIZE;
	int y = (int)offset_position.y / LATTICE_GRID_SIZE;
	int z = (int)offset_position.z / LATTICE_GRID_SIZE;
	
	// if the plane is at the very farthest edge, it'd technically be part of the next cell (beyond
	// the grid) so we make sure it stays in bounds
	// e.g. if it was at (100, 100, 100) and the grid size is 10 it'd be placed at [10][10][10]
	// but the lattice goes from 0 to 9, so it needs to be in the 9th cell
	if (x == LATTICE_SUBDIVS) { x--; }
	if (y == LATTICE_SUBDIVS) { y--; }
	if (z == LATTICE_SUBDIVS) { z--; }
	// same idea but in the negative direction (just in case)
	if (x == -1) { x++; }
	if (y == -1) { y++; }
	if (z == -1) { z++; }
	
	// another sanity check
	if (x >= 0 && x < LATTICE_SUBDIVS && y >= 0 && y < LATTICE_SUBDIVS && z >= 0 && z < LATTICE_SUBDIVS) {
		bins[x][y][z].push_back(this_plane);
		//cout << "Plane added to bin (" << x << " " << y << " " << z << ")" << endl;
	}
}

ofVec3f Flock::separate(paper_plane* plane, vector<paper_plane*> &cell) {
	ofVec3f steer, diff;
	int count = 0;

	for (paper_plane* other : cell) {
		float distance = plane->position.distance(other->position);
		//check if too close to other boid
		if (distance > 0 && distance < desired_separation) {
			//create vector pointing away from it
			diff = ofVec3f(plane->position - other->position);
			diff.normalize();
			diff /= distance;
			steer += diff;
			count++;
		}
	}

	if (count > 0) {
		steer /= count;
	}
	//steering = desired - velocity
	if (steer.lengthSquared() > 0) {
		steer.scale(max_speed);
		steer - plane->velocity;
	}
	return steer;
}

ofVec3f Flock::align(paper_plane* plane, vector<paper_plane*> &cell) {
	int count = 0;
	ofVec3f velocity_sum;
	for (paper_plane* other : cell) {
		float distance = plane->position.distance(other->position);
		if (distance > 0 && distance < neighbor_search_radius) {
			velocity_sum += other->velocity;
			count++;
		}
	}
	if (count > 0) {
		velocity_sum /= count;
		velocity_sum.scale(max_speed);
		return velocity_sum - plane->velocity;
	}
	else {
		return ZERO_VECTOR;
	}
}

ofVec3f Flock::cohere(paper_plane* plane, vector<paper_plane*> &cell) {
	ofVec3f position_sum;
	int count = 0;
	for (paper_plane* other : cell) {
		float distance = plane->position.distance(other->position);
		if (distance > 0 && distance < neighbor_search_radius) {
			position_sum += plane->position;
			count++;
		}
	}
	if (count > 0) {
		position_sum /= count; // center of mass of the flock
		return seek(plane, position_sum);
	}
	else {
		return ZERO_VECTOR;
	}
}

ofVec3f Flock::bound(int index) {
	paper_plane this_plane = planes[index];
	ofVec3f steer;
	if (this_plane.position.length() >= MAX_RADIUS) {
		//create vector pointing to origin
		steer = -this_plane.position / MAX_RADIUS;
	}
	return steer;
}

void Flock::wrap(int index) {
	paper_plane* plane = &planes[index];
	// "why don't you just flip the position vector?"
	// well, then they'd get stuck outside and constantly zap back and forth
	// there's probably a fix for that but this isn't that horrible
	if (plane->position.x < -MAX_RADIUS) {
		plane->position.x = MAX_RADIUS;
	}
	if (plane->position.x > MAX_RADIUS) {
		plane->position.x = -MAX_RADIUS;
	}
	if (plane->position.y < -MAX_RADIUS) {
		plane->position.y = MAX_RADIUS;
	}
	if (plane->position.y > MAX_RADIUS) {
		plane->position.y = -MAX_RADIUS;
	}
	if (plane->position.z < -MAX_RADIUS) {
		plane->position.z = MAX_RADIUS;
	}
	if (plane->position.z > MAX_RADIUS) {
		plane->position.z = -MAX_RADIUS;
	}
}

ofVec3f Flock::seek(paper_plane* plane, ofVec3f target) {
	ofVec3f desired = target - plane->position;
	desired.scale(max_speed);
	ofVec3f steer = desired - plane->velocity;
	return steer;
}