#ifndef NODE_H
#define NODE_H

#include <vector>
#include "math/Matrix.h"

namespace fbxconv {
	// A node in a scene graph, can be a skeleton node (==joint/bone), a model
	// node containing (parts) of a mesh, or an empty node for the purpose of
	// grouping other nodes.
	class Node {
		Node();
		~Node();
		
		bool isJoint();

		void setIsJoint(bool isJoint);

		Node* getParent();

		void setParent(Node* parent);

		int getChildCount();

		Node* getChild(int index);

		void addChild(Node* node);		

		Matrix& getTransform();

		Matrix& getWorldTransform();

	private:
		// whether this is a joint node or not
		bool _isJoint;
		// local transformation
		Matrix _transform;
		// world transform
		Matrix _worldTransform;
		// Parent
		Node* _parent;
		// child nodes
		std::vector<Node*> _children;
	};
}

#endif