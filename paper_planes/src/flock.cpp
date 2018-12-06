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

	for (unsigned int i = 0; i < planes.size(); i++) {
		
		separation = separate(i);
		alignment = align(i);
		cohesion = cohere(i);

		planes[i].apply_force(separation, separation_weight);
		planes[i].apply_force(alignment, alignment_weight);
		planes[i].apply_force(cohesion, cohesion_weight);

		if (wraparound) {
			wrap(i);
		}
		else {
			bounding = bound(i);
			planes[i].apply_force(bounding, bounding_weight);
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

void Flock::paper_plane::apply_force(ofVec3f force, float scale) {
	acceleration += force.limit(max_force).scale(scale);
}

ofVec3f Flock::separate(int index) {
	paper_plane this_plane = planes[index];
	ofVec3f steer, diff;
	int count = 0;

	for (int i = 0; i < planes.size(); i++) {
		paper_plane other = planes[i];
		float distance = this_plane.position.distance(other.position);
		//check if too close to other boid
		if (distance > 0 && distance < desired_separation) {
			//create vector pointing away from it
			diff = ofVec3f(this_plane.position - other.position);
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
		steer - this_plane.velocity;
	}
	return steer;
}

ofVec3f Flock::align(int index) {
	paper_plane this_plane = planes[index];
	int count = 0;
	ofVec3f velocity_sum;
	for (int i = 0; i < planes.size(); i++) {
		paper_plane other = planes[i];
		float distance = this_plane.position.distance(other.position);
		if (distance > 0 && distance < neighbor_search_radius) {
			velocity_sum += other.velocity;
			count++;
		}
	}
	if (count > 0) {
		velocity_sum /= count;
		velocity_sum.scale(max_speed);
		return velocity_sum - this_plane.velocity;
	}
	else {
		return ZERO_VECTOR;
	}
}

ofVec3f Flock::cohere(int index) {
	paper_plane this_plane = planes[index];
	ofVec3f position_sum;
	int count = 0;
	for (int i = 0; i < planes.size(); i++) {
		paper_plane other = planes[i];
		float distance = this_plane.position.distance(other.position);
		if (distance > 0 && distance < neighbor_search_radius) {
			position_sum += planes[i].position;
			count++;
		}
	}
	if (count > 0) {
		position_sum /= count; // center of mass of the flock
		return seek(index, position_sum);
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
	paper_plane* this_plane = &planes[index];
	// "why don't you just flip the position vector?"
	// well, then they'd get stuck outside and constantly zap back and forth
	// there's probably a fix for that but this isn't that horrible
	if (this_plane->position.x < -MAX_RADIUS) {
		this_plane->position.x = MAX_RADIUS;
	}			  
	if (this_plane->position.x > MAX_RADIUS) {
		this_plane->position.x = -MAX_RADIUS;
	}			  
	if (this_plane->position.y < -MAX_RADIUS) {
		this_plane->position.y = MAX_RADIUS;
	}			  
	if (this_plane->position.y > MAX_RADIUS) {
		this_plane->position.y = -MAX_RADIUS;
	}			  
	if (this_plane->position.z < -MAX_RADIUS) {
		this_plane->position.z = MAX_RADIUS;
	}			  
	if (this_plane->position.z > MAX_RADIUS) {
		this_plane->position.z = -MAX_RADIUS;
	}
}

ofVec3f Flock::seek(int index, ofVec3f target) {
	paper_plane this_plane = planes[index];
	ofVec3f desired = target - this_plane.position;
	desired.scale(max_speed);
	ofVec3f steer = desired - this_plane.velocity;
	return steer;
}