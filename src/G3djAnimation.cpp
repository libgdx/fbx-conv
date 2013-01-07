#include "G3djAnimation.h"

namespace fbxconv{
	void G3djAnimation::addKeyframe(Keyframe* keyframe){
		keyframes.push_back(keyframe);
	}

	void G3djAnimation::setBoneId(const char* targetId){
		boneId = new char[strlen(targetId)+1];
		strcpy(boneId, targetId);
	}

	Keyframe* G3djAnimation::getKeyframe(unsigned int index){
		return keyframes[index];
	}

	unsigned int G3djAnimation::getKeyframeCount(){
		return keyframes.size();
	}

	char *G3djAnimation::getBoneId(){
		return boneId;
	}
};