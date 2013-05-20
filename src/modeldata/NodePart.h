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
	struct NodePart : public json::Serializable {
		const MeshPart *meshPart;
		const Material *material;
		std::vector<Node *> bones;
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