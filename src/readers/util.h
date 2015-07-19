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
#endif //_MSC_VER
#ifndef FBXCONV_READERS_UTIL_H
#define FBXCONV_READERS_UTIL_H

#include <vector>
#include <algorithm>
#include <assert.h>

namespace fbxconv {
namespace readers {

	// Index + weight pair for vertex blending
	struct BlendWeight {
		float weight;
		int index;
		BlendWeight() : weight(0.f), index(-1) {}
		BlendWeight(float w, int i) : weight(w), index(i) {}
		inline bool operator<(const BlendWeight& rhs) const {
			return weight < rhs.weight;
		}
		inline bool operator>(const BlendWeight& rhs) const {
			return weight > rhs.weight;
		}
		inline bool operator==(const BlendWeight& rhs) const {
			return weight == rhs.weight;
		}
	};

	// Group of indices for vertex blending
	struct BlendBones {
		int *bones;
		unsigned int capacity;
		BlendBones(const unsigned int &capacity = 2) : capacity(capacity) {
			bones = new int[capacity];
			memset(bones, -1, capacity * sizeof(int));
		}
		BlendBones(const BlendBones &rhs) : capacity(rhs.capacity) {
			bones = new int[capacity];
			memcpy(bones, rhs.bones, capacity * sizeof(int));
		}
		~BlendBones() {
			delete bones;
		}
		inline bool has(const int &bone) const {
			for (unsigned int i = 0; i < capacity; i++)
				if (bones[i] == bone)
					return true;
			return false;
		}
		inline unsigned int size() const {
			for (unsigned int i = 0; i < capacity; i++)
				if (bones[i] < 0)
					return i;
			return capacity;
		}
		inline unsigned int available() const {
			return capacity - size();
		}
		inline int cost(const std::vector<std::vector<BlendWeight>*> &rhs) const {
			int result = 0;
			for (std::vector<std::vector<BlendWeight>*>::const_iterator itr = rhs.begin(); itr != rhs.end(); ++itr)
				for (std::vector<BlendWeight>::const_iterator jtr = (*itr)->begin(); jtr != (*itr)->end(); ++jtr)
					if (!has((*jtr).index))
						result++;
			return (result > (int)available()) ? -1 : result;
		}
		inline void sort() {
			std::sort(bones, bones + size());
		}
		inline int idx(const int &bone) const {
			for (unsigned int i = 0; i < capacity; i++)
				if (bones[i] == bone)
					return i;
			return -1;
		}
		inline int add(const int &v) {
			for (unsigned int i = 0; i < capacity; i++) {
				if (bones[i] == v)
					return i;
				else if (bones[i] < 0) {
					bones[i] = v;
					return i;
				}
			}
			return -1;
		}
		inline bool add(const std::vector<std::vector<BlendWeight>*> &rhs) {
			for (std::vector<std::vector<BlendWeight>*>::const_iterator itr = rhs.begin(); itr != rhs.end(); ++itr)
				for (std::vector<BlendWeight>::const_iterator jtr = (*itr)->begin(); jtr != (*itr)->end(); ++jtr)
					if (add((*jtr).index)<0)
						return false;
			return true;
		}
		inline int operator[](const unsigned int idx) const {
			return idx < capacity ? bones[idx] : -1;
		}
		inline BlendBones &operator=(const BlendBones &rhs) {
			if (&rhs == this)
				return *this;
			if (capacity != rhs.capacity) {
				delete[] bones;
				bones = new int[capacity = rhs.capacity];
			}
			memcpy(bones, rhs.bones, capacity * sizeof(int));
			return *this;
		}
	};

	// Collection of group of indices for vertex blending
	struct BlendBonesCollection {
		std::vector<BlendBones> bones;
		unsigned int bonesCapacity;
		BlendBonesCollection(const unsigned int &bonesCapacity) : bonesCapacity(bonesCapacity) { }
		BlendBonesCollection(const BlendBonesCollection &rhs) : bonesCapacity(rhs.bonesCapacity) {
			bones.insert(bones.begin(), rhs.bones.begin(), rhs.bones.end());
		}
		inline BlendBonesCollection &operator=(const BlendBonesCollection &rhs) {
			if (&rhs == this)
				return (*this);
			bones = rhs.bones;
			bonesCapacity = rhs.bonesCapacity;
			return (*this);
		}
		inline unsigned int size() const {
			return (unsigned int)bones.size();
		}
		inline BlendBones &operator[](const unsigned int &idx) {
			return bones[idx];
		}
		inline unsigned int add(const std::vector<std::vector<BlendWeight>*> &rhs) {
			int cost = (int)bonesCapacity, idx = -1, n = bones.size();
			for (int i = 0; i < n; i++) {
				const int c = bones[i].cost(rhs);
				if (c >= 0 && c < cost) {
					cost = c;
					idx = i;
				}
			}
			if (idx < 0) {
				bones.push_back(BlendBones(bonesCapacity));
				idx = n;
			}
			return bones[idx].add(rhs) ? idx : -1;
		}
		inline void sortBones() {
			for (std::vector<BlendBones>::iterator itr = bones.begin(); itr != bones.end(); ++itr)
				(*itr).sort();
		}
	};

	// Provides information about an animation
	struct AnimInfo {
		float start;
		float stop;
		float framerate;
		bool translate;
		bool rotate;
		bool scale;

		AnimInfo() : start(FLT_MAX), stop(-1.f), framerate(0.f), translate(false), rotate(false), scale(false) {}

		inline AnimInfo& operator+=(const AnimInfo& rhs) {
			start = std::min(rhs.start, start);
			stop = std::max(rhs.stop, stop);
			framerate = std::max(rhs.framerate, framerate);
			translate = translate || rhs.translate;
			rotate = rotate || rhs.rotate;
			scale = scale || rhs.scale;
			return *this;
		}
	};
} }
#endif //FBXCONV_READERS_UTIL_H