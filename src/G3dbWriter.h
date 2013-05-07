/**
 * File specification:
 *  0.. 3 Identifyer (must be "G3DB")
 *  4.. 5 File version high part
 *  6.. 7 File version low part
 *  8..31 Reserved, must be 0
 * 32..EOF Data blocks, where each block is defined as:
 * 1 byte: SSTTTTTT: 
 * - SS: the size of the next value (00 = 1 byte, 01 = 2 bytes, 10 = 4 bytes, 11 = 8 bytes)
 * - TTTTTT: the block type
 * SS bytes: the size of the (rest of the) block in bytes
 */
#ifndef G3DBWRITER_H
#define G3DBWRITER_H

#include <stdio.h>
#include "G3djFile.h"
#include "G3djMeshPart.h"

#define G3DB_TAG			"G3DB"
#define G3DB_VERSION_HI		0
#define G3DB_VERSION_LO		1

#define SMALL	0x00
#define MEDIUM	0x40
#define LARGE	0x80
#define XLARGE	0xC0

#define SMALL_MAX	(((unsigned long)1 << 7) - 1)
#define MEDIUM_MAX	(((unsigned long)1 << 15) - 1)
#define LARGE_MAX	(((unsigned long)1 << 31) - 1)
#define XLARGE_MAX	(((unsigned long long)1 << 63) - 1)

#define G3DB_TAG_MESH			0x01
#define G3DB_TAG_ID				0x02
#define G3DB_TAG_ATTRIBUTES		0x03
#define G3DB_TAG_VERTICES		0x04
#define G3DB_TAG_MESHPART		0x05
#define G3DB_TAG_TYPE			0x06
#define G3DB_TAG_INDICES		0x07
#define G3DB_TAG_MATERIAL		0x08
#define G3DB_TAG_DIFFUSE		0x09
#define G3DB_TAG_AMBIENT		0x0A
#define G3DB_TAG_EMMISIVE		0x0B
#define G3DB_TAG_OPACITY		0x0C
#define G3DB_TAG_SPECULAR		0x0D
#define G3DB_TAG_SHININESS		0x0E
#define G3DB_TAG_NODE			0x0F
#define G3DB_TAG_TRANSLATE		0x10
#define G3DB_TAG_ROTATE			0x11
#define G3DB_TAG_SCALE			0x12
#define G3DB_TAG_PARTMATERIAL	0x13
#define G3DB_TAG_TEXTURE		0x14
#define G3DB_TAG_FILENAME		0x15
#define G3DB_TAG_ANIMATIONCLIP	0x16
#define G3DB_TAG_BONE			0x17
#define G3DB_TAG_KEYFRAME		0x18
#define G3DB_TAG_TIME			0x19

#define SWAP(X,Y,T) {T=X; X=Y; Y=T;}

namespace fbxconv {
	typedef union {
		short s;
		float f;
		int i;
		long l;
		char data[8];
		void swap2() {
			char tmp;
			SWAP(data[0], data[1], tmp);
		}
		void swap4() {
			char tmp;
			SWAP(data[0], data[3], tmp);
			SWAP(data[1], data[2], tmp);
		}
		void swap8() {
			char tmp;
			SWAP(data[0], data[7], tmp);
			SWAP(data[1], data[6], tmp);
			SWAP(data[2], data[5], tmp);
			SWAP(data[3], data[4], tmp);
		}
	} EndianSwapper;

	class G3dbWriter {
	public:
		static const short versionHi = G3DB_VERSION_HI;
		static const short versionLo = G3DB_VERSION_LO;
	private:
		FILE * file;
		EndianSwapper swapper;

		void writeTag();
		void writeVersion();
		void writeNull(const int &size);
		void writeMesh(const Mesh &mesh);
		void writeMaterial(G3djMaterial &material);
		void writeNode(const G3djNode &node);
		void writeAnimationClip(const AnimationClip &clip);
		void writeFloat(const float &value);
		void writeLong(const long &value);
		void writeInt(const int &value);
		void writeShort(const short &value);
		void writeByte(const unsigned char &value);
		void writeString(const char *value);
		void writeVector(const gameplay::Vector2 &value);
		void writeVector(const gameplay::Vector3 &value);
		void writeVector(const gameplay::Vector4 &value);
		void writeQuaternion(const gameplay::Quaternion &value);
		int writeSize(const unsigned char &tag, const long &size);
		int writeTag(unsigned char tag, const long &size);
		void writeString(const unsigned char &tag, const char *value);
		void writeVector(const unsigned char &tag, const gameplay::Vector2 &value);
		void writeVector(const unsigned char &tag, const gameplay::Vector3 &value);
		void writeQuaternion(const unsigned char &tag, const gameplay::Quaternion &value);
		void writeFloat(const unsigned char &tag, const float &value);
		void writeByte(const unsigned char &tag, const unsigned char &value);
		long *beginBlock(const unsigned char &tag, const long &maxSize);
		void endBlock(const long *block);
	public:
		bool exportG3db(G3djFile &g3dj, const char *fileName);
	};
}

#endif // G3DBWRITER_H