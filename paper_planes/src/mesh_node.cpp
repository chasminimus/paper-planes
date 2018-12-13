#include "mesh_node.h"

ofxMeshNode::ofxMeshNode() {
}

ofxMeshNode::ofxMeshNode(ofMesh mesh) {
	mesh_ = mesh;
}

void ofxMeshNode::customDraw() {
	mesh_.draw();
}
