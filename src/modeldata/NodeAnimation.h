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