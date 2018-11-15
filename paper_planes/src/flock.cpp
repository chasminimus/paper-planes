#include "flock.h"

Flock::Flock() {
	light.setAmbientColor(ofColor(128.0f, 0, 0));
}

void Flock::init(int n_planes) {
	if (planes.size() != 0) {
		ofLogWarning("Flock") << "Flock already initialized!?";
		planes.clear();
	}

	ofSeedRandom();

	ofVec3f position;
	ofVec3f velocity;
	ofVec3f acceleration(0, 0, 0); //linear motion for now

	for (int i = 0; i < n_planes; i++) {
		position.x = (ofRandom(1.0f) - 0.5f);
		position.y = (ofRandom(1.0f) - 0.5f);
		position.z = (ofRandom(1.0f) - 0.5f);
		
		velocity.x = (ofRandom(1.0f) - 0.5f);
		velocity.y = (ofRandom(1.0f) - 0.5f);
		velocity.z = (ofRandom(1.0f) - 0.5f);

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
	for (unsigned int i = 0; i < planes.size(); i++) {
		ofDrawSphere(planes[i].position, 1.0); //improvise for now
	}

	light.disable();
	ofDisableLighting();
	ofPopStyle();
}

void Flock::update() {
	// time differential used for updating vectors
	float dt = ofGetLastFrameTime();

	for (unsigned int i = 0; i < planes.size(); i++) {
		// ok so basically numerically integrate
		// v = dx / dt
		// dx = v * dt
		// x = x + dx
		planes[i].position += planes[i].velocity * dt;
		
		// for now, velocity is constant.
		// in the future, acceleration will be non-zero
		// the flocking rules will be called here to change accel
		planes[i].velocity += planes[i].acceleration * dt;
	}
}
