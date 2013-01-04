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

		const char* id;

		unsigned int textureUse;
		const char* relativePath;
	};
};

#endif