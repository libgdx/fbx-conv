#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_MATERIAL_H
#define MODELDATA_MATERIAL_H

#include <vector>

namespace fbxconv {
namespace modeldata {
	struct Material {
		struct Texture {
			std::string id;
			std::string path;
			float uvTranslation[2];
			float uvScale[2];
			float bakeUvTranslation[2];
			float bakeUvScale[2];
			bool bakeUvRotate;
			unsigned int usage;

			Texture() {
				uvTranslation[0] = uvTranslation[1] = bakeUvTranslation[0] = bakeUvTranslation[1] = 0.f;
				uvScale[0] = uvScale[1] = bakeUvScale[0] = bakeUvScale[1] = 1.f;
				bakeUvRotate = false;
				usage = 0;
			}
		};

		std::string id;
		float diffuse[3];
		float ambient[3];
		float emissive[3];
		float specular[3];
		float shininess;
		float opacity;
		std::vector<Texture *> textures;
		
		Material() {
			memset(diffuse,  0, sizeof(diffuse));
			memset(ambient,  0, sizeof(ambient));
			memset(emissive, 0, sizeof(emissive));
			memset(specular, 0, sizeof(specular));
			shininess = opacity = 0.f;
		}

		Material(const Material &copyFrom) {
			id = copyFrom.id;
			memcpy(diffuse,  copyFrom.diffuse,  sizeof(diffuse));
			memcpy(ambient,  copyFrom.ambient,  sizeof(diffuse));
			memcpy(emissive, copyFrom.emissive, sizeof(diffuse));
			memcpy(specular, copyFrom.specular, sizeof(diffuse));
			shininess = copyFrom.shininess;
			opacity = copyFrom.opacity;
		}

		~Material() {
			for (std::vector<Texture *>::iterator itr = textures.begin(); itr != textures.end(); ++itr)
				delete (*itr);
		}

		Texture *getTexture(const char *id) const {
			for (std::vector<Texture *>::const_iterator itr = textures.begin(); itr != textures.end(); ++itr)
				if ((*itr)->id.compare(id)==0)
					return (*itr);
			return NULL;
		}
	};
}
}

#endif //MODELDATA_MATERIAL_H