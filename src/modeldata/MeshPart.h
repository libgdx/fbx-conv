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

namespace fbxconv {
namespace modeldata {
	struct MeshPart {
		std::string id;
		std::vector<unsigned short> indices;
		unsigned int primitiveType;
		std::vector<const FbxNode *> sourceBones;

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
	};
}
}

#endif //MODELDATA_MESHPART_H