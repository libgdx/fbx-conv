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
#ifndef JSON_UBJSON_H
#define JSON_UBJSON_H

namespace json {

#define UBJSON_TYPE_NULL		'Z'
#define UBJSON_TYPE_TRUE		'T'
#define UBJSON_TYPE_FALSE		'F'
#define UBJSON_TYPE_BYTE		'B'
#define UBJSON_TYPE_SHORT		'i'
#define UBJSON_TYPE_INT			'I'
#define UBJSON_TYPE_LONG		'L'
#define UBJSON_TYPE_FLOAT		'd'
#define UBJSON_TYPE_DOUBLE		'D'
#define UBJSON_TYPE_HUGE1		'h'
#define UBJSON_TYPE_HUGE4		'H'
#define UBJSON_TYPE_STRING1		's'
#define UBJSON_TYPE_STRING4		'S'
#define UBJSON_TYPE_NOOP		'N'
#define UBJSON_TYPE_END			'E'

#define UBJSON_TYPE_DATA1		'a'
#define UBJSON_TYPE_DATA4		'A'

#define SWAP(X,Y,T) {T=X; X=Y; Y=T;}

const int one = 1;
const bool is_bigendian = ( (*(char*)&one) == 0 );

template<size_t n> void swap(char * const &data) {assert(("This shouldnt happen", false));}
template<> inline void swap<1>(char * const &data) {}
template<> inline void swap<2>(char * const &data) {
	static char tmp;
	SWAP(data[0], data[1], tmp);
}
template<> inline void swap<4>(char * const &data) {
	static char tmp;
	SWAP(data[0], data[3], tmp);
	SWAP(data[1], data[2], tmp);
}
template<> inline void swap<8>(char * const &data) {
	static char tmp;
	SWAP(data[0], data[7], tmp);
	SWAP(data[1], data[6], tmp);
	SWAP(data[2], data[5], tmp);
	SWAP(data[3], data[4], tmp);
}

char swap_data[8];
template<typename T, size_t n> struct Swapper {
	static const char *swap(const T &v) {
		assert(("Data too big", n<=8));
		memcpy(&swap_data[0], &v, n);
		json::swap<n>(&swap_data[0]);
		return (const char*)(&swap_data[0]);
	}
};

template<typename T> struct Swapper<T, 1> {
	inline static const char *swap(const T &v) {
		return (const char*)&v;
	}
};

template<typename T> inline const char *swap(const T &v) {
	return Swapper<T, sizeof(T)>::swap(v);
}

}

#endif //JSON_UBJSON_H