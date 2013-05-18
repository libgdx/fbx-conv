#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <fbxsdk.h>
#include "FbxConvCommand.h"
#include "readers/FbxConverter.h"
#include "json/JSONWriter.h"
#include "json/UBJSONWriter.h"
#include "writers/G3dbWriter.h"
#include "writers/G3dWriter.h"
#include <string>
#include <iostream>
#include <fstream>

using namespace fbxconv;
using namespace fbxconv::modeldata;
using namespace fbxconv::writers;
using namespace fbxconv::readers;

int process(int argc, const char** argv) {
	printf("FBX to G3Dx converter, version %d.%d\n\n", VERSION_HI, VERSION_LO);
	FbxConvCommand command(argc, argv);
	if (command.help) {
		command.printHelp();
		return 1;
	}
	if (command.error.length() > 0)
	{
		command.printCommand();
		printf("ERROR: %s\n\n", command.error.c_str());
		command.printHelp();
		return 1;
	}
	if (command.inType != FILETYPE_FBX) {
		printf("ERROR: incorrect input filetype\n");
		return 1;
	}

	printf("Loading source file...\n");
	FbxConverter reader(command.inFile.c_str(), command.packColors, ((1<<15)-1), ((1<<15)-1), command.maxVertexBonesCount, true, command.maxNodePartBonesCount);
	modeldata::Model *model = new modeldata::Model();

	for (std::map<std::string, TextureFileInfo>::iterator it = reader.textureFiles.begin(); it != reader.textureFiles.end(); ++it)
		it->second.path = it->first.substr(it->first.find_last_of("/\\")+1);

	printf("Converting source file...\n");
	if (!reader.convert(model, command.flipV)) {
		delete model;
		model = 0;
	}
	if (model != 0) {
		std::ofstream myfile;
		myfile.open (command.outFile.c_str(), std::ios::binary);

		json::BaseJSONWriter *jsonWriter = 0;
		if (command.outType == FILETYPE_G3DB) {
			printf("Exporting to g3db...\n");
			jsonWriter = new json::UBJSONWriter(myfile);
		} else{
			printf("Exporting to g3dj...\n");
			jsonWriter = new json::JSONWriter(myfile);
		}

		if (jsonWriter) {
			(*jsonWriter) << model;
			delete jsonWriter;
		}
		myfile.close();
		delete model;
	}

	return 0;
}

int main(int argc, const char** argv) {

#if defined(_MSC_VER) && defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif

	int result = process(argc, argv);

#ifdef _DEBUG
	std::cout << "Press ENTER to continue...";
	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
#endif

	return result;
}