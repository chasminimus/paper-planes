#include "ofApp.h"

ofxAssimpModelLoader model;

//--------------------------------------------------------------
void ofApp::setup() {
	ofEnableSmoothing();
	
	//model.loadModel("model/paper_plane.obj");
	
	// set up camera
	camEasy.setTarget(node_paper_planes);
	camEasy.setDistance(25);
	camEasy.setNearClip(0);
	camEasy.setFarClip(10000);

	// set up gui
	gui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	gui->addHeader(":: Controls ::");
	gui->addFooter();

	// weight sliders
	f_weights = gui->addFolder("Weights");

	s_separation = f_weights->addSlider("Separation", 0, 5.0);
	s_alignment = f_weights->addSlider("Alignment", 0, 5.0);
	s_cohesion = f_weights->addSlider("Cohesion", 0, 5.0);
	s_bounding = f_weights->addSlider("Bounding", 0, 5.0);

	// weight slider bindings
	s_separation->bind(Flock::separation_weight);
	s_alignment->bind(Flock::alignment_weight);
	s_cohesion->bind(Flock::cohesion_weight);
	s_bounding->bind(Flock::bounding_weight);

	// other controls
	s_speed = gui->addSlider("Simluation Speed", 0, 10.0);
	s_desired_separation = gui->addSlider("Separation Distance", 0, 20.0);
	s_neighbor_radius = gui->addSlider("Neighbor Radius", 0, 20.0);
	
	t_wraparound = gui->addToggle("Wraparound");
	t_wraparound->onToggleEvent(this, &ofApp::onToggleEvent);

	// and their bindings
	s_speed->bind(Flock::sim_speed);
	s_desired_separation->bind(Flock::desired_separation);
	s_neighbor_radius->bind(Flock::neighbor_search_radius);

	node_paper_planes.init(150);
}

//--------------------------------------------------------------
void ofApp::update() {

}

//--------------------------------------------------------------
void ofApp::draw() {
	//ofBackground(50, 50, 50, 0);
	camEasy.begin();
	ofSetColor(255, 255, 255, 255);
	//ofDrawRotationAxes(MAX_RADIUS, 0.1f);
	ofDrawGrid((float)MAX_RADIUS / (LATTICE_SUBDIVS / 2), LATTICE_SUBDIVS / 2, true, true, true, true);
	node_paper_planes.customDraw();
	camEasy.end();
}

void ofApp::onToggleEvent(ofxDatGuiToggleEvent e)
{
	cout << e.target->getLabel() << " checked = " << e.checked << endl;
	Flock::wraparound = e.checked;
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
