#ifndef G3DJANIMATION_H
#define G3DJANIMATION_H

#include "gameplay\Animation.h"
#include "Keyframe.h"

using namespace gameplay;

namespace fbxconv {
	class G3djAnimation : public Animation {
	public:
		void addKeyframe(Keyframe* keyframe);
		void setBoneId(const char* targetId);

		Keyframe* getKeyframe(unsigned int index);
		char* getBoneId();

		unsigned int getKeyframeCount();
	private:
		std::vector<Keyframe*> keyframes;
		char* boneId;
	};
};

#endif