#pragma once

#include "ofMain.h"
#include "ofxDatGui.h"
#include "flock.h"

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void onToggleEvent(ofxDatGuiToggleEvent e);

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
		ofColor SKY_BLUE = ofColor::fromHex(0x66ddff);
		ofEasyCam camEasy;
		Flock node_paper_planes;

		ofxDatGui* gui;
		
		// weights
		ofxDatGuiFolder* f_weights;
		ofxDatGuiSlider* s_separation;
		ofxDatGuiSlider* s_alignment;
		ofxDatGuiSlider* s_cohesion;
		ofxDatGuiSlider* s_bounding;
		ofxDatGuiSlider* s_flee;

		// other controls
		ofxDatGuiSlider* s_speed;
		ofxDatGuiSlider* s_neighbor_radius;
		ofxDatGuiSlider* s_desired_separation;

		ofxDatGuiToggle* t_wraparound;
};
