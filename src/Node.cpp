#include "Node.h"

namespace fbxconv {

Node::Node(): _parent(0), _isJoint(false) {
}

Node::~Node() {
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

int Node::getChildCount() {
	return _children.size();
}

Node* Node::getChild(int index) {
	return _children[index];
}

void Node::addChild(Node* node) {
	_children.push_back(node);
}

Matrix& Node::getTransform() {
	return _transform;
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