#ifndef FBX_CONVERTER_H
#define FBX_CONVERTER_H

#include <fbxsdk.h>
#include <map>
#include <string>

namespace fbxconv {
	class Mesh;
	class Node;
	class Scene;
	class Matrix;

	class FbxConverter {
	public:
		FbxConverter();
		~FbxConverter();

		bool load(const char* file);
		void printHierarchy();
		bool write(const char* file);
	private:
		void triangulateRecursive(FbxNode* node);
		Node* loadNode(FbxNode* node, bool isRoot);
		void loadTransform(FbxNode* fbxNode, Node* node);
		void copyMatrix(const FbxMatrix& fbxMatrix, float* matrix);
		void copyMatrix(const FbxMatrix& fbxMatrix, Matrix& matrix);

		FbxManager* _fbxManager;
		FbxScene* _fbxScene;
		Scene* _scene;
	};
};

#endif