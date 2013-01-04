#include "G3djMaterial.h"

namespace fbxconv {
	G3djMaterial::G3djMaterial(const char* pId, unsigned int pMaterialType){
		id = pId;
		materialType = pMaterialType;
	}

	G3djMaterial::~G3djMaterial(){
	
	}

	unsigned int G3djMaterial::getMaterialType(){
		return materialType;
	}

	std::string G3djMaterial::getId(){
		return id;
	}

	void G3djMaterial::setDiffuse(float r, float g, float b){
		diffuse.set(r, g, b);
	}

	Vector3 G3djMaterial::getDiffuse(){
		return diffuse;
	}

	void G3djMaterial::setAmbient(float r, float g, float b){
		ambient.set(r, g, b);
	}

	Vector3 G3djMaterial::getAmbient(){
		return ambient;
	}

	void G3djMaterial::setEmissive(float r, float g, float b){
		emissive.set(r, g, b);
	}

	Vector3 G3djMaterial::getEmissive(){
		return emissive;
	}

	void G3djMaterial::setSpecular(float r, float g, float b){
		specular.set(r, g, b);
	}

	Vector3 G3djMaterial::getSpecular(){
		return specular;
	}

	void G3djMaterial::setOpacity(float a){
		opacity = a;
	}

	float G3djMaterial::getOpacity(){
		return opacity;
	}

	void G3djMaterial::setShininess(float shininess){
		this->shininess = shininess;
	}

	float G3djMaterial::getShininess(){
		return shininess;
	}

	void G3djMaterial::addTexture(Texture* texture){
		textures[texture->id] = texture;
	}

	unsigned int G3djMaterial::getTextureCount(){
		return textures.size();
	}

	Texture* G3djMaterial::getTexture(const char* id){
		std::map<std::string, Texture*>::iterator it = textures.find(id);
		if (it != textures.end())
		{
			return it->second;
		}
		return NULL;
	}

	Texture* G3djMaterial::getTexture(unsigned int index){
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(index >= textures.size())
			return NULL;
		std::map<std::string, Texture*>::iterator i( textures.begin() );
		std::advance(i, index);

		return i->second;
	}
};