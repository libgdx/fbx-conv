#include "G3dbWriter.h"

const int one = 1;
#define is_bigendian() ( (*(char*)&one) == 0 )

namespace fbxconv {
namespace writers {
	bool G3dbWriter::exportG3db(Model *model, const char *fileName) {
		file = fopen(fileName, "wb");
		if (!file) {
			printf("Could not open %s for write mode", fileName);
			return false;
		}
		bool result = true;
		try {
			writeTag();
			writeVersion();
			writeNull(32-8);
			for (std::vector<Mesh *>::const_iterator itr = model->meshes.begin(); itr != model->meshes.end(); itr++)
				writeMesh(*itr);
			for (std::vector<Material *>::const_iterator itr = model->materials.begin(); itr != model->materials.end(); itr++)
				writeMaterial(*itr);
			for (std::vector<Node *>::const_iterator itr = model->nodes.begin(); itr != model->nodes.end(); itr++)
				writeNode(*itr);
			for (std::vector<Animation *>::const_iterator itr = model->animations.begin(); itr != model->animations.end(); itr++)
				writeAnimation(*itr);
		}
		catch(const char *err) {
			printf("%s\n", err);
			result = false;
		}
		fclose(file);
		return result;
	}

	void G3dbWriter::writeTag() {
		fputs(G3DB_TAG, file);
	}

	void G3dbWriter::writeVersion() {
		writeShort(versionHi);
		writeShort(versionLo);
	}

	void G3dbWriter::writeNull(const int &size) {
		for (int i = 0; i < size; i++)
			writeByte(0);
	}

	void G3dbWriter::writeMesh(const Mesh *mesh) {
		long *meshBlock = beginBlock(G3DB_TAG_MESH, LARGE_MAX);

		const unsigned int n = mesh->attributes.length();
		writeTag(G3DB_TAG_ATTRIBUTES, n);
		for (unsigned int i = 0; i < n; i++)
			writeByte((unsigned char)mesh->attributes.get(i));

		writeTag(G3DB_TAG_VERTICES, mesh->vertices.size() * sizeof(float));
		for (std::vector<float>::const_iterator itr = mesh->vertices.begin(); itr != mesh->vertices.end(); ++itr)
			writeFloat(*itr);

		for (std::vector<MeshPart *>::const_iterator itr = mesh->parts.begin(); itr != mesh->parts.end(); ++itr) {
			long *partBlock = beginBlock(G3DB_TAG_MESHPART, LARGE_MAX);
			writeString(G3DB_TAG_ID, (*itr)->id.c_str());
			writeByte(G3DB_TAG_TYPE, (*itr)->primitiveType);
			writeTag((char)G3DB_TAG_INDICES, (*itr)->indices.size() * 2);
			for (std::vector<unsigned short>::const_iterator str = (*itr)->indices.begin(); str != (*itr)->indices.end(); str++) 
				writeShort((short)(*str));
			endBlock(partBlock);
		}

		endBlock(meshBlock);
	}

	void G3dbWriter::writeMaterial(const Material *material) {
		long *materialBlock = beginBlock(G3DB_TAG_MATERIAL, MEDIUM_MAX);
		writeString(G3DB_TAG_ID, material->id.c_str());

		if (material->diffuse[0] != 0.f || material->diffuse[1] != 0.f || material->diffuse[2] != 0.f)
			writeFloat(G3DB_TAG_DIFFUSE, material->diffuse, 3);
		if (material->ambient[0] != 0.f || material->ambient[1] != 0.f || material->ambient[2] != 0.f)
			writeFloat(G3DB_TAG_AMBIENT, material->ambient, 3);
		if (material->emissive[0] != 0.f || material->emissive[1] != 0.f || material->emissive[2] != 0.f)
			writeFloat(G3DB_TAG_EMISSIVE, material->emissive, 3);
		if (material->opacity != 0.f)
			writeFloat(G3DB_TAG_OPACITY, material->opacity);
		if (material->specular[0] != 0.f || material->specular[1] != 0.f || material->specular[2] != 0.f)
			writeFloat(G3DB_TAG_SPECULAR, material->specular, 3);
		if (material->shininess != 0.f)
			writeFloat(G3DB_TAG_SHININESS, material->shininess);

		for (std::vector<Material::Texture *>::const_iterator itr = material->textures.begin(); itr != material->textures.end(); ++itr) {
			long *textureBlock = beginBlock(G3DB_TAG_TEXTURE, MEDIUM_MAX);
			writeString(G3DB_TAG_ID, (*itr)->id.c_str());
			writeString(G3DB_TAG_FILENAME, (*itr)->path.c_str());
			writeByte(G3DB_TAG_TYPE, (*itr)->usage);
			writeFloat(G3DB_TAG_TRANSLATE, (*itr)->uvTranslation, 2);
			writeFloat(G3DB_TAG_SCALE, (*itr)->uvScale, 2);
			endBlock(textureBlock);
		}
		endBlock(materialBlock);
	}

