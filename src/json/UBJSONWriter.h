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

template<size_t n> void swap(char * const &data) {assert(("This shouldnt happen", false));}
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

union Swapper {
	char data[8];
	template<size_t n> void swap() { json::swap<n>(&data[0]); }
};

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
	bool writeOpenData(const char *type, const size_t &count) {
		// NOTE: This breaks the current ubjson specs because we use H as a strong typed container,
		// See: https://github.com/thebuzzmedia/universal-binary-json/issues/27
		if (count < 255) { // 0xFF was previously reserved, so dont include it to be safe
			stream << "h" << type;
			const unsigned char len = (unsigned char)count;
			write(len);
		} else {
			stream << "H" << type;
			const unsigned int len = (unsigned int)count;
			write(len);
		}
		return true;
	}
	template<class T> void writeData(const T * const &values, const size_t &count) {
		for (size_t i = 0; i < count; i++)
			write(values[i]);
	}

	inline virtual bool writeOpenFloatData(const size_t& count) { return writeOpenData("d", count); }
	inline virtual bool writeOpenDoubleData(const size_t& count) { return writeOpenData("D", count); }
	inline virtual bool writeOpenShortData(const size_t& count) { return writeOpenData("i", count); }
	inline virtual bool writeOpenUShortData(const size_t& count) { return writeOpenData("i", count); }
	inline virtual bool writeOpenIntData(const size_t& count) { return writeOpenData("I", count); }
	inline virtual bool writeOpenUIntData(const size_t& count) { return writeOpenData("I", count); }
	inline virtual bool writeOpenLongData(const size_t& count) { return writeOpenData("L", count); }
	inline virtual bool writeOpenULongData(const size_t& count) { return writeOpenData("L", count); }

	inline virtual void writeFloatData(const float * const &values, const size_t &count) { writeData(values, count); }
	inline virtual void writeDoubleData(const double * const &values, const size_t &count) {writeData(values, count); }
	inline virtual void writeShortData(const short * const &values, const size_t &count) {writeData(values, count); }
	inline virtual void writeUShortData(const unsigned short * const &values, const size_t &count) {writeData(values, count); }
	inline virtual void writeIntData(const int * const &values, const size_t &count) {writeData(values, count); }
	inline virtual void writeUIntData(const unsigned int * const &values, const size_t &count) {writeData(values, count); }
	inline virtual void writeLongData(const long * const &values, const size_t &count) {writeData(values, count); }
	inline virtual void writeULongData(const unsigned long * const &values, const size_t &count) {writeData(values, count); }

	virtual void writeCloseData() {}
};

}

#endif //JSON_UBJSONWRITER_H