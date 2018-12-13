#include "mesh_node.h"

meshNode::meshNode() {
}

meshNode::meshNode(ofMesh mesh) {
	mesh_ = mesh;
}

void meshNode::customDraw() {
	mesh_.draw();
}
