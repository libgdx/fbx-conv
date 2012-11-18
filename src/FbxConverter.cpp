#include <stdio.h>
#include "HierarchyPrinter.h"
#include "FbxConverter.h"

using namespace fbxconv;

FbxConverter::FbxConverter() {
	fbxManager = FbxManager::Create();
	FbxIOSettings* ioSettings = FbxIOSettings::Create(fbxManager, IOSROOT);
	fbxManager->SetIOSettings(ioSettings);
	fbxScene = 0;
}

FbxConverter::~FbxConverter() {
	if(fbxManager) fbxManager->Destroy();
}

bool FbxConverter::load(const char* file) {
	// destroy any old scene
	if(fbxScene) {
		fbxScene->Destroy();
		fbxScene = 0;
	}

	// create and initialize the importer
	FbxImporter* importer = FbxImporter::Create(fbxManager, "");
	if(!importer->Initialize(file, -1, fbxManager->GetIOSettings())) {
		printf("faild to initialize importer for file '%s'\n", file);
		importer->Destroy();
		return false;
	}

	// load the scene and destroy the importer
	fbxScene = FbxScene::Create(fbxManager, "__scene__");
	importer->Import(fbxScene);
	importer->Destroy();
	return true;
}

void FbxConverter::printHierarchy() {
	if(!fbxScene) {
		printf("no file loaded\n");
		return;
	}

	fbxconv::PrintHierarchy(fbxScene);
}

void main(int argc, char** argv) {
	const char* file = "samples/cube.fbx";
	FbxConverter converter;
	if(!converter.load(file)) {
		printf("Couldn't load file '%s'\n", file);
		exit(-1);
	}

	converter.printHierarchy();
	exit(0);
}