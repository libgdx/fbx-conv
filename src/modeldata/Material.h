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
#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_MATERIAL_H
#define MODELDATA_MATERIAL_H

#include <vector>
#include <fbxsdk.h>
#include "../readers/matrix3.h"
#include "../json/BaseJSONWriter.h"

namespace fbxconv {
namespace modeldata {
	struct Material : public json::ConstSerializable {
		struct Texture : public json::ConstSerializable {
			enum Usage {
				Unknown = 0,
				None = 1,
				Diffuse = 2, 
				Emissive = 3,
				Ambient = 4,
				Specular = 5,
				Shininess = 6,
				Normal = 7,
				Bump = 8,
				Transparency = 9,
				Reflection = 10
			};

			FbxFileTexture *source;
			std::string id;
			std::string path;
			float uvTranslation[2];
			float uvScale[2];
			// FIXME add Matrix3<float> uvTransform;
			Usage usage;

			Texture() : usage(Unknown), source(0) {
				uvTranslation[0] = uvTranslation[1] = 0.f;
				uvScale[0] = uvScale[1] = 1.f;
			}

			virtual void serialize(json::BaseJSONWriter &writer) const;
		};

		FbxSurfaceMaterial *source;
		std::string id;
		float diffuse[3];
		float ambient[3];
		float emissive[3];
		float specular[3];
		float shininess;
		float opacity;
		std::vector<Texture *> textures;
		
		Material() : source(0) {
			memset(diffuse,  0, sizeof(diffuse));
			memset(ambient,  0, sizeof(ambient));
			memset(emissive, 0, sizeof(emissive));
			memset(specular, 0, sizeof(specular));
			shininess = 0.f;
			opacity = 1.f;
		}

		Material(const Material &copyFrom) {
			id = copyFrom.id;
			memcpy(diffuse,  copyFrom.diffuse,  sizeof(diffuse));
			memcpy(ambient,  copyFrom.ambient,  sizeof(diffuse));
			memcpy(emissive, copyFrom.emissive, sizeof(diffuse));
			memcpy(specular, copyFrom.specular, sizeof(diffuse));
			shininess = copyFrom.shininess;
			opacity = copyFrom.opacity;
			source = copyFrom.source;
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

		int getTextureIndex(const Texture * const &texture) const {
			int n = (int)textures.size();
			for (int i = 0; i < n; i++)
				if (textures[i] == texture)
					return i;
			return -1;
		}

		virtual void serialize(json::BaseJSONWriter &writer) const;
	};
}
}

#endif //MODELDATA_MATERIAL_H