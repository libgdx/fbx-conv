#include "G3djFile.h"

using namespace gameplay;

namespace fbxconv {
	G3djFile::G3djFile(){
	}

	G3djFile::~G3djFile(){
	}

	void G3djFile::addScene(Scene* scene){
		this->scene = scene;
	}

	void G3djFile::addNode(Node* node){
		nodes[node->getId()] = node;
	}

	void G3djFile::addMesh(Mesh* mesh){
		meshes[mesh->getId()] = mesh;
	}

	Node* G3djFile::getNode(const char* nodeId){
		return nodes[nodeId];
	}

	Node* G3djFile::getNode(unsigned int nodeIndex){
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(nodeIndex >= nodes.size())
			return NULL;
		std::map<std::string, Node*>::iterator i( nodes.begin() );
		std::advance(i, nodeIndex);

		return i->second;
	}

	Mesh* G3djFile::getMesh(const char* meshId){
		return meshes[meshId];
	}

	Mesh* G3djFile::getMesh(unsigned int meshIndex){
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(meshIndex >= meshes.size())
			return NULL;
		std::map<std::string, Mesh*>::iterator i( meshes.begin() );
		std::advance(i, meshIndex);

		return i->second;
	}

	unsigned int G3djFile::getNodeCount(){
		return nodes.size();
	}

	unsigned int G3djFile::getMeshCount(){
		return meshes.size();
	}
};