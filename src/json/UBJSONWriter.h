#ifdef _MSC_VER 
#pragma once
#endif
#ifndef JSON_UBJSONWRITER_H
#define JSON_UBJSONWRITER_H

#include <stdio.h>
#include "BaseJSONWriter.h"

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

#define SWAP(X,Y,T) {T=X; X=Y; Y=T;}

namespace json {

const int one = 1;
const bool is_bigendian = ( (*(char*)&one) == 0 );

typedef union {
	char data[8];
	template<size_t n> void swap() { assert(("This shouldnt happen", false)); }
	template<> void swap<2>() {
		static char tmp;
		SWAP(data[0], data[1], tmp);
	}
	template<> void swap<4>() {
		static char tmp;
		SWAP(data[0], data[3], tmp);
		SWAP(data[1], data[2], tmp);
	}
	template<> void swap<8>() {
		static char tmp;
		SWAP(data[0], data[7], tmp);
		SWAP(data[1], data[6], tmp);
		SWAP(data[2], data[5], tmp);
		SWAP(data[3], data[4], tmp);
	}
} Swapper;

class UBJSONWriter : public BaseJSONWriter {
private:
	Swapper swapper;
public:
	std::ostream &stream;

	UBJSONWriter(std::ostream &stream) : BaseJSONWriter(), stream(stream) {}
private:
	template<typename T> inline void write(const T &v) { write<T, sizeof(T)>(v); }
	template<typename T, size_t n> void write(const T &v) {
		assert(("Data too big", n<=8));
		if (!is_bigendian && n > 1) {
			memcpy(&swapper.data[0], &v, n);
			swapper.swap<n>();
			stream.write((const char*)(&swapper.data[0]), n);
		} else
			stream.write((const char *)&v, n);
	}
protected:
	virtual void writeOpenObject(const bool &varsize, const long long &size, const bool &inl) {
		stream << "{";
	}
	virtual void writeCloseObject(const bool &varsize, const long long &size, const bool &inl) {
		stream << "}";
	}
	virtual void writeOpenArray(const bool &varsize, const long long &size, const bool &inl) {
		stream << "[";
	}
	virtual void writeCloseArray(const bool &varsize, const long long &size, const bool &inl) {		
		stream << "]";
	}
	virtual void writeNextValue(const bool &first, const bool &nl) {
	}
	virtual void writeNull() {
		stream << "Z";
	}
	virtual void writeValue(const char * const &value, const bool &iskey = false) {
		size_t len = strlen(value);
		if (len < 256) {
			stream << "s";
			write((unsigned char)len);
			stream.write(value, len);
		} else {
			stream << "S";
			write((unsigned int)len);
			stream.write(value, len);
		}
	}
	virtual void writeValue(const bool &value, const bool &iskey = false) {
		stream << (value ? "T" : "F");
	}
	virtual void writeValue(const char &value, const bool &iskey = false) {
		stream << "B";
		write(value);
	}
	virtual void writeValue(const short &value, const bool &iskey = false) {
		stream << "i";
		write(value);
	}
	virtual void writeValue(const int &value, const bool &iskey = false) {
		stream << "I";
		write(value);
	}
	virtual void writeValue(const long &value, const bool &iskey = false) {
		stream << "L";
		write(value);
	}
	virtual void writeValue(const float &value, const bool &iskey = false) {
		stream << "d";
		write(value);
	}
	virtual void writeValue(const double &value, const bool &iskey = false) {
		stream << "D";
		write(value);
	}
	virtual void writeValue(const unsigned char &value, const bool &iskey = false) {
		writeValue(*(char*)&value, iskey);
	}
	virtual void writeValue(const unsigned short &value, const bool &iskey = false) {
		writeValue(*(short*)&value, iskey);
	}
	virtual void writeValue(const unsigned int &value, const bool &iskey = false) {
		writeValue(*(int*)&value, iskey);
	}
	virtual void writeValue(const unsigned long &value, const bool &iskey = false) {
		writeValue(*(long*)&value, iskey);
	}
};

}

#endif //JSON_UBJSONWRITER_H