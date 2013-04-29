#ifndef JFBXFILE_H
#define JFBXFILE_H

#include "gameplay/Scene.h"
#include "G3djNode.h"
#include "G3djMaterial.h"
#include "AnimationClip.h"

using namespace gameplay;

namespace fbxconv {

	class G3djFile {
	public:
		G3djFile();
		~G3djFile();

		void addScene(Scene *scene);

		void addNode(G3djNode* node);
		void addNodeFlatList(G3djNode* node);
		void addMesh(Mesh* mesh);
		void addMaterial(G3djMaterial* material);
		void addAnimationClip(AnimationClip* animationClip);

		G3djNode* getNode(const char* nodeId);
		G3djNode* getNodeFlatList(const char* nodeId);
		G3djNode* getNode(unsigned int nodeIndex);

		Mesh* getMesh(const char* meshId);
		Mesh* getMesh(unsigned int meshIndex);

		G3djMaterial* getMaterial(const char* materialId);
		G3djMaterial* getMaterial(unsigned int materialIndex);

		AnimationClip* getAnimationClip(unsigned int clipIndex);

		unsigned int getMeshCount() const;
		unsigned int getNodeCount() const;
		unsigned int getMaterialCount() const;
		unsigned int getAnimationClipCount() const;

		std::map<std::string, Mesh*> meshes;
	private:
		Scene* scene;

		// We keep a flat list so we don't double load nodes
		std::map<std::string, G3djNode*> nodesFlatList;
		// This is the actual node hierarchy
		std::map<std::string, G3djNode*> nodes;
		std::map<std::string, G3djMaterial*> materials;
		std::vector<AnimationClip*> animationClips;
	};
};

#endif