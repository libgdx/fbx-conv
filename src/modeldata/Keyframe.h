/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
/** @author Xoppa */
#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_KEYFRAME_H
#define MODELDATA_KEYFRAME_H

#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {

	struct Keyframe : public json::ConstSerializable {
		float time;
		float translation[3];
		float rotation[4];
		float scale[3];
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