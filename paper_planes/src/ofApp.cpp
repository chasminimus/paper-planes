#include "ofApp.h"

ofxAssimpModelLoader model;

//--------------------------------------------------------------
void ofApp::setup() {
	model.loadModel("..\\model\\paper_plane.obj");
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	ofBackground(50, 50, 50, 0);
	ofSetColor(255, 255, 255, 255);

	model.setPosition(ofGetWidth() * 2 / 6, (float)ofGetHeight() * 0.75, 0);
	model.draw(OF_MESH_FILL); //same as model.drawFaces();
	model.setPosition(ofGetWidth() * 3 / 6, (float)ofGetHeight() * 0.75, 0);
	model.draw(OF_MESH_POINTS); // same as model.drawVertices();
	model.setPosition(ofGetWidth() * 4 / 6, (float)ofGetHeight() * 0.75, 0);
	model.draw(OF_MESH_WIREFRAME); // same as model.drawWireframe();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
