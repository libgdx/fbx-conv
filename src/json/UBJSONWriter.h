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
#ifndef JSON_UBJSONWRITER_H
#define JSON_UBJSONWRITER_H

#include <stdio.h>
#include "BaseJSONWriter.h"
#include "UBJSON.h"

namespace json {

class UBJSONWriter : public BaseJSONWriter {
public:
	std::ostream &stream;

	UBJSONWriter(std::ostream &stream) : BaseJSONWriter(), stream(stream) {}
private:
	template<typename T> inline void write(const T &v) { write<T, sizeof(T)>(v); }
	template<typename T, size_t n> void write(const T &v) {
		stream.write(is_bigendian ? (const char*)&v : swap(v), n);
		/*assert(("Data too big", n<=8));
		if (!is_bigendian && n > 1) {
			memcpy(&swapper.data[0], &v, n);
			swapper.swap<n>();
			stream.write((const char*)(&swapper.data[0]), n);
		} else
			stream.write((const char *)&v, n);*/
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
		stream << UBJSON_TYPE_NULL;
	}
	virtual void writeValue(const char * const &value, const bool &iskey = false) {
		size_t len = strlen(value);
		if (len < 256) {
			stream << UBJSON_TYPE_STRING1;
			write((unsigned char)len);
			stream.write(value, len);
		} else {
			stream << UBJSON_TYPE_STRING4;
			write((unsigned int)len);
			stream.write(value, len);
		}
	}
	virtual void writeValue(const bool &value, const bool &iskey = false) {
		stream << (value ? UBJSON_TYPE_TRUE : UBJSON_TYPE_FALSE);
	}
	virtual void writeValue(const char &value, const bool &iskey = false) {
		stream << UBJSON_TYPE_BYTE;
		write(value);
	}
	virtual void writeValue(const short &value, const bool &iskey = false) {
		stream << UBJSON_TYPE_SHORT;
		write(value);
	}
	virtual void writeValue(const int &value, const bool &iskey = false) {
		stream << UBJSON_TYPE_INT;
		write(value);
	}
	virtual void writeValue(const long &value, const bool &iskey = false) {
		stream << UBJSON_TYPE_LONG;
		write(value);
	}
	virtual void writeValue(const float &value, const bool &iskey = false) {
		stream << UBJSON_TYPE_FLOAT;
		write(value);
	}
	virtual void writeValue(const double &value, const bool &iskey = false) {
		stream << UBJSON_TYPE_DOUBLE;
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
		if (count < 255) {
			stream << UBJSON_TYPE_DATA1 << type;
			const unsigned char len = (unsigned char)count;
			write(len);
		} else {
			stream << UBJSON_TYPE_DATA4 << type;
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
