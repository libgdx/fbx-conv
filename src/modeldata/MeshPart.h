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
#ifndef MODELDATA_MESHPART_H
#define MODELDATA_MESHPART_H

#define PRIMITIVETYPE_POINTS		0
#define PRIMITIVETYPE_LINES			1
#define PRIMITIVETYPE_LINESTRIP		3
#define PRIMITIVETYPE_TRIANGLES		4
#define PRIMITIVETYPE_TRIANGLESTRIP	5

#include <string>
#include <fbxsdk.h>
#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {
	struct MeshPart : public json::ConstSerializable {
		std::string id;
		std::vector<unsigned short> indices;
		unsigned int primitiveType;
		std::vector<FbxCluster *> sourceBones;

		MeshPart() : primitiveType(0) {}

		MeshPart(const MeshPart &copyFrom) {
			set(copyFrom.id.c_str(), copyFrom.primitiveType, copyFrom.indices);
		}

		~MeshPart() {
			clear();
		}

		void clear() {
			indices.clear();
			id.clear();
			primitiveType = 0;
		}

		void set(const char *id, const unsigned int &primitiveType, const std::vector<unsigned short> &indices) {
			this->id = id;
			this->primitiveType = primitiveType;
			this->indices.clear();
			this->indices.insert(this->indices.end(), indices.begin(), indices.end());
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
}
}

#endif //MODELDATA_MESHPART_H