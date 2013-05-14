#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_NODE_H
#define MODELDATA_NODE_H

#include <vector>
#include "NodePart.h"
#include <fbxsdk.h>

namespace fbxconv {
namespace modeldata {
	/** A node is responsable for destroying its parts and children */
	struct Node {
		struct {
			double translation[3];
			double rotation[4];
			double scale[3];
		} transform;
		std::string id;
		std::vector<NodePart *> parts;
		std::vector<Node *> children;
		FbxNode *source;

		Node(const char *id = NULL) : source(0) {
			memset(&transform, 0, sizeof(transform));
			transform.scale[0] = transform.scale[1] = transform.scale[2] = 1.f;
			if (id != NULL)
				this->id = id;
		}

		Node(const Node &copyFrom) {
			id = copyFrom.id;
			source = copyFrom.source;
			memcpy(&transform, &(copyFrom.transform), sizeof(transform));
			for (std::vector<NodePart *>::const_iterator itr = copyFrom.parts.begin(); itr != copyFrom.parts.end(); ++itr)
				parts.push_back(new NodePart(**itr));
			for (std::vector<Node *>::const_iterator itr = copyFrom.children.begin(); itr != copyFrom.children.end(); ++itr)
				children.push_back(new Node(**itr));
		}

		~Node() {
			for (std::vector<NodePart *>::iterator itr = parts.begin(); itr != parts.end(); ++itr)
				delete (*itr);
			for (std::vector<Node *>::iterator itr = children.begin(); itr != children.end(); ++itr)
				delete (*itr);
		}

		Node * getChild(const char *id) const {
			for (std::vector<Node *>::const_iterator itr = children.begin(); itr != children.end(); ++itr) {
				if ((*itr)->id.compare(id)==0)
					return *itr;
				Node *cnode = (*itr)->getChild(id);
				if (cnode != NULL)
					return cnode;
			}
			return NULL;
		}

		bool hasPartsRecursive() const {
			if (!parts.empty())
				return true;
			for (std::vector<Node *>::const_iterator itr = children.begin(); itr != children.end(); ++itr)
				if ((*itr)->hasPartsRecursive())
					return true;
			return false;
		}
	};
}
}

#endif // MODELDATA_NODE_H