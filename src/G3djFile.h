#ifndef JFBXFILE_H
#define JFBXFILE_H

#include "gameplay\Scene.h"
#include "G3djNode.h"
#include "G3djMaterial.h"

using namespace gameplay;

namespace fbxconv {

	class G3djFile {
	public:
		G3djFile();
		~G3djFile();

		void addScene(Scene *scene);

		void addNode(G3djNode* node);
		void addMesh(Mesh* mesh);
		void addMaterial(G3djMaterial *material);

		G3djNode* getNode(const char* nodeId);
		G3djNode* getNode(unsigned int nodeIndex);

		Mesh* getMesh(const char* meshId);
		Mesh* getMesh(unsigned int meshIndex);

		G3djMaterial* getMaterial(const char* materialId);
		G3djMaterial* getMaterial(unsigned int materialIndex);

		unsigned int getMeshCount();
		unsigned int getNodeCount();
		unsigned int getMaterialCount();
	private:
		Scene* scene;

		std::map<std::string, G3djNode*> nodes;
		std::map<std::string, Mesh*> meshes;
		std::map<std::string, G3djMaterial*> materials;
	};
};

#endif