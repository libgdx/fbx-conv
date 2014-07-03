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
	template<typename T, size_t n=1>
	struct OptionalValue {
		bool valid;
		T value[n];
		OptionalValue() : valid(false) {
			for (int i = 0; i < n; ++i)
				value[i] = (T)0;
		}
		inline void unset() {
			valid = false;
		}
		void set(T v0, ...) {
			va_list vl;
			value[0] = v0;
			va_start(vl, v0);
			for (int i = 1; i < n; i++)
				value[i] = va_arg(vl, T);
			va_end(vl);
			valid = true;
		}
		template <typename S, size_t m> void set(const S (&v)[m]) {
			const int c = m > n ? n : m;
			for (int i = 0; i < c; i++)
				value[i] = (T)v[i];
			valid = true;
		}
	};

	template<typename T>
	struct OptionalValue<T, 1> {
		bool valid;
		T value;
		OptionalValue() : valid(false), value((T)0) {}
		inline void unset() {
			valid = false;
		}
		inline void set(T v) {
			valid = true;
			value = v;
		}
	};

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
		OptionalValue<float, 3> diffuse;
		OptionalValue<float, 3> ambient;
		OptionalValue<float, 3> emissive;
		OptionalValue<float, 3> specular;
		OptionalValue<float> shininess;
		OptionalValue<float> opacity;
		std::vector<Texture *> textures;
		
		Material() : source(0), diffuse(), ambient(), emissive(), specular(), shininess(), opacity() { }

		Material(const Material &rhs) : source(rhs.source), id(rhs.id), diffuse(rhs.diffuse),
			ambient(rhs.ambient), emissive(rhs.emissive), specular(rhs.specular), shininess(rhs.shininess), opacity(rhs.opacity)  {	}

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