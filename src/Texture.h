#ifndef TEXTURE_H
#define TEXTURE_H

#include "gameplay\Vector2.h"

using namespace gameplay;

namespace fbxconv {
	class Texture {
	public:
		Texture();
		~Texture();

		Vector2 uvTranslation;
		Vector2 uvScale;

		unsigned int textureUse;

		void setId(const char* id);
		void setRelativePath(const char* path);

		char* getId();
		char* getRelativePath();
	private:
		char *id;
		char* relativePath;
	};
};

#endif