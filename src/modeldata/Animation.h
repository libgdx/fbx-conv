#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_ANIMATION_H
#define MODELDATA_ANIMATION_H

#include <vector>
#include "Node.h"
#include "NodeAnimation.h"

namespace fbxconv {
namespace modeldata {
	struct Node;

	struct Animation {
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
	};
} }

#endif // MODELDATA_ANIMATION_H