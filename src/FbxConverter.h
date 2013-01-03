#ifndef FBXCONVERTER_H
#define FBXCONVERTER_H

#include "G3djFile.h"
#include <fbxsdk.h>

namespace fbxconv
{
	class JFbxFile;

	class FbxConverter {
	public:
		FbxConverter();
		~FbxConverter();

		G3djFile* load(const char* fileName);
	private:
		void loadScene(FbxScene* fbxScene);
		Node* loadNode(FbxNode* fbxNode);
		void loadBindShapes(FbxScene* fbxScene);
		void loadModel(FbxNode* fbxNode, Node* node);
		Mesh* loadMesh(FbxMesh* fbxMesh);
		void loadSkin(FbxMesh* fbxMesh, Model* model);
		bool loadBlendWeights(FbxMesh* fbxMesh, std::vector<std::vector<Vector2> >& weights);
		
		void loadTextureCoords(FbxMesh*, const FbxGeometryElementUV*, int, int, int, int, Vertex*);
		void loadNormal(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadTangent(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadBinormal(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadVertexColor(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadBlendData(const std::vector<Vector2>& vertexWeights, Vertex* vertex);

		void triangulateRecursive(FbxNode* fbxNode);
		void transformNode(FbxNode* fbxNode, Node* node);

		void copyMatrix(const FbxMatrix& fbxMatrix, float* matrix);

		Mesh* getMesh(FbxUInt64 meshId);
		void saveMesh(FbxUInt64 meshId, Mesh* mesh);

		G3djFile* g3djFile;

		// The collection of meshes for the purpose of making sure that the same model is not loaded twice. (Mesh instancing)
		std::map<FbxUInt64, Mesh*> meshes;
	};
};

#endif