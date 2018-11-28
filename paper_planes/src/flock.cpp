#include "flock.h"

Flock::Flock() {
	light.setAmbientColor(ofColor(128.0f, 0, 0));
	cone.set(.25f, 0.5f);
	cone.tiltDeg(90.0f);
}

//flock system variables
float Flock::desired_separation = 25;
float Flock::max_speed = 5;
float Flock::max_force = 0.05;

void Flock::init(int n_planes) {
	if (planes.size() != 0) {
		ofLogWarning("Flock") << "Flock already initialized!?";
		planes.clear();
	}

	ofSeedRandom();
	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration(0, 0, 0); //linear motion for now

	float dTheta = TWO_PI / (float)n_planes;
	for (int i = 0; i < n_planes; i++) {
		/*
		position.x = (ofRandom(1.0f) - 0.5f) * POSITION_DISPERSION;
		position.y = (ofRandom(1.0f) - 0.5f) * POSITION_DISPERSION;
		position.z = (ofRandom(1.0f) - 0.5f) * POSITION_DISPERSION;
		
		velocity.x = (ofRandom(1.0f) - 0.5f) * VELOCITY_DISPERSION;
		velocity.y = (ofRandom(1.0f) - 0.5f) * VELOCITY_DISPERSION;
		velocity.z = (ofRandom(1.0f) - 0.5f) * VELOCITY_DISPERSION;
		*/

		
		// doing circles as a test
		float theta = (float)i * dTheta;
		position = ofVec3f(10.0f * cos(theta), 10.0f * sin(theta), 0.0f);
		velocity = ofVec3f(10.0f * -sin(theta), 10.0f * cos(theta), 0.0f);
		
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
		//cone.rollDeg(180.0f);
		//cone.panDeg(90.0f);
		cone.draw();
	}

	light.disable();
	ofDisableLighting();
	ofPopStyle();
}

void Flock::update() {
	// time differential used for updating vectors
	float dt = ofGetLastFrameTime();

	for (unsigned int i = 0; i < planes.size(); i++) {
		
		ofVec3f separation = separate(i);

		planes[i].apply_force(separation);

		// ok so basically numerically integrate
		// v = dx / dt
		// dx = v * dt
		// x = x + dx

		planes[i].velocity += planes[i].acceleration * dt;
		planes[i].position += planes[i].velocity * dt;

		// boundary constraint (basic for now and prone to sticking on the boundary, will use acceleration eventually)
		if (planes[i].position.lengthSquared() >= MAX_RADIUS * MAX_RADIUS) {
			planes[i].velocity = -(planes[i].velocity);
		}
	}
}

void Flock::paper_plane::apply_force(ofVec3f force) {
	acceleration += force.limit(max_force);
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