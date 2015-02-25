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
	template<int n>
	struct Keyframe : public json::ConstSerializable {
		static const unsigned char InterpolationConstant = 0;
		static const unsigned char InterpolationLinear = 1;
		static const unsigned char InterpolationCubic = 2;

		float time;
		float value[n];
		unsigned char interpolation;

		Keyframe() {
			time = 0.;
			memset(value, 0, sizeof(value));
			interpolation = InterpolationLinear;
		}

		Keyframe(const Keyframe<n> &rhs) {
			time = rhs.time;
			memcpy(value, rhs.value, sizeof(value));
		}

		virtual void serialize(json::BaseJSONWriter &writer) const {
			writer << json::obj;
			if (time != 0.0f)
				writer << "keytime" = time;
			writer << "value" = value;
			writer << json::end;
		}

	};

} }

#endif // MODELDATA_KEYFRAME_H