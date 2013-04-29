#include "AnimationClip.h"

namespace fbxconv {
	void AnimationClip::addAnimation(G3djAnimation* animation){
		animations.push_back(animation);
	}

	G3djAnimation* AnimationClip::getAnimation(unsigned int index) const {
		return animations[index];
	}

	unsigned int AnimationClip::getAnimationCount() const {
		return animations.size();
	}

	void AnimationClip::setClipId(const char* clipId){
		this->clipId = new char[strlen(clipId)+1];
		strcpy(this->clipId, clipId);
	}

	char* AnimationClip::getClipId() const {
		return clipId;
	}
};