	void G3dbWriter::writeNode(const Node *node) {
		long *nodeBlock = beginBlock(G3DB_TAG_NODE, LARGE_MAX);
		writeString(G3DB_TAG_ID, node->id.c_str());
		if (node->transform.translation[0] != 0.f || node->transform.translation[1] != 0.f || node->transform.translation[2] != 0.f)
			writeFloat(G3DB_TAG_TRANSLATE, node->transform.translation, 3);
		if (node->transform.rotation[0] != 0.f || node->transform.rotation[1] != 0.f || node->transform.rotation[2] != 0.f || node->transform.rotation[3] != 1.f)
			writeFloat(G3DB_TAG_ROTATE, node->transform.rotation, 4);
		if (node->transform.scale[0] != 0.f || node->transform.scale[1] != 1.f || node->transform.scale[2] != 1.f)
			writeFloat(G3DB_TAG_SCALE, node->transform.scale, 3);

		for (std::vector<NodePart *>::const_iterator itr = node->parts.begin(); itr != node->parts.end(); ++itr) {
			long *partMaterialBlock = beginBlock(G3DB_TAG_NODEPART, MEDIUM_MAX);
			writeString(G3DB_TAG_MESHPART, (*itr)->meshPart->id.c_str());
			writeString(G3DB_TAG_MATERIAL, (*itr)->material->id.c_str());
			for (std::vector<const Node *>::const_iterator ntr = (*itr)->bones.begin(); ntr != (*itr)->bones.end(); ++ntr)
				writeString(G3DB_TAG_BONE, (*ntr)->id.c_str());
			endBlock(partMaterialBlock);
		}

		for (std::vector<Node *>::const_iterator itr = node->children.begin(); itr != node->children.end(); ++itr)
			writeNode(*itr);

		endBlock(nodeBlock);
	}

	void G3dbWriter::writeAnimation(const Animation *animation) {
		long *animBlock = beginBlock(G3DB_TAG_ANIMATION, LARGE_MAX);
		writeString(G3DB_TAG_ID, animation->id.c_str());

		for (std::vector<NodeAnimation *>::const_iterator itr = animation->nodeAnimations.begin(); itr != animation->nodeAnimations.end(); ++itr) {
			long *nodeBlock = beginBlock(G3DB_TAG_NODEANIMATION, LARGE_MAX);
			writeString(G3DB_TAG_NODE, (*itr)->node->id.c_str());
			for (std::vector<Keyframe *>::const_iterator ktr = (*itr)->keyframes.begin(); ktr != (*itr)->keyframes.end(); ++ktr) {
				long *keyframeBlock = beginBlock(G3DB_TAG_KEYFRAME, SMALL_MAX);
				writeFloat(G3DB_TAG_TIME, (float)((*ktr)->time));
				if ((*itr)->translate)
					writeFloat(G3DB_TAG_TRANSLATE, (*ktr)->translation, 3);
				if ((*itr)->rotate)
					writeFloat(G3DB_TAG_ROTATE, (*ktr)->rotation, 4);
				if ((*itr)->scale)
					writeFloat(G3DB_TAG_SCALE, (*ktr)->scale, 3);
				endBlock(keyframeBlock);
			}
			endBlock(nodeBlock);
		}
		endBlock(animBlock);
	}

	void G3dbWriter::writeFloat(const float &value) {
		if (!is_bigendian()) {
			swapper.f = value;
			swapper.swap4();
			fwrite((void*)&(swapper.f), 4, 1, file);
		} else
			fwrite((void*)&value, 4, 1, file);
	}

