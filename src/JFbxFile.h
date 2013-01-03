#ifndef JFBXFILE_H
#define JFBXFILE_H

#include "gameplay\Scene.h"
#include "JFbxFileWriter.h"

using namespace gameplay;

namespace fbxconv {

	class JFbxFile {
	public:
		JFbxFile();
		~JFbxFile();

		void addScene(Scene *scene);

		void addNode(Node* node);
		void addMesh(Mesh* mesh);

		Node* getNode(const char* nodeId);
		Mesh* getMesh(const char* meshId);
		
		void exportWith(JFbxFileWriter *writer);
	private:
		void writeAttributes(Mesh* mesh, JFbxFileWriter *writer);
		void writeVertices(Mesh* mesh, JFbxFileWriter *writer);
		void writeMeshParts(Mesh* mesh, JFbxFileWriter *writer);

		const char* JFbxFile::getPrimitiveTypeString(int primitiveTypeId);

		Scene* scene;

		std::map<std::string, Node*> nodes;
		std::map<std::string, Mesh*> meshes;
	};
};

#endif