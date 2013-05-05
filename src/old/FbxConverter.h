#ifndef FBXCONVERTER_H
#define FBXCONVERTER_H

#include "G3djFile.h"
#include "G3djAnimation.h"
#include <fbxsdk.h>

#include "FbxConverterConfig.h"

namespace fbxconv
{
	class FbxConverter {
		static const unsigned int SCENE_SKIN_VERTEXINFLUENCES_MAX = 4;
	public:
		FbxConverter(FbxConverterConfig config);
		~FbxConverter();

		G3djFile* load(const char* fileName);
	private:
		void loadScene(FbxScene* fbxScene);
		void loadAnimations(FbxScene* fbxScene);

		G3djNode* loadNode(FbxNode* fbxNode, bool isRoot);
		void loadBindShapes(FbxScene* fbxScene);
		void loadModel(FbxNode* fbxNode, Node* node);
		Mesh* loadMesh(FbxMesh* fbxMesh);
		void loadTexturesFromProperty(FbxProperty fbxProperty, G3djMaterial* material);
		void loadSkin(FbxMesh* fbxMesh, Model* model);
		bool loadBlendWeights(FbxMesh* fbxMesh, std::vector<std::vector<Vector2> >& weights);

		void loadAnimationLayer(FbxAnimLayer* fbxAnimLayer, FbxNode* fbxNode, AnimationClip* clip);
		void loadAnimationChannels(FbxAnimLayer* animLayer, FbxNode* fbxNode, G3djAnimation* animation, AnimationClip* clip);

		void loadTextureCoords(FbxMesh*, const FbxGeometryElementUV*, int, int, int, int, Vertex*);
		void loadNormal(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadTangent(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadBinormal(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadVertexColor(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex);
		void loadBlendData(const std::vector<Vector2>& vertexWeights, Vertex* vertex);

		void triangulateRecursive(FbxNode* fbxNode);
		void transformNode(FbxNode* fbxNode, G3djNode* node);

		void copyMatrix(const FbxMatrix& fbxMatrix, float* matrix);
		void copyMatrix(const FbxMatrix& fbxMatrix, Matrix& matrix);

		bool isGroupAnimationPossible(FbxScene *fbxScene);
		bool isGroupAnimationPossible(FbxNode *fbxNode);
		bool isGroupAnimationPossible(FbxMesh *fbxMesh);

		FbxAnimCurve* getCurve(FbxPropertyT<FbxDouble3>& prop, FbxAnimLayer* animLayer, const char* pChannel);
		void findMinMaxTime(FbxAnimCurve* animCurve, float* startTime, float* stopTime, float* frameRate);
		Mesh* getMesh(FbxUInt64 meshId);
		void saveMesh(FbxUInt64 meshId, Mesh* mesh);

		G3djFile* g3djFile;
		FbxConverterConfig config;

		bool autoGroupAnimations;
		
		// The animation that channels should be added to if the user is using the -groupAnimation command line argument. May be NULL.
		Animation* groupAnimation;

		// The collection of meshes for the purpose of making sure that the same model is not loaded twice. (Mesh instancing)
		std::map<FbxUInt64, Mesh*> meshes;
		// We keep a list of all nodes, because the G3djFile only has the hierarchical nodes. Hrm
		std::map<std::string, Node*> nodes;
	};
};

#endif