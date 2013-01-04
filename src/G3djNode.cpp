#include "G3djNode.h"

namespace fbxconv {
	void G3djNode::setTranslation(float x, float y, float z){
		translation.set(x, y, z);
	}
	
	void G3djNode::setRotation(float x, float y, float z){
		rotation.set(x, y, z);
	}

	void G3djNode::setScale(float x, float y, float z){
		scale.set(x, y, z);
	}

	Vector3 G3djNode::getTranslation(){
		return translation;
	}

	Vector3 G3djNode::getRotation(){
		return rotation;
	}

	Vector3 G3djNode::getScale(){
		return scale;
	}
}