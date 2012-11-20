#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>
#include "math/Matrix.h"

namespace fbxconv {
	// A node in a scene graph, can be a skeleton node (==joint/bone), a model
	// node containing (parts) of a mesh, or an empty node for the purpose of
	// grouping other nodes.
	class Node {
	public:
		Node();
		~Node();
		
		const std::string& getName();

		void setName(const char* name);

		void setName(const std::string& name);

		bool isJoint();

		void setIsJoint(bool isJoint);

		Node* getParent();

		void setParent(Node* parent);

		std::vector<Node*>& getChildren();		

		Matrix& getTransform();

		void setTransform(float* matrix);

		Matrix& getWorldTransform();

	private:
		// the name of the node
		std::string _name;
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