	void G3dbWriter::writeFloat(const float *arr, const unsigned int &size) {
		for (unsigned int i = 0; i < size; i++)
			writeFloat(arr[i]);
	}

	void G3dbWriter::writeFloat(const double *arr, const unsigned int &size) {
		for (unsigned int i = 0; i < size; i++)
			writeFloat((float)arr[i]);
	}

	void G3dbWriter::writeInt(const int &value) {
		if (!is_bigendian()) {
			swapper.i = value;
			swapper.swap4();
			fwrite((void*)&(swapper.i), 4, 1, file);
		} else
			fwrite((void*)&value, 4, 1, file);
	}

	void G3dbWriter::writeLong(const long &value) {
		if (!is_bigendian()) {
			swapper.l = value;
			swapper.swap8();
			fwrite((void*)&(swapper.l), 8, 1, file);
		} else
			fwrite((void*)&value, 8, 1, file);
	}

	void G3dbWriter::writeShort(const short &value) {
		if (!is_bigendian()) {
			swapper.s = value;
			swapper.swap2();
			fwrite((void*)&(swapper.s), 2, 1, file);
		} else
			fwrite((void*)&value, 2, 1, file);
	}

	void G3dbWriter::writeByte(const unsigned char &value) {
		fwrite((void*)&value, 1, 1, file);
	}

	void G3dbWriter::writeString(const char *value) {
		fputs(value, file);
	}

	int G3dbWriter::writeSize(const unsigned char &tag, const long &size) {
		if ((tag & XLARGE) == SMALL) {
			writeByte((unsigned char)size);
			return 1;
		} else if ((tag & XLARGE) == MEDIUM) {
			writeShort((short)size);
			return 2;
		} else if ((tag & XLARGE) == LARGE) {
			writeInt(size);
			return 4;
		} else {
			writeLong(size);
			return 8;
		}
	}

	int G3dbWriter::writeTag(unsigned char tag, const long &size) {
		if (size <= SMALL_MAX)
			tag |= SMALL;
		else if (size <= MEDIUM_MAX)
			tag |= MEDIUM;
		else if (size <= LARGE_MAX)
			tag |= LARGE;
		else
			tag |= XLARGE;
		writeByte(tag);
		return 1 + writeSize(tag, size);
	}

	void G3dbWriter::writeString(const unsigned char &tag, const char *value) {
		writeTag(tag, strlen(value));
		writeString(value);
	}

	void G3dbWriter::writeFloat(const unsigned char &tag, const float *arr, const unsigned int &size) {
		writeTag(tag, size*4);
		writeFloat(arr, size);
	}

	void G3dbWriter::writeFloat(const unsigned char &tag, const double *arr, const unsigned int &size) {
		writeTag(tag, size*4);
		writeFloat(arr, size);
	}

	void G3dbWriter::writeFloat(const unsigned char &tag, const float &value) {
		writeTag(tag, 4);
		writeFloat(value);
	}

	void G3dbWriter::writeByte(const unsigned char &tag, const unsigned char &value) {
		writeTag(tag, 1);
		writeByte(value);
	}

	long *G3dbWriter::beginBlock(const unsigned char &tag, const long &maxSize) {
		long *block = new long[2];
		block[0] = writeTag(tag, maxSize) - 1;
		block[1] = ftell(file);
		return block;
	}

	void G3dbWriter::endBlock(const long *block) {
		long pos = ftell(file);
		long size = pos - block[1];
		fseek(file, block[1] - block[0], SEEK_SET);
		if (block[0] == 1) {
			if (size > SMALL_MAX)
				throw "Data overflow detected";
			writeByte((unsigned char)size);
		} else if (block[0] == 2) {
			if (size > MEDIUM_MAX)
				throw "Data overflow detected";
			writeShort((short)size);
		} else if (block[0] == 4) {
			if (size > LARGE_MAX)
				throw "Data overflow detected";
			writeInt(size);
		} else {
			if (size > XLARGE_MAX)
				throw "Data overflow detected";
			writeLong(size);
		}
		fseek(file, pos, SEEK_SET);
		delete[] block;
	}
} }