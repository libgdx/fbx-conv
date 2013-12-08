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
		std::vector<Keyframe *> keyframes;
		bool translate, rotate, scale;

		NodeAnimation() : node(0), translate(false), rotate(false), scale(false) {}

		NodeAnimation(const NodeAnimation &copyFrom) {
			node = copyFrom.node;
			translate = copyFrom.translate;
			rotate = copyFrom.rotate;
			scale = copyFrom.scale;
			for (std::vector<Keyframe *>::const_iterator itr = copyFrom.keyframes.begin(); itr != copyFrom.keyframes.end(); ++itr)
				keyframes.push_back(new Keyframe(*(*itr)));
		}

		~NodeAnimation() {
			for (std::vector<Keyframe *>::iterator itr = keyframes.begin(); itr != keyframes.end(); ++itr)
				if ((*itr)!=0)
					delete *itr;
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
} }

#endif //MODELDATA_NODEANIMATION_H