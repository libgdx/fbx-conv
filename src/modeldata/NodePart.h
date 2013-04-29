#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_NODEPART_H
#define MODELDATA_NODEPART_H

#include <vector>
#include "MeshPart.h"
#include "Material.h"

namespace fbxconv {
namespace modeldata {
	struct Node;
	/** A nodepart references (but not owns) a meshpart and a material */
	struct NodePart {
		const MeshPart *meshPart;
		const Material *material;
		std::vector<const Node *> links;

		NodePart() : meshPart(0), material(0) {}

		NodePart(const NodePart &copyFrom) : meshPart(copyFrom.meshPart), material(copyFrom.material) {
			links.insert(links.end(), copyFrom.links.begin(), copyFrom.links.end());
		}
	};
}
}

#endif // MODELDATA_NODEPART_H