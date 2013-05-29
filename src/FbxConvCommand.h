#ifndef FBXCONVCOMMAND_H
#define FBXCONVCOMMAND_H

#define FILETYPE_AUTO			0x00
#define FILETYPE_FBX			0x10
#define FILETYPE_G3DB			0x20
#define FILETYPE_G3DJ			0x21
#define FILETYPE_OUT_DEFAULT	FILETYPE_G3DB
#define FILETYPE_IN_DEFAULT		FILETYPE_FBX

//#define ALLOW_INPUT_TYPE

#include <string>

namespace fbxconv {

struct FbxConvCommand {
	const int argc;
	const char **argv;
	std::string error;
	std::string inFile;
	int inType;
	std::string outFile;
	int outType;
	bool flipV;
	bool packColors;
	bool help;
	bool verbose;
	int maxNodePartBonesCount;
	int maxVertexBonesCount;
	int maxVertexCount;
	int maxIndexCount;

	FbxConvCommand(const int &argc, const char** argv) : argc(argc), argv(argv) {
		help = (argc <= 1);
		flipV = false;
		packColors = false;
		verbose = false;
		maxNodePartBonesCount = 12;
		maxVertexBonesCount = 4;
		maxVertexCount = (1<<15)-1;
		maxIndexCount = (1<<15)-1;
		outType = inType = FILETYPE_AUTO;
		for (int i = 1; i < argc; i++) {
			const char *arg = argv[i];
			const int len = (int)strlen(arg);
			if (len > 1 && arg[0] == '-') {
				if (arg[1] == '?')
					help = true;
				else if (arg[1] == 'f')
					flipV = true;
				else if (arg[1] == 'v')
					verbose = true;
				else if (arg[1] == 'p')
					packColors = true;
				else if ((arg[1] == 'i') && (i + 1 < argc))
					inType = parseType(argv[++i]);
				else if ((arg[1] == 'o') && (i + 1 < argc))
					outType = parseType(argv[++i]);
				else if ((arg[1] == 'b') && (i + 1 < argc))
					maxNodePartBonesCount = atoi(argv[++i]);
				else if ((arg[1] == 'w') && (i + 1 < argc))
					maxVertexBonesCount = atoi(argv[++i]);
				else if ((arg[1] == 'm') && (i + 1 < argc))
					maxVertexCount = maxIndexCount = atoi(argv[++i]);
				else
					((error = "Unknown commandline option '") += arg) += "'";
			}
			else if (inFile.length() < 1)
				inFile = arg;
			else if (outFile.length() < 1)
				outFile = arg;
			else
				((error = "Unknown commandline argument '") += arg) += "'";
			if (!error.empty())
				break;
		}
		if (error.empty())
			validate();
	}

	void printCommand() const {
		for (int i = 1; i < argc; i++) {
			if (i > 1)
				printf(" ");
			printf(argv[i]);
		}
		printf("\n");
	}

	void printHelp() const {
		printf("Usage: fbx-conv.exe [options] <input> [<output>]\n");
		printf("\n");
		printf("Options:\n");
		printf("-?       : Display this help information.\n");
#ifdef ALLOW_INPUT_TYPE
		printf("-i <type>: Set the type of the input file to <type>\n");
#endif
		printf("-o <type>: Set the type of the output file to <type>\n");
		printf("-f       : Flip the V texture coordinates.\n");
		printf("-p       : Pack vertex colors to one float.\n");
		printf("-m <size>: The maximum amount of vertices or indices a mesh may contain (default: 32k)\n");
		printf("-b <size>: The maximum amount of bones a nodepart can contain (default: 12)\n");
		printf("-w <size>: The maximum amount of bone weights per vertex (default: 4)\n");
		printf("-v       : Verbose: print additional progress information\n");
		printf("\n");
		printf("<input>  : The filename of the file to convert.\n");
		printf("<output> : The filename of the converted file.\n");
		printf("\n");
		printf("<type>   : FBX, G3DJ (json) or G3DB (binary).\n");
	}
private:
	void validate() {
		if (inFile.empty()) {
			error = "No input file specified";
			return;
		}
#ifdef ALLOW_INPUT_TYPE
		if (inType == FILETYPE_AUTO)
			inType = guessType(inFile, FILETYPE_IN_DEFAULT);
#else
		inType = FILETYPE_IN_DEFAULT;
#endif
		if (outFile.empty())
			setExtension(outFile = inFile, outType = (outType == FILETYPE_AUTO ? FILETYPE_OUT_DEFAULT : outType));
		else if (outType == FILETYPE_AUTO)
			outType = guessType(outFile);
		if (maxVertexBonesCount < 0 || maxVertexBonesCount > 8) {
			error = "Maximum vertex weights must be between 0 and 8";
			return;
		}
		if (maxNodePartBonesCount < maxVertexBonesCount) {
			error = "Maximum bones per nodepart must be greater or equal to the maximum vertex weights";
			return;
		}
		if (maxVertexCount < 0 || maxVertexCount > (1<<15)-1) {
			error = "Maximum vertex count must be between 0 and 32k";
			return;
		}
	}

	int parseType(const char* arg, const int &def = -1) {
		if (stricmp(arg, "fbx")==0)
			return FILETYPE_FBX;
		else if (stricmp(arg, "g3db")==0)
			return FILETYPE_G3DB;
		else if (stricmp(arg, "g3dj")==0)
			return FILETYPE_G3DJ;
		if (def < 0)
			((error = "Unknown filetype '") += arg) += "'";
		return def;
	}

	int guessType(const std::string &fn, const int &def = -1) {
		int o = (int)fn.find_last_of('.');
		if (o == std::string::npos)
			return def;
		std::string ext = fn.substr(++o, fn.length() - o);
		return parseType(ext.c_str(), def);
	}

	void setExtension(std::string &fn, const std::string &ext) const {
		int o = (int)fn.find_last_of('.');
		if (o == std::string::npos)
			fn += "." + ext;
		else
			fn = fn.substr(0, ++o) + ext;
	}

	void setExtension(std::string &fn, const int &type) const {
		switch(type) {
		case FILETYPE_FBX:	return setExtension(fn, "fbx");
		case FILETYPE_G3DB:	return setExtension(fn, "g3db");
		case FILETYPE_G3DJ:	return setExtension(fn, "g3dj");
		default:			return setExtension(fn, "");
		}
	}
};

}

#endif //FBXCONVCOMMAND_H