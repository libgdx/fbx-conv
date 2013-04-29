#ifdef _MSC_VER 
#pragma once
#endif
#ifndef MODELDATA_ATTRIBUTES_H
#define MODELDATA_ATTRIBUTES_H

#define ATTRIBUTE_UNKNOWN		0
#define ATTRIBUTE_POSITION		1
#define ATTRIBUTE_NORMAL		2
#define ATTRIBUTE_COLOR			3
#define ATTRIBUTE_TANGENT		4
#define ATTRIBUTE_BINORMAL		5
#define ATTRIBUTE_BLENDWEIGHTS	6
#define ATTRIBUTE_BLENDINDICES	7
#define ATTRIBUTE_TEXCOORD0		8
#define ATTRIBUTE_TEXCOORD1		9
#define ATTRIBUTE_TEXCOORD2		10
#define ATTRIBUTE_TEXCOORD3		11
#define ATTRIBUTE_TEXCOORD4		12
#define ATTRIBUTE_TEXCOORD5		13
#define ATTRIBUTE_TEXCOORD6		14
#define ATTRIBUTE_TEXCOORD7		15
#define ATTRIBUTE_COUNT			16

namespace fbxconv {
namespace modeldata {
	static const char * AttributeNames[] = {
		"UNKNOWN", "POSITION", "NORMAL", "COLOR", "TANGENT", "BINORMAL", "BLENDWEIGHTS", "BLENDINDICES", 
		"TEXCOORD0", "TEXCOORD1", "TEXCOORD2", "TEXCOORD3", "TEXCOORD4", "TEXCOORD5", "TEXCOORD6", "TEXCOORD7"
	};
	static const unsigned short AttributeSizes[] = { 4, 3, 3, 4, 3, 3, 4, 4, 2, 2, 2, 2, 2, 2, 2, 2 };

	struct Attributes {
		unsigned long value;

		Attributes() : value(0) {}

		Attributes(const unsigned long &v) : value(v) {}

		Attributes(const Attributes &copyFrom) : value(copyFrom.value) {}

		inline bool operator==(const Attributes& rhs) const {
			return value == rhs.value;
		}

		unsigned int size() const {
			unsigned int result = 0;
			for (unsigned int i = 0; i < ATTRIBUTE_COUNT; i++)
				if (has(i))
					result += AttributeSizes[i];
			return result;
		}

		unsigned int length() const {
			unsigned int result = 0;
			for (unsigned int i = 0; i < ATTRIBUTE_COUNT; i++)
				if (has(i))
					result++;
			return result;
		}

		int get(unsigned int index) const {
			for (unsigned int i = 0; i < ATTRIBUTE_COUNT; i++)
				if (has(i) && index-- <= 0)
					return i;
			return -1;
		}

		const char *name(const unsigned int &index) const {
			const int a = get(index);
			return a < 0 ? 0 : AttributeNames[a];
		}

		void set(const unsigned int &attribute, const bool &v) {
			if (v)
				add(attribute);
			else
				remove(attribute);
		}

		void add(const unsigned int &attribute) {
			value |= (1 << attribute);
		}

		void remove(const unsigned int &attribute) {
			value &= -1 ^ (1 << attribute);
		}

		inline bool has(const unsigned int &attribute) const {
			return (value & (1 << attribute)) != 0;
		}

		inline bool hasPosition() const {
			return has(ATTRIBUTE_POSITION);
		}

		void hasPosition(const bool &v) {
			set(ATTRIBUTE_POSITION, v);
		}

		inline bool hasNormal() const {
			return has(ATTRIBUTE_NORMAL);
		}

		void hasNormal(const bool &v) {
			set(ATTRIBUTE_NORMAL, v);
		}

		inline bool hasColor() const {
			return has(ATTRIBUTE_COLOR);
		}

		void hasColor(const bool &v) {
			set(ATTRIBUTE_COLOR, v);
		}

		inline bool hasTangent() const {
			return has(ATTRIBUTE_TANGENT);
		}

		void hasTangent(const bool &v) {
			set(ATTRIBUTE_TANGENT, v);
		}

		inline bool hasBinormal() const {
			return has(ATTRIBUTE_BINORMAL);
		}

		void hasBinormal(const bool &v) {
			set(ATTRIBUTE_BINORMAL, v);
		}

		inline bool hasBlendWeights() const {
			return has(ATTRIBUTE_BLENDWEIGHTS);
		}

		void hasBlendWeights(const bool &v) {
			set(ATTRIBUTE_BLENDWEIGHTS, v);
		}

		inline bool hasBlendIndices() const {
			return has(ATTRIBUTE_BLENDINDICES);
		}

		void hasBlendIndices(const bool &v) {
			set(ATTRIBUTE_BLENDINDICES, v);
		}

		inline bool hasUV(const unsigned short &uv) const {
			return has(ATTRIBUTE_TEXCOORD0 + uv);
		}

		void hasUV(const unsigned short &uv, const bool &v) {
			set(ATTRIBUTE_TEXCOORD0 + uv, v);
		}
	};
} }

#endif //MODELDATA_ATTRIBUTES_H