#include "FbxConverter.h"
#include "G3djWriter.h"
#include "G3dbWriter.h"
#include <string>

// #define COMMAND_INPUT_TYPE

using namespace fbxconv;
using namespace gameplay;

static const int AUTO=0x00;
static const int FBX=0x10;
static const int G3DB=0x20;
static const int G3DJ=0x21;

typedef struct {
	std::string error;
	std::string inFile;
	int inType;
	std::string outFile;
	int outType;
	bool flipV;
} FbxConvCommand;

void printHelp() {
	printf("Usage: fbx-conv.exe [options] <input> [<output>]\n");
	printf("\n");
	printf("Options:\n");
#ifdef COMMAND_INPUT_TYPE
	printf("-in <type>:\tSet the type of the input file to <type>\n");
#endif
	printf("-out <type>:\tSet the type of the output file to <type>\n");
	printf("-flipV:\t\tFlip the V texture coordinates.\n");
	printf("\n");
	printf("<input>:\tThe filename of the file to convert.\n");
	printf("<output>:\tThe filename of the converted file.\n");
	printf("\n");
	printf("<type>:\t\tG3DJ (json) or G3DB (binary).\n");
}

inline int parseType(const char* arg) {
	if (stricmp(arg, "fbx")==0)
		return FBX;
	else if (stricmp(arg, "g3db")==0)
		return G3DB;
	else if (stricmp(arg, "g3dj")==0)
		return G3DJ;
	return -1;
}

inline int guessType(const std::string &fn, int def) {
	int o = fn.find_last_of('.');
	if (o == std::string::npos)
		return def;
	std::string ext = fn.substr(++o, fn.length() - o);
	int r = parseType(ext.c_str());
	return r < 0 ? def : r;
}

inline void setExtension(std::string &fn, const std::string &ext) {
	int o = fn.find_last_of('.');
	if (o == std::string::npos)
		fn += "." + ext;
	else
		fn = fn.substr(0, ++o) + ext;
}

inline void setExtension(std::string &fn, int type) {
	switch(type) {
	case FBX: setExtension(fn, "fbx"); break;
	case G3DB: setExtension(fn, "g3db"); break;
	case G3DJ: setExtension(fn, "g3dj"); break;
	}
}

FbxConvCommand parseCommand(int argc, const char** argv) {
	FbxConvCommand result;
	result.inType = AUTO;
	result.outType = AUTO;
	result.flipV = false;
	for (int i = 1; i < argc; i++) {
		if (argv[i][0] == '-' && i < argc - 1 && strlen(argv[i]) > 1) {
			if (argv[i][1] == 'o') {
				result.outType = parseType(argv[++i]);
				if (result.outType < 0) {
					result.error = "Unknown output type: ";
					result.error += argv[i];
					return result;
				}
			}
#ifdef COMMAND_INPUT_TYPE
			else if (argv[i][1] == 'i') {
				result.inType = parseType(argv[++i]);
				if (result.inType < 0) {
					result.error = "Unknown input type: ";
					result.error += argv[i];
					return result;
				}
			}
#endif
			else if (argv[i][1] == 'f')
				result.flipV = true;
		}
		else if (result.inFile.length() < 1)
			result.inFile = argv[i];
		else if (result.outFile.length() < 1)
			result.outFile = argv[i];
		else {
			result.error = "Unknown commandline option: ";
			result.error += argv[i];
			return result;
		}
	}
	if (result.inFile.length() < 1) {
		result.error = "No input file specified.";
		return result;
	}
	if (result.inType == AUTO)
#ifdef COMMAND_INPUT_TYPE
		result.inType = guessType(result.inFile, FBX);
#else
		result.inType = FBX;
#endif
	if (result.inType != FBX) {
		result.error = "Unsupported input type, only FBX is supported";
		return result;
	}
	if (result.outType == AUTO)
		result.outType = guessType(result.outFile, G3DJ);
	if (result.outType != G3DJ && result.outType != G3DB) {
		result.error = "Unsupported output type, only G3DJ or G3DB is supported";
		return result;
	}
	if (result.outFile.empty()) {
		result.outFile = result.inFile;
		setExtension(result.outFile, result.outType);
	}
	return result;
}

void flipV(G3djFile *file) {
	unsigned int size = file->getMeshCount();
	int *cnt = new int[8];
	memset(cnt, 0, 8*4);
	for (unsigned int i = 0; i < size; i++) {
		Mesh *mesh = file->getMesh(i);
		unsigned int n = mesh->getVertexCount();
		for (unsigned int j = 0; j < n; j++) {
			Vertex *vertex = &(mesh->vertices.at(j));
			if (vertex->hasTexCoord[0])
				vertex->texCoord[0].y = 1.0f - vertex->texCoord[0].y;
			if (vertex->hasTexCoord[1])
				vertex->texCoord[1].y = 1.0f - vertex->texCoord[1].y;
			if (vertex->hasTexCoord[2])
				vertex->texCoord[2].y = 1.0f - vertex->texCoord[2].y;
			if (vertex->hasTexCoord[3])
				vertex->texCoord[3].y = 1.0f - vertex->texCoord[3].y;
			if (vertex->hasTexCoord[4])
				vertex->texCoord[4].y = 1.0f - vertex->texCoord[4].y;
			if (vertex->hasTexCoord[5])
				vertex->texCoord[5].y = 1.0f - vertex->texCoord[5].y;
			if (vertex->hasTexCoord[6])
				vertex->texCoord[6].y = 1.0f - vertex->texCoord[6].y;
			if (vertex->hasTexCoord[7])
				vertex->texCoord[7].y = 1.0f - vertex->texCoord[7].y;
		}
	}
}

int main(int argc, const char** argv) {
	FbxConvCommand command = parseCommand(argc, argv);
	if (command.error.length() > 0)
	{
		printf("ERROR: %s\n\n", command.error.c_str());
		printHelp();
		return 1;
	}

	FbxConverterConfig config = FbxConverterConfig();
	config.flipV = false;

	FbxConverter converter(config);
	G3djFile *g3djFile = converter.load(command.inFile.c_str());

	if (command.flipV) {
		printf("Flipping V\n");
		flipV(g3djFile);
	}

	if (command.outType == G3DJ) {
		printf("Exporting to json.\n");
		G3djWriter writer;
		writer.exportG3dj(g3djFile, command.outFile.c_str());
	} else {
		printf("Exporting to binary.\n");
		G3dbWriter writer;
		if (!writer.exportG3db(*g3djFile, command.outFile.c_str()))
			printf("Error while exporting to binary\n");
	}

	printf("Done.\n");

	return 0;
}