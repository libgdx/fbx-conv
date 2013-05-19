#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_KEYFRAME_H
#define MODELDATA_KEYFRAME_H

#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {

	struct Keyframe : public json::Serializable {
		double time;
		double translation[3];
		double rotation[4];
		double scale[3];
		bool hasTranslation;
		bool hasRotation;
		bool hasScale;

		Keyframe() {
			time = 0.;
			translation[0] = translation[1] = translation[2] = 0.;
			rotation[0] = rotation[1] = rotation[2] = 0.; rotation[3] = 1.;
			scale[0] = scale[1] = scale[2] = 1.;
			hasTranslation = hasRotation = hasScale = false;
		}

		Keyframe(const Keyframe &rhs) {
			time = rhs.time;
			memcpy(translation, rhs.translation, sizeof(translation));
			memcpy(rotation, rhs.rotation, sizeof(rotation));
			memcpy(scale, rhs.scale, sizeof(scale));
			hasTranslation = rhs.hasTranslation;
			hasRotation = rhs.hasRotation;
			hasScale = rhs.hasScale;
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
} }

#endif // MODELDATA_KEYFRAME_H