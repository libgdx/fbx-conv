#ifndef FBX_CONVERTER_H
#define FBX_CONVERTER_H

#include <fbxsdk.h>

namespace fbxconv {
	class FbxConverter {
	public:
		FbxConverter();
		~FbxConverter();

		bool load(const char* file);
		void printHierarchy();
		bool write(const char* file);
	private:
		FbxManager* fbxManager;
		FbxScene* fbxScene;
	};
};

#endif