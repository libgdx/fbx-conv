#include "G3dbWriter.h"

const int one = 1;
#define is_bigendian() ( (*(char*)&one) == 0 )

namespace fbxconv {
	bool G3dbWriter::exportG3db(G3djFile &g3dj, const char *fileName) {
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
			const unsigned int nm = g3dj.getMeshCount();
			for (unsigned int i = 0; i < nm; i++)
				writeMesh(*g3dj.getMesh(i));
			const unsigned int nt = g3dj.getMaterialCount();
			for (unsigned int i = 0; i < nt; i++)
				writeMaterial(*g3dj.getMaterial(i));
			const unsigned int nn = g3dj.getNodeCount();
			for (unsigned int i = 0; i < nn; i++)
				writeNode(*g3dj.getNode(i));
			const unsigned int an = g3dj.getAnimationClipCount();
			for (unsigned int i = 0; i < an; i++)
				writeAnimationClip(*g3dj.getAnimationClip(i));
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

	void G3dbWriter::writeMesh(const Mesh &mesh) {
		unsigned char vtag = G3DB_TAG_VERTICES;

		long *meshBlock = beginBlock(G3DB_TAG_MESH, LARGE_MAX);

		writeString(G3DB_TAG_ID, mesh.getId().c_str());

		unsigned int n = mesh.getVertexElementCount();
		writeTag(G3DB_TAG_ATTRIBUTES, n);
		for (unsigned int i = 0; i < n; i++)
			writeByte(mesh.getVertexElement(i).usage);

		long *verticesBlock = beginBlock(G3DB_TAG_VERTICES, LARGE_MAX);
		n = mesh.getVertexCount();
		for (unsigned int i = 0; i < n; i++) {
			const Vertex vertex = mesh.getVertex(i);
			writeVector(vertex.position);
			if (vertex.hasNormal)
				writeVector(vertex.normal);
			if (vertex.hasTangent)
				writeVector(vertex.tangent);
			if (vertex.hasBinormal)
				writeVector(vertex.binormal);
			for (unsigned int i = 0; i < MAX_UV_SETS; i++)
				if (vertex.hasTexCoord[i])
					writeVector(vertex.texCoord[i]);
			if (vertex.hasDiffuse) {
				writeFloat(vertex.diffuse.x);
				writeFloat(vertex.diffuse.y);
				writeFloat(vertex.diffuse.z);
			}
			if (vertex.hasWeights) {
				writeVector(vertex.blendWeights);
				writeVector(vertex.blendIndices);
			}
		}
		endBlock(verticesBlock);

		n = mesh.parts.size();
		for (unsigned int i = 0; i < n; i++) {
			long *partBlock = beginBlock(G3DB_TAG_MESHPART, LARGE_MAX);
			writeString(G3DB_TAG_ID, mesh.parts[i]->getId().c_str());
			writeTag(G3DB_TAG_TYPE, 1);
			writeByte((unsigned char)mesh.parts[i]->_primitiveType);
			short c = mesh.parts[i]->getIndicesCount();
			writeTag((char)G3DB_TAG_INDICES, c*2);
			for (short j = 0; j < c; j++)
				writeShort(mesh.parts[i]->getIndex(j));
			endBlock(partBlock);
		}

		endBlock(meshBlock);
	}

	void G3dbWriter::writeMaterial(G3djMaterial &material) {
		long *materialBlock = beginBlock(G3DB_TAG_MATERIAL, MEDIUM_MAX);
		writeString(G3DB_TAG_ID, material.getId().c_str());
		writeByte(G3DB_TAG_TYPE, material.getMaterialType());
		writeVector(G3DB_TAG_DIFFUSE, material.getDiffuse());
		writeVector(G3DB_TAG_AMBIENT, material.getAmbient());
		writeVector(G3DB_TAG_EMMISIVE, material.getEmissive());
		writeFloat(G3DB_TAG_OPACITY, material.getOpacity());
		if(material.getMaterialType() == PHONG){
			writeVector(G3DB_TAG_SPECULAR, material.getSpecular());
			writeFloat(G3DB_TAG_SHININESS, material.getShininess());
		}
		unsigned int n = material.getTextureCount();
		for (unsigned int i = 0; i < n; i++) {
			Texture *texture = material.getTexture(i);
			long *textureBlock = beginBlock(G3DB_TAG_TEXTURE, MEDIUM_MAX);
			writeString(G3DB_TAG_ID, texture->getId());
			writeString(G3DB_TAG_FILENAME, texture->getRelativePath());
			writeByte(G3DB_TAG_TYPE, texture->textureUse);
			writeVector(G3DB_TAG_TRANSLATE, texture->uvTranslation);
			writeVector(G3DB_TAG_SCALE, texture->uvScale);
			endBlock(textureBlock);
		}
		endBlock(materialBlock);
	}

	void G3dbWriter::writeNode(const G3djNode &node) {
		long *nodeBlock = beginBlock(G3DB_TAG_NODE, LARGE_MAX);
		writeString(G3DB_TAG_ID, node.getId().c_str());
		writeByte(G3DB_TAG_BONE, node.isJoint() ? 1 : 0);
		writeVector(G3DB_TAG_TRANSLATE, node.getTranslation());
		writeQuaternion(G3DB_TAG_ROTATE, node.getRotation());
		writeVector(G3DB_TAG_SCALE, node.getScale());
		if (node.getModel() != NULL) {
			gameplay::Mesh *mesh = node.getModel()->getMesh();
			writeString(G3DB_TAG_MESH, mesh->getId().c_str());
			const unsigned int n = mesh->parts.size();
			for (unsigned int i = 0; i < n; i++) {
				long *partMaterialBlock = beginBlock(G3DB_TAG_PARTMATERIAL, SMALL_MAX);
				writeString(G3DB_TAG_MESHPART, mesh->parts[i]->getId().c_str());
				writeString(G3DB_TAG_MATERIAL, ((G3djMeshPart*)(mesh->parts[i]))->getMaterialId());
				endBlock(partMaterialBlock);
			}
		}
		if (node.hasChildren()) {
			for (G3djNode* cnode = dynamic_cast<G3djNode*>(node.getFirstChild()); cnode != NULL; cnode = dynamic_cast<G3djNode*>(cnode->getNextSibling())) {
				writeNode(*cnode);
			}
		}
		endBlock(nodeBlock);
	}

	void G3dbWriter::writeAnimationClip(const AnimationClip &clip) {
		long *clipBlock = beginBlock(G3DB_TAG_ANIMATIONCLIP, LARGE_MAX);
		writeString(G3DB_TAG_ID, clip.getClipId());
		const unsigned int n = clip.getAnimationCount();
		for (unsigned int i = 0; i < n; i++) {
			G3djAnimation *animation = clip.getAnimation(i);
			long *boneBlock = beginBlock(G3DB_TAG_BONE, LARGE_MAX);
			writeString(G3DB_TAG_ID, animation->getBoneId());
			const unsigned int kn = animation->getKeyframeCount();
			for (unsigned int j = 0; j < kn; j++) {
				Keyframe *keyframe = animation->getKeyframe(j);
				long *keyframeBlock = beginBlock(G3DB_TAG_KEYFRAME, SMALL_MAX);
				writeFloat(G3DB_TAG_TIME, keyframe->keytime);
				writeVector(G3DB_TAG_TRANSLATE, keyframe->translation);
				writeQuaternion(G3DB_TAG_ROTATE, keyframe->rotation);
				writeVector(G3DB_TAG_SCALE, keyframe->scale);
				endBlock(keyframeBlock);
			}
			endBlock(boneBlock);
		}
		endBlock(clipBlock);
	}

	void G3dbWriter::writeFloat(const float &value) {
		if (!is_bigendian()) {
			swapper.f = value;
			swapper.swap4();
			fwrite((void*)&(swapper.f), 4, 1, file);
		} else
			fwrite((void*)&value, 4, 1, file);
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

	void G3dbWriter::writeVector(const gameplay::Vector2 &value) {
		writeFloat(value.x);
		writeFloat(value.y);
	}

	void G3dbWriter::writeVector(const gameplay::Vector3 &value) {
		writeFloat(value.x);
		writeFloat(value.y);
		writeFloat(value.z);
	}

	void G3dbWriter::writeVector(const gameplay::Vector4 &value) {
		writeFloat(value.x);
		writeFloat(value.y);
		writeFloat(value.z);
		writeFloat(value.w);
	}

	void G3dbWriter::writeQuaternion(const gameplay::Quaternion &value) {
		writeFloat(value.x);
		writeFloat(value.y);
		writeFloat(value.z);
		writeFloat(value.w);
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

	void G3dbWriter::writeVector(const unsigned char &tag, const gameplay::Vector2 &value) {
		writeTag(tag, 2*4);
		writeVector(value);
	}

	void G3dbWriter::writeVector(const unsigned char &tag, const gameplay::Vector3 &value) {
		writeTag(tag, 3*4);
		writeVector(value);
	}

	void G3dbWriter::writeQuaternion(const unsigned char &tag, const gameplay::Quaternion &value) {
		writeTag(tag, 4*4);
		writeQuaternion(value);
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
}