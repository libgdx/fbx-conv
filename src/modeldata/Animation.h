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
#ifndef MODELDATA_ANIMATION_H
#define MODELDATA_ANIMATION_H

#include <vector>
#include "Node.h"
#include "NodeAnimation.h"
#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {
	struct Node;

	struct Animation : public json::ConstSerializable {
		std::string id;
		std::vector<NodeAnimation *> nodeAnimations;

		Animation() {}

		Animation(const Animation &copyFrom) {
			id = copyFrom.id;
			for (std::vector<NodeAnimation *>::const_iterator itr = copyFrom.nodeAnimations.begin(); itr != copyFrom.nodeAnimations.end(); ++itr)
				nodeAnimations.push_back(new NodeAnimation(*(*itr)));
		}

		~Animation() {
			for (std::vector<NodeAnimation *>::iterator itr = nodeAnimations.begin(); itr != nodeAnimations.end(); ++itr)
				if ((*itr)!=0)
					delete *itr;
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
} }

#endif // MODELDATA_ANIMATION_H