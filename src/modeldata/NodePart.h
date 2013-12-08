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
#ifndef MODELDATA_NODEPART_H
#define MODELDATA_NODEPART_H

#include <vector>
#include "MeshPart.h"
#include "Material.h"
#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {
	struct Node;
	/** A nodepart references (but not owns) a meshpart and a material */
	struct NodePart : public json::ConstSerializable {
		const MeshPart *meshPart;
		const Material *material;
		std::vector<std::pair<Node *, FbxAMatrix> > bones;
		std::vector<std::vector<Material::Texture *> > uvMapping;

		NodePart() : meshPart(0), material(0) {}

		NodePart(const NodePart &copyFrom) : meshPart(copyFrom.meshPart), material(copyFrom.material) {
			bones.insert(bones.end(), copyFrom.bones.begin(), copyFrom.bones.end());
			uvMapping.resize(copyFrom.uvMapping.size());
			for (unsigned int i = 0; i < uvMapping.size(); i++)
				uvMapping[i].insert(uvMapping[i].begin(), copyFrom.uvMapping[i].begin(), copyFrom.uvMapping[i].end());
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
}
}

#endif // MODELDATA_NODEPART_H