#include "flock.h"

Flock::Flock() {
	light.setAmbientColor(ofColor(0, 0, 0));
	cone.set(.5f, 1.0f);
	//cone.tiltDeg(90.0f);
	//cone.rollDeg(180.0f);
	//cone.panDeg(90.0f);
}

Flock::~Flock() {
	for (paper_plane* plane : planes) {
		delete plane;
	}
	for (paper_plane* pred : predators) {
		delete pred;
	}
}

//flock system variables
float Flock::desired_separation = 5;
float Flock::neighbor_search_radius = 10; //TODO: this might be irrelevant with binning now
float Flock::sim_speed = 4;
bool Flock::wraparound = false;

//flock rule weights
float Flock::separation_weight = 1.5;
float Flock::alignment_weight = 1.2;
float Flock::cohesion_weight = 1.0;
float Flock::bounding_weight = 1.5;
float Flock::flee_weight = 1.5;

void Flock::init(int n_planes, int n_predators) {
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
		
		paper_plane* plane = new paper_plane();

		plane->position = position;
		plane->velocity = velocity;
		plane->acceleration = acceleration;

		planes.push_back(plane);

		if (i > n_planes - n_predators) {
			predators.push_back(new predator());
		}
	}
}

void Flock::customDraw() {
	// manually update
	update();

	// drawing paper planes
	ofPushStyle();
	light.enable();
	light.setPosition(ZERO_VECTOR);

	for (paper_plane* plane : planes) {
		draw_velocity(plane);
		cone.setPosition(plane->position);
		cone.lookAt(plane->position + plane->velocity.getNormalized());

		ofSetColor(255.0f, 255.0f, 255.0f);
		cone.draw();
	}

	for (paper_plane* pred : predators) {
		draw_velocity(pred);
		cone.setPosition(pred->position);
		cone.lookAt(pred->position + pred->velocity.getNormalized());

		ofSetColor(255.0f, 0.0f, 0.0f);
		cone.draw();
	}

	light.disable();
	ofDisableLighting();
	ofPopStyle();
}

void Flock::draw_velocity(paper_plane* plane) {
	ofVec3f arrowTail = plane->position;
	ofVec3f arrowHead = arrowTail + plane->velocity.getNormalized();

	ofDrawArrow(arrowTail, arrowHead, 0.0f);
}

// TODO: extract the update loops into methods belonging to paper_plane and predator
// however, this requires moving the force rules into those classes too
void Flock::update() {
	// force vectors
	ofVec3f bounding;
	ofVec3f separation;
	ofVec3f alignment;
	ofVec3f cohesion;
	ofVec3f flee;

	// time differential used for updating vectors
	float dt = ofGetLastFrameTime() * sim_speed;

	// list of planes in neighbor cells
	vector<paper_plane*> cell_aggregate;
	
	// get in deep into the lattice and clear out the bin lists
	for (int i = 0; i < LATTICE_SUBDIVS; i++) {
		for (int j = 0; j < LATTICE_SUBDIVS; j++) {
			for (int k = 0; k < LATTICE_SUBDIVS; k++) {
				bins[i][j][k].clear();
			}
		}
	}

	// put each paper_plane in the correct bin
	for (paper_plane* plane : planes) {
		binRegister(plane);
	}

	// for each cell...
	for (int i = 0; i < LATTICE_SUBDIVS; i++) {
		for (int j = 0; j < LATTICE_SUBDIVS; j++) {
			for (int k = 0; k < LATTICE_SUBDIVS; k++) {
				vector<paper_plane*> cell = bins[i][j][k]; // the list of planes in the cell
				cell_aggregate = aggregrateNeighborCells(i, j, k);
				//TODO: aggregate the lists of neighboring cells too?
				for (paper_plane* plane : cell) {
					separation = separate(plane, cell_aggregate);
					alignment = align(plane, cell_aggregate);
					cohesion = cohere(plane, cell_aggregate);

					for (paper_plane* pred : predators) {
						flee = repel(plane, pred->position, neighbor_search_radius);
						plane->applyForce(flee, flee_weight);
					}

					plane->applyForce(separation, separation_weight);
					plane->applyForce(alignment, alignment_weight);
					plane->applyForce(cohesion, cohesion_weight);
				}
			}
		}
	}

	// for all the planes (this has to be done separately from cells since planes sometimes go out of bounds if not wrapping around
	for (paper_plane* plane : planes) {
		if (wraparound) {
			wrap(plane);
		}
		else {
			bounding = bound(plane);
			plane->applyForce(bounding, bounding_weight);
		}

		// ok so basically numerically integrate
		// a = dv / dt
		// dv = a * dt
		// v_new = v_old + dv
		plane->velocity += plane->acceleration * dt;
		// cap the speed so they don't get outta control
		plane->velocity.limit(plane->get_max_speed());
		plane->position += plane->velocity * dt;
		// reset the acceleration each frame
		plane->acceleration *= 0;
	}

	// for the predators
	for (paper_plane* pred : predators) {
		if (wraparound) {
			wrap(pred);
		}
		else {
			bounding = bound(pred);
			pred->applyForce(bounding, bounding_weight);
		}

		separation = separate(pred, predators);
		alignment = align(pred, planes);
		cohesion = cohere(pred, planes);

		pred->applyForce(separation, separation_weight);
		pred->applyForce(alignment, alignment_weight);
		pred->applyForce(cohesion, cohesion_weight * 2);

		pred->velocity += pred->acceleration * dt;
		pred->velocity.limit(pred->get_max_speed());
		pred->position += pred->velocity * dt;
		pred->acceleration *= 0;
	}
}

