#include "Node.h"

namespace fbxconv {

Node::Node(): _parent(0), _isJoint(false) {
}

Node::~Node() {
	for(unsigned i = 0; i < _children.size(); i++) {
		delete _children[i];
	}
}

const std::string& Node::getName() {
	return _name;
}

void Node::setName(const char* name) {
	_name = name;
}

void Node::setName(const std::string& name) {
	_name = name;
}
		
bool Node::isJoint() {
	return _isJoint;
}

void Node::setIsJoint(bool isJoint) {
	this->_isJoint = isJoint;
}

Node* Node::getParent() {
	return _parent;
}

void Node::setParent(Node* node) {
	_parent = node;
}

std::vector<Node*>& Node::getChildren() {
	return _children;
}

Matrix& Node::getTransform() {
	return _transform;
}

void Node::setTransform(float* matrix) {
	memcpy(_transform.m, matrix, sizeof(float) * 16);
}

Matrix& Node::getWorldTransform() {
	if(_parent) {
		Matrix::multiply(_parent->getWorldTransform().m, _transform.m, _worldTransform.m);
	} else {
		memcpy(_worldTransform.m, _transform.m, sizeof(float) * 16);
	}
	return _worldTransform;
}

}