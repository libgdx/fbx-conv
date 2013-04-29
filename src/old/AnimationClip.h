#ifndef ANIMATIONCLIP_H
#define ANIMATIONCLIP_H

#include "G3djAnimation.h"

namespace fbxconv {
	class AnimationClip {
	public:

		void setClipId(const char* clipId);

		void addAnimation(G3djAnimation* animation);
		G3djAnimation* getAnimation(unsigned int index) const;

		unsigned int getAnimationCount() const;
		char* getClipId() const;
	private:
		std::vector<G3djAnimation*> animations;
		char* clipId;
	};
};

#endif