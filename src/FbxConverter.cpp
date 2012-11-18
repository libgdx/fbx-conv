#include <stdio.h>
#include <fbxsdk.h>

int main(int argc, char** argv) {
	FbxManager* fbxManager = FbxManager::Create();

	fbxManager->Destroy();
}