#include "flock.h"

Flock::Flock() {
	light.setAmbientColor(ofColor(128.0f, 0, 0));
	cone.set(.25f, 0.5f);
	// rotate cone be properly oriented when pointing

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
		//ofDrawSphere(planes[i].position, 0.5f);
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

	float dTheta = TWO_PI / (float)planes.size();
	for (unsigned int i = 0; i < planes.size(); i++) {
		// ok so basically numerically integrate
		// v = dx / dt
		// dx = v * dt
		// x = x + dx
		planes[i].position += planes[i].velocity * dt;
		//planes[i].position = ofVec3f(10.0f * cos(theta), 10.0f * sin(theta), 0.0f);
		// for now, velocity is constant.
		// in the future, acceleration will be non-zero
		// the flocking rules will be called here to change accel
		//planes[i].velocity += planes[i].acceleration * dt;
		float theta = (float)i * dTheta + (float)ofGetFrameNum() / (PI * 100.0f);
		planes[i].velocity = ofVec3f(10.0f * -sin(theta), 10.0f * cos(theta), 0.0f);

		if (planes[i].position.lengthSquared() >= MAX_RADIUS * MAX_RADIUS) {
			planes[i].velocity = -(planes[i].velocity);
		}
	}
}
