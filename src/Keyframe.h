#ifndef KEYFRAME_H
#define KEYFRAME_H

#include "gameplay\Vector3.h"
#include "gameplay\Quaternion.h"

using namespace gameplay;

namespace fbxconv {
	class Keyframe {
	public:
		float keytime;
		Vector3 translation;
		Vector3 scale;
		Quaternion rotation;
	};
};

#endif 