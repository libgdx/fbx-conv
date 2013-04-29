#include "Texture.h"
#include <stdio.h>
#include <string.h>

namespace fbxconv {
	Texture::Texture(){
	}

	Texture::~Texture(){
	}

	void Texture::setId(const char* pId){
	   id = new char[strlen(pId)+1];
	   strcpy(id, pId);
	}

	void Texture::setRelativePath(const char* path){
	   relativePath = new char[strlen(path)+1];
	   strcpy(relativePath, path);
	}

	char* Texture::getId(){
		return id;
	}

	char* Texture::getRelativePath(){
		return relativePath;
	}
};