void Flock::paper_plane::applyForce(ofVec3f force, float scale) {
	acceleration += force.limit(get_max_force()).scale(scale);
}

void Flock::binRegister(paper_plane* plane) {
	// shift the positions to all be positive to make bin calculation easier
	// e.g. with a radius of 50, something at -50 will become 0 and something
	// at 50 will become 100
	ofVec3f offset_position = plane->position + RADIUS_VECTOR;
	
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
		bins[x][y][z].push_back(plane);
		//cout << "Plane added to bin (" << x << " " << y << " " << z << ")" << endl;
	}
}

vector<Flock::paper_plane*> Flock::aggregrateNeighborCells(int i, int j, int k) {
	vector<paper_plane*> cell, cell_aggregate;
	bool valid_x, valid_y, valid_z;
	for (int x = -1; x <= 1; x++) {
		for (int y = -1; y <= 1; y++) {
			for (int z = -1; z <= 1; z++) {
				valid_x = (i + x >= 0 && i + x <= LATTICE_SUBDIVS);
				valid_y = (j + y >= 0 && j + y <= LATTICE_SUBDIVS);
				valid_z = (k + z >= 0 && k + z <= LATTICE_SUBDIVS);
				if (valid_x && valid_y && valid_z) {
					cell = bins[i][j][k];
					cell_aggregate.insert(cell_aggregate.end(), cell.begin(), cell.end());
				}

			}
		}
	}
	return cell_aggregate;
}

ofVec3f Flock::separate(paper_plane* plane, vector<paper_plane*> &cell) {
	ofVec3f steer, diff;
	int count = 0;

	for (paper_plane* other : cell) {
		float distance = plane->position.squareDistance(other->position);
		//check if too close to other boid
		if (distance > 0 && distance < desired_separation * desired_separation) {
			//create vector pointing away from it
			diff = plane->position - other->position;
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
		steer.scale(plane->get_max_speed());
		steer - plane->velocity;
	}
	return steer;
}

ofVec3f Flock::align(paper_plane* plane, vector<paper_plane*> &cell) {
	int count = 0;
	ofVec3f velocity_sum;
	for (paper_plane* other : cell) {
		float distance = plane->position.squareDistance(other->position);
		if (distance > 0 && distance < neighbor_search_radius * neighbor_search_radius) {
			velocity_sum += other->velocity;
			count++;
		}
	}
	if (count > 0) {
		velocity_sum /= count;
		velocity_sum.scale(plane->get_max_speed());
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
		float distance = plane->position.squareDistance(other->position);
		if (distance > 0 && distance < neighbor_search_radius * neighbor_search_radius) {
			position_sum += other->position;
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

ofVec3f Flock::bound(paper_plane* plane) {
	ofVec3f steer;
	if (plane->position.length() >= MAX_RADIUS) {
		//create vector pointing to origin
		steer = -plane->position / MAX_RADIUS;
	}
	return steer;
}

ofVec3f Flock::seek(paper_plane* plane, ofVec3f target) {
	ofVec3f desired = target - plane->position;
	desired.scale(plane->get_max_speed());
	ofVec3f steer = desired - plane->velocity;
	return steer;
}

ofVec3f Flock::repel(paper_plane* plane, ofVec3f obstacle, float radius) {
	ofVec3f repel;
	ofVec3f fut_pos = plane->position + plane->velocity; // future position for accuracy
	float distance = plane->position.distance(obstacle);

	if (distance <= radius) {
		repel = plane->position - obstacle;
		repel.normalize();
		if (distance != 0) { //Don't divide by zero.
			float scale = 1.0 / distance; //The closer to the obstacle, the stronger the force.
			repel.scale(scale);
		}
	}
	return repel;
}

void Flock::wrap(paper_plane* plane) {
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