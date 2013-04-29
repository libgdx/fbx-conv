#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <fbxsdk.h>
#include "FbxConvCommand.h"
#include "readers/FbxReader.h"
#include "writers/G3djWriter.h"
#include "writers/G3dbWriter.h"
#include <string>
#include <iostream>

#define VERSION_HI	0
#define VERSION_LO	1

using namespace fbxconv;
using namespace fbxconv::modeldata;
using namespace fbxconv::writers;
using namespace fbxconv::readers;

void textureCallback(Material::Texture *texture) {
	texture->path = texture->path.substr(texture->path.find_last_of("/\\")+1);
}

int process(int argc, const char** argv) {
	printf("FBX to G3DJ converter, version %d.%d\n\n", VERSION_HI, VERSION_LO);
	FbxConvCommand command(argc, argv);
	if (command.help) {
		command.printHelp();
		return 1;
	}
	if (command.error.length() > 0)
	{
		printf("ERROR: %s\n\n", command.error.c_str());
		command.printHelp();
		return 1;
	}
	if (command.inType != FILETYPE_FBX) {
		printf("ERROR: incorrect input filetype\n");
		return 1;
	}

	modeldata::Model *model = new modeldata::Model();
	fbxconv::readers::FbxReader reader;

	printf("Loading source file...\n");
	FbxScene *scene = reader.openFbxFile(command.inFile.c_str());
	if (scene == NULL) {
		printf("ERROR: %s\n", reader.error);
		return 1;
	}

#ifdef _DEBUG
	std::vector<std::string> textures;
	if (reader.listTextures(scene, textures)) {
		printf("Textures within this file\n");
		for (std::vector<std::string>::const_iterator itr = textures.begin(); itr != textures.end(); ++itr)
			printf("%s\n", (*itr).c_str());
	}
#endif

	printf("Converting source file...\n");
	reader.textureCallback = textureCallback;
	if (!reader.load(model, scene)) {
		printf("ERROR: %s\n", reader.error);
		return 1;
	}
	reader.closeFbxFile(scene);

	if (command.outType == FILETYPE_G3DB) {
		printf("Exporting to g3db...\n");
		G3dbWriter writer;
		writer.exportG3db(model, command.outFile.c_str());
	} else{
		printf("Exporting to g3dj...\n");
		JSONWriter writer(command.outFile.c_str());
		G3djWriter exporter;
		exporter.exportModel(model, &writer);
	}
	delete model;

#ifdef _DEBUG
	std::cout << "Press ENTER to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
#endif

	return 0;
}

int main(int argc, const char** argv) {

#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	return process(argc, argv);
}