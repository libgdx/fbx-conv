#include "G3djMaterial.h"

namespace fbxconv {
	G3djMaterial::G3djMaterial(const char* pId, unsigned int pMaterialType){
		id = pId;
		materialType = pMaterialType;
	}

	G3djMaterial::~G3djMaterial(){
	
	}

	unsigned int G3djMaterial::getMaterialType() const {
		return materialType;
	}

	std::string G3djMaterial::getId() const {
		return id;
	}

	void G3djMaterial::setDiffuse(float r, float g, float b){
		diffuse.set(r, g, b);
	}

	Vector3 G3djMaterial::getDiffuse() const {
		return diffuse;
	}

	void G3djMaterial::setAmbient(float r, float g, float b){
		ambient.set(r, g, b);
	}

	Vector3 G3djMaterial::getAmbient() const {
		return ambient;
	}

	void G3djMaterial::setEmissive(float r, float g, float b){
		emissive.set(r, g, b);
	}

	Vector3 G3djMaterial::getEmissive() const {
		return emissive;
	}

	void G3djMaterial::setSpecular(float r, float g, float b){
		specular.set(r, g, b);
	}

	Vector3 G3djMaterial::getSpecular() const {
		return specular;
	}

	void G3djMaterial::setOpacity(float a){
		opacity = a;
	}

	float G3djMaterial::getOpacity() const {
		return opacity;
	}

	void G3djMaterial::setShininess(float shininess){
		this->shininess = shininess;
	}

	float G3djMaterial::getShininess() const {
		return shininess;
	}

	void G3djMaterial::addTexture(Texture* texture){
		textures[texture->getId()] = texture;
	}

	unsigned int G3djMaterial::getTextureCount() const {
		return textures.size();
	}

	Texture* G3djMaterial::getTexture(const char* id) {
		std::map<std::string, Texture*>::iterator it = textures.find(id);
		if (it != textures.end())
		{
			return it->second;
		}
		return NULL;
	}

	Texture* G3djMaterial::getTexture(unsigned int index) {
		// Ugh, this seems ugly. But without having a second flat list I don't see a better way right now.
		if(index >= textures.size())
			return NULL;
		std::map<std::string, Texture*>::iterator i( textures.begin() );
		std::advance(i, index);

		return i->second;
	}
};