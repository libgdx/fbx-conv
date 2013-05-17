#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_MODEL_H
#define MODELDATA_MODEL_H

#include <vector>
#include <string>
#include "animation.h"
#include "material.h"
#include "mesh.h"
#include "node.h"
#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {
	const short VERSION_HI = 0;
	const short VERSION_LO = 1;

	/** A model is responsable for freeing all animations, materials, meshes and nodes it contains */
	struct Model : public json::Serializable {
		short version[2];
		std::string id;
		std::vector<Animation *> animations;
		std::vector<Material *> materials;
		std::vector<Mesh *> meshes;
		std::vector<Node *> nodes;

		Model() { version[0] = VERSION_HI; version[1] = VERSION_LO; }

		Model(const Model &copyFrom) {
			version[0] = copyFrom.version[0];
			version[1] = copyFrom.version[1];
			id = copyFrom.id;
			for (std::vector<Animation *>::const_iterator itr = copyFrom.animations.begin(); itr != copyFrom.animations.end(); ++itr)
				animations.push_back(new Animation(**itr));
			for (std::vector<Material *>::const_iterator itr = copyFrom.materials.begin(); itr != copyFrom.materials.end(); ++itr)
				materials.push_back(new Material(**itr));
			for (std::vector<Mesh *>::const_iterator itr = copyFrom.meshes.begin(); itr != copyFrom.meshes.end(); ++itr)
				meshes.push_back(new Mesh(**itr));
			for (std::vector<Node *>::const_iterator itr = copyFrom.nodes.begin(); itr != copyFrom.nodes.end(); ++itr)
				nodes.push_back(new Node(**itr));
		}

		~Model() {
			clear();
		}

		void clear() {
			for (std::vector<Animation *>::iterator itr = animations.begin(); itr != animations.end(); ++itr)
				delete *itr;
			animations.clear();
			for (std::vector<Material *>::iterator itr = materials.begin(); itr != materials.end(); ++itr)
				delete *itr;
			materials.clear();
			for (std::vector<Mesh *>::iterator itr = meshes.begin(); itr != meshes.end(); ++itr)
				delete *itr;
			meshes.clear();
			for (std::vector<Node *>::iterator itr = nodes.begin(); itr != nodes.end(); ++itr)
				delete *itr;
			nodes.clear();
		}

		Node *getNode(const char *id) const {
			for (std::vector<Node *>::const_iterator itr = nodes.begin(); itr != nodes.end(); ++itr) {
				if ((*itr)->id.compare(id)==0)
					return *itr;
				Node *cnode = (*itr)->getChild(id);
				if (cnode != NULL)
					return cnode;
			}
			return NULL;
		}

		Material *getMaterial(const char *id) const {
			for (std::vector<Material *>::const_iterator itr = materials.begin(); itr != materials.end(); ++itr)
				if ((*itr)->id.compare(id)==0)
					return *itr;
			return NULL;
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
}
}

#endif