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
#if defined(_MSC_VER) && defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <stdlib.h>
#include <crtdbg.h>
#endif

#include "FbxConv.h"

#include <fbxsdk.h>
#include "Settings.h"
#include "FbxConvCommand.h"
#include "readers/FbxConverter.h"
#include "json/JSONWriter.h"
#include "json/UBJSONWriter.h"
#include <string>
#include <iostream>
#include <fstream>

#include "log/messages.h"

using namespace fbxconv;
using namespace fbxconv::modeldata;
using namespace fbxconv::readers;



int process(int argc, const char** argv) {
	log::Log log(new log::DefaultMessages(), -1);
	FbxConv conv(&log);
	return conv.execute(argc, argv) ? 0 : 1;
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