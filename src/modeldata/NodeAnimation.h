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
#ifndef MODELDATA_NODEANIMATION_H
#define MODELDATA_NODEANIMATION_H

#include <vector>
#include "Keyframe.h"
#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {
	struct Node;

	struct NodeAnimation : public json::ConstSerializable {
		const Node *node;
		std::vector<Keyframe<3>> translation;
		std::vector<Keyframe<4>> rotation;
		std::vector<Keyframe<3>> scaling;
		bool translate, rotate, scale;

		NodeAnimation() : node(0), translate(false), rotate(false), scale(false) {}

		NodeAnimation(const NodeAnimation &copyFrom) {
			node = copyFrom.node;
			translate = copyFrom.translate;
			rotate = copyFrom.rotate;
			scale = copyFrom.scale;
			for (std::vector<Keyframe<3>>::const_iterator itr = copyFrom.translation.begin(); itr != copyFrom.translation.end(); ++itr)
				translation.push_back(*itr);
			for (std::vector<Keyframe<4>>::const_iterator itr = copyFrom.rotation.begin(); itr != copyFrom.rotation.end(); ++itr)
				rotation.push_back(*itr);
			for (std::vector<Keyframe<3>>::const_iterator itr = copyFrom.scaling.begin(); itr != copyFrom.scaling.end(); ++itr)
				scaling.push_back(*itr);
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
} }

#endif //MODELDATA_NODEANIMATION_H