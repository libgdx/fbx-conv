/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
/** @author Xoppa */
#ifndef FBXCONVCOMMAND_H
#define FBXCONVCOMMAND_H

//#define ALLOW_INPUT_TYPE

#include "Settings.h"
#include <string>
#include "log/log.h"

namespace fbxconv {

struct FbxConvCommand {
	const int argc;
	const char **argv;
	int error;
	bool help;
	Settings *settings;
	log::Log *log;

	FbxConvCommand(log::Log *log, const int &argc, const char** argv, Settings *settings)
		: log(log), argc(argc), argv(argv), settings(settings), error(log::iNoError) {
		help = (argc <= 1);

		settings->flipV = false;
		settings->packColors = false;
		settings->verbose = false;
		settings->maxNodePartBonesCount = 12;
		settings->maxVertexBonesCount = 4;
		settings->maxVertexCount = (1<<15)-1;
		settings->maxIndexCount = (1<<15)-1;
		settings->outType = FILETYPE_AUTO;
		settings->inType = FILETYPE_AUTO;

		for (int i = 1; i < argc; i++) {
			const char *arg = argv[i];
			const int len = (int)strlen(arg);
			if (len > 1 && arg[0] == '-') {
				if (arg[1] == '?')
					help = true;
				else if (arg[1] == 'f')
					settings->flipV = true;
				else if (arg[1] == 'v')
					settings->verbose = true;
				else if (arg[1] == 'p')
					settings->packColors = true;
				else if ((arg[1] == 'i') && (i + 1 < argc))
					settings->inType = parseType(argv[++i]);
				else if ((arg[1] == 'o') && (i + 1 < argc))
					settings->outType = parseType(argv[++i]);
				else if ((arg[1] == 'b') && (i + 1 < argc))
					settings->maxNodePartBonesCount = atoi(argv[++i]);
				else if ((arg[1] == 'w') && (i + 1 < argc))
					settings->maxVertexBonesCount = atoi(argv[++i]);
				else if ((arg[1] == 'm') && (i + 1 < argc))
					settings->maxVertexCount = settings->maxIndexCount = atoi(argv[++i]);
				else
					log->error(error = log::eCommandLineUnknownOption, arg);
			}
			else if (settings->inFile.length() < 1)
				settings->inFile = arg;
			else if (settings->outFile.length() < 1)
				settings->outFile = arg;
			else
				log->error(error = log::eCommandLineUnknownArgument, arg);
			if (error != log::iNoError)
				break;
		}
		if (error == log::iNoError)
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
		if (settings->inFile.empty()) {
			log->error(error = log::eCommandLineMissingInputFile);
			return;
		}
#ifdef ALLOW_INPUT_TYPE
		if (inType == FILETYPE_AUTO)
			inType = guessType(inFile, FILETYPE_IN_DEFAULT);
#else
		settings->inType = FILETYPE_IN_DEFAULT;
#endif
		if (settings->outFile.empty())
			setExtension(
				settings->outFile = settings->inFile, 
				settings->outType = (settings->outType == FILETYPE_AUTO ? FILETYPE_OUT_DEFAULT : settings->outType));
		else if (settings->outType == FILETYPE_AUTO)
			settings->outType = guessType(settings->outFile);
		if (settings->maxVertexBonesCount < 0 || settings->maxVertexBonesCount > 8) {
			log->error(error = log::eCommandLineInvalidVertexWeight);
			return;
		}
		if (settings->maxNodePartBonesCount < settings->maxVertexBonesCount) {
			log->error(error = log::eCommandLineInvalidBoneCount);
			return;
		}
		if (settings->maxVertexCount < 0 || settings->maxVertexCount > (1<<15)-1) {
			log->error(error = log::eCommandLineInvalidVertexCount);
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
			log->error(error = log::eCommandLineUnknownFiletype, arg);
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