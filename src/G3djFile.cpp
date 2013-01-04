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

	void G3djFile::addNode(G3djNode* node){
		nodes[node->getId()] = node;
	}

	void G3djFile::addMesh(Mesh* mesh){
		meshes[mesh->getId()] = mesh;
	}

	void G3djFile::addMaterial(G3djMaterial* material){
		materials[material->getId()] = material;
	}

	G3djNode* G3djFile::getNode(const char* nodeId){
		std::map<std::string, G3djNode*>::iterator it = nodes.find(nodeId);
		if (it != nodes.end())
		{
			return it->second;
		}
		return NULL;
	}

	G3djNode* G3djFile::getNode(unsigned int nodeIndex){
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(nodeIndex >= nodes.size())
			return NULL;
		std::map<std::string, G3djNode*>::iterator i( nodes.begin() );
		std::advance(i, nodeIndex);

		return i->second;
	}

	Mesh* G3djFile::getMesh(const char* meshId){
		std::map<std::string, Mesh*>::iterator it = meshes.find(meshId);
		if (it != meshes.end())
		{
			return it->second;
		}
		return NULL;
	}

	Mesh* G3djFile::getMesh(unsigned int meshIndex){
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(meshIndex >= meshes.size())
			return NULL;
		std::map<std::string, Mesh*>::iterator i( meshes.begin() );
		std::advance(i, meshIndex);

		return i->second;
	}

	G3djMaterial* G3djFile::getMaterial(const char* materialId){
		std::map<std::string, G3djMaterial*>::iterator it = materials.find(materialId);
		if (it != materials.end())
		{
			return it->second;
		}
		return NULL;
	}

	G3djMaterial* G3djFile::getMaterial(unsigned int materialIndex){
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(materialIndex >= meshes.size())
			return NULL;
		std::map<std::string, G3djMaterial*>::iterator i( materials.begin() );
		std::advance(i, materialIndex);

		return i->second;
	}

	unsigned int G3djFile::getNodeCount(){
		return nodes.size();
	}

	unsigned int G3djFile::getMeshCount(){
		return meshes.size();
	}

	unsigned int G3djFile::getMaterialCount(){
		return materials.size();
	}
};