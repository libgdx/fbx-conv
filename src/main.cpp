#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include <fbxsdk.h>
#include "FbxConvCommand.h"
#include "readers/FbxConverter.h"
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

void printModelInfo(const Model *model) {
	printf("Model ID             : %s\n", model->id.c_str());
	printf("Number of meshes     : %d\n", model->meshes.size());
	printf("Number of materials  : %d\n", model->materials.size());
	printf("Number of root nodes : %d\n", model->nodes.size());
	printf("Number of animations : %d\n", model->animations.size());
	printf("\n");
	printf("Listing meshes       :\n");
	for (int i = 0; i < model->meshes.size(); i++) {
		printf("- Mesh %d\n", i);
		printf("  - Attributes       : ");
		for (int j = 0; j < model->meshes[i]->attributes.length(); j++)
			printf("%s%s", (j == 0) ? "" : ", ", model->meshes[i]->attributes.name(j));
		printf("\n");
		printf("  - VertexSize       : %d (%d bytes)\n", model->meshes[i]->vertexSize, model->meshes[i]->vertexSize * 4);
		printf("  - VertexCount      : %d (%d bytes)\n", model->meshes[i]->vertices.size() / model->meshes[i]->vertexSize, model->meshes[i]->vertices.size());
		unsigned long indexCount = model->meshes[i]->indexCount();
		printf("  - IndexCount       : %d (%d bytes)\n", indexCount, indexCount * 2);
		printf("  - Numder of parts  : %d\n", model->meshes[i]->parts.size());
		for (int j = 0; j < model->meshes[i]->parts.size(); j++)
			printf("    - Part %-5d     : '%s' of %s with %d indices.\n", j, model->meshes[i]->parts[j]->id.c_str(),
				G3djWriter::getPrimitiveTypeString(model->meshes[i]->parts[j]->primitiveType), model->meshes[i]->parts[j]->indices.size());
	}
	printf("\n");
}

void textureCallback(Material::Texture *texture) {
	texture->path = texture->path.substr(texture->path.find_last_of("/\\")+1);
}

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

	printf("Converting source file...\n");
	if (!reader.convert(model, command.flipV)) {
		delete model;
		model = 0;
	}
	if (model != 0) {
		if (command.verbose)
			printModelInfo(model);
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