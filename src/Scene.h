#ifndef SCENE_H
#define SCENE_H

#include <map>
#include <string>
#include <vector>

namespace fbxconv {
	class Node;

	class Scene {
	public:
		Scene() { }
		~Scene() { 
			for(unsigned i = 0; i < _rootNodes.size(); i++) {
				Node* node = _rootNodes[i];
				delete node;
			}
		}

		Node* getNode(const char* name) {
			return _nodes[name];
		}

		void addNode(Node* node, bool isRoot) {
			if(isRoot) {
				_rootNodes.push_back(node);
			}			
			_nodes[node->getName()] = node;
		}

		const std::vector<Node*>& getRootNodes() {
			return _rootNodes;
		}

	private:
		std::vector<Node*> _rootNodes;
		std::map<std::string, Node*> _nodes;
	};
};

#endif