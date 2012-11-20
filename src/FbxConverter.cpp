#include <stdio.h>
#include "HierarchyPrinter.h"
#include "FbxConverter.h"
#include "Node.h"
#include "Scene.h"

using namespace fbxconv;

FbxConverter::FbxConverter() {
	_fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(_fbxManager, IOSROOT);
	_fbxManager->SetIOSettings(ioSettings);
	_fbxScene = 0;
}

FbxConverter::~FbxConverter() {
	if(_fbxManager) _fbxManager->Destroy();
}

bool FbxConverter::load(const char* file) {
	// destroy any old scene
	if(_fbxScene) {
		_fbxScene->Destroy();
		_fbxScene = 0;
	}

	// create and initialize the importer
	FbxImporter* importer = FbxImporter::Create(_fbxManager, "");
	if(!importer->Initialize(file, -1, _fbxManager->GetIOSettings())) {
		printf("faild to initialize importer for file '%s'\n", file);
		importer->Destroy();
		return false;
	}

	// load the scene and destroy the importer
	_fbxScene = FbxScene::Create(_fbxManager, "__scene__");
	importer->Import(_fbxScene);
	importer->Destroy();

	// triangulate all meshes, nurbs etc.
	printf("Triangulating all meshes\n");
	triangulateRecursive(_fbxScene->GetRootNode());

	// load all nodes, attach them to the scene
	_scene = new Scene();
	FbxNode* fbxRoot = _fbxScene->GetRootNode();
	for(int i = 0; i < fbxRoot->GetChildCount(); i++) {
		Node* node = loadNode(fbxRoot->GetChild(i), true);
	}
	return true;
}

void printNode(Node* node, int tabs) {
	for(int i = 0; i < tabs; i++) printf("   ");
	printf("node\n");
	for(int i = 0; i < node->getChildren().size(); i++) {
		printNode(node->getChildren()[i], ++tabs);
	}	
}

void FbxConverter::printHierarchy() {
	if(!_fbxScene || !_scene) {
		printf("no file loaded\n");
		return;
	}

	fbxconv::PrintHierarchy(_fbxScene);

	for(int i = 0; i < _scene->getRootNodes().size(); i++) {
		Node* node = _scene->getRootNodes()[i];
		printNode(node, 0);
	}
}

void FbxConverter::triangulateRecursive(FbxNode* node) {
	// Triangulate all NURBS, patch and mesh under this node recursively.
    FbxNodeAttribute* nodeAttribute = node->GetNodeAttribute();

    if (nodeAttribute) {
        if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
            nodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
            nodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
            nodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch) {
            FbxGeometryConverter converter(node->GetFbxManager());
            converter.TriangulateInPlace(node);
        }
    }

    const int childCount = node->GetChildCount();
    for (int childIndex = 0; childIndex < childCount; ++childIndex) {
        triangulateRecursive(node->GetChild(childIndex));
    }
}

Node* FbxConverter::loadNode(FbxNode* fbxNode, bool isRoot) {
	Node* node = 0;

	// check if we already loaded this node
	const char* name = fbxNode->GetName();
	if(name && strlen(name) > 0) {
		Node* node = _scene->getNode(name);
		if(node) {
			return node;
		}
	}

	// create a new node
	node = new Node();
	if(name) {
		node->setName(name);
	}
	_scene->addNode(node, isRoot);

	// load transform of this node
	loadTransform(fbxNode, node);
	// TODO load mesh
	// TODO load skeleton

	// load child nodes
	for(int i = 0; i < fbxNode->GetChildCount(); i++) {		
		Node* child = loadNode(fbxNode->GetChild(i), false); // can't be root :)
		node->getChildren().push_back(child);
		child->setParent(node);
	}
	return node;
}

void FbxConverter::loadTransform(FbxNode* fbxNode, Node* node) {
	FbxAMatrix matrix = fbxNode->EvaluateLocalTransform();
    float m[16];
    copyMatrix(matrix, m);
	node->setTransform(m);
}

void FbxConverter::copyMatrix(const FbxMatrix& fbxMatrix, float* matrix)
{
    int i = 0;
    for (int row = 0; row < 4; ++row)
    {
        for (int col = 0; col < 4; ++col)
        {
            matrix[i++] = (float)fbxMatrix.Get(row, col);
        }
    }
}

void main(int argc, char** argv) {
	const char* file = "samples/cubes.fbx";
	FbxConverter converter;
	if(!converter.load(file)) {
		printf("Couldn't load file '%s'\n", file);
		exit(-1);
	}

	converter.printHierarchy();
	exit(0);
}