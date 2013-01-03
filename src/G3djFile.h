#ifndef JFBXFILE_H
#define JFBXFILE_H

#include "gameplay\Scene.h"

using namespace gameplay;

namespace fbxconv {

	class G3djFile {
	public:
		G3djFile();
		~G3djFile();

		void addScene(Scene *scene);

		void addNode(Node* node);
		void addMesh(Mesh* mesh);

		Node* getNode(const char* nodeId);
		Node* getNode(unsigned int nodeIndex);
		Mesh* getMesh(const char* meshId);
		Mesh* getMesh(unsigned int meshIndex);

		unsigned int getMeshCount();
		unsigned int getNodeCount();
	private:
		Scene* scene;

		std::map<std::string, Node*> nodes;
		std::map<std::string, Mesh*> meshes;
	};
};

#endif