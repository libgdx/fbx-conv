#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_KEYFRAME_H
#define MODELDATA_KEYFRAME_H

namespace fbxconv {
namespace modeldata {
	struct Keyframe {
		double time;
		double translation[3];
		double rotation[4];
		double scale[3];

		Keyframe() {
			memset(this, 0, sizeof(this));
		}

		Keyframe(const Keyframe &copyFrom) {
			memcpy(this, &copyFrom, sizeof(this));
		}
	};
} }

#endif // MODELDATA_KEYFRAME_H