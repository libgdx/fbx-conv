#ifndef FBXCONVCOMMAND_H
#define FBXCONVCOMMAND_H

#define FILETYPE_AUTO			0x00
#define FILETYPE_FBX			0x10
#define FILETYPE_G3DB			0x20
#define FILETYPE_G3DJ			0x21
#define FILETYPE_OUT_DEFAULT	FILETYPE_G3DJ
#define FILETYPE_IN_DEFAULT		FILETYPE_FBX

//#define ALLOW_INPUT_TYPE

#include <string>

namespace fbxconv {

struct FbxConvCommand {
	std::string error;
	std::string inFile;
	int inType;
	std::string outFile;
	int outType;
	bool flipV;
	bool help;

	FbxConvCommand(const int &argc, const char** argv) {
		help = (argc <= 1);
		flipV = false;
		outType = inType = FILETYPE_AUTO;
		for (int i = 1; i < argc; i++) {
			const char *arg = argv[i];
			const int len = strlen(arg);
			if (len > 1 && arg[0] == '-') {
				if (arg[1] == '?')
					help = true;
				else if (arg[1] == 'i' && i + 1 < argc)
					inType = parseType(argv[++i]);
				else if (arg[i] == 'o' && i + 1 < argc)
					outType = parseType(argv[++i]);
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
		printf("-m <size>: Merge meshes with the same attributes, up to <size> bytes");
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
			setExtension(outFile = inFile, outType = (outType == FILETYPE_AUTO ? FILETYPE_IN_DEFAULT : outType));
		else if (outType == FILETYPE_AUTO)
			outType = guessType(outFile);
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
		int o = fn.find_last_of('.');
		if (o == std::string::npos)
			return def;
		std::string ext = fn.substr(++o, fn.length() - o);
		return parseType(ext.c_str(), def);
	}

	void setExtension(std::string &fn, const std::string &ext) const {
		int o = fn.find_last_of('.');
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