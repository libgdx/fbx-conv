#ifndef MATERIAL_H
#define MATERIAL_H

#include <stdio.h>
#include <map>

#include "gameplay/Vector3.h"
#include "gameplay/Vector4.h"
#include "Texture.h"

using namespace gameplay;

namespace fbxconv {
	enum MATERIALTYPE {
		LAMBERT = 0,
		PHONG = 1
	};

	class G3djMaterial {
	public:
		G3djMaterial(const char* pId, unsigned int pMaterialType);
		~G3djMaterial();

		std::string getId();

		unsigned int getMaterialType();

		void setDiffuse(float r, float g, float b);
		void setAmbient(float r, float g, float b);
		void setEmissive(float r, float g, float b);
		void setSpecular(float r, float g, float b);
		void setOpacity(float a);
		void setShininess(float shininess);

		Vector3 getDiffuse();
		Vector3 getAmbient();
		Vector3 getEmissive();
		Vector3 getSpecular();
		float getOpacity();
		float getShininess();

		void addTexture(Texture *texture);

		Texture* getTexture(const char* id);
		Texture* getTexture(unsigned int index);

		unsigned int getTextureCount();

	private:
		std::string id;
		unsigned int materialType;
		
		Vector3 diffuse;
		Vector3 ambient;
		Vector3 emissive;
		Vector3 specular;
		float shininess;
		float opacity;

		std::map<std::string, Texture*> textures;
	};
};

#endif