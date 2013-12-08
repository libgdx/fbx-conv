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
#ifndef JSON_JSONWRITER_H
#define JSON_JSONWRITER_H

#include "BaseJSONWriter.h"

namespace json {

class JSONWriter : public BaseJSONWriter {
public:
	const char *newline;
	const char *indent;
	const char *openObject;
	const char *closeObject;
	const char *openArray;
	const char *closeArray;
	const char *keySeparator;
	const char *valueSeparator;
public:
	std::ostream &stream;

	JSONWriter(std::ostream &stream) : BaseJSONWriter(), stream(stream), indentCount(0), 
		newline("\r\n"), indent("\t"), openObject("{"), closeObject("}"), openArray("["), closeArray("]"), keySeparator(": "), valueSeparator(", ")
	{}
protected:
	char tmp[256];
	int indentCount;
	void nextline() {
		stream << newline;
		for (int i = 0; i < indentCount; i++)
			stream << indent;
	}
	virtual void writeOpenObject(const bool &varsize, const long long &size, const bool &inl) {
		stream << openObject;
		++indentCount;
		if (!inl)
			nextline();
	}
	virtual void writeCloseObject(const bool &varsize, const long long &size, const bool &inl) {
		if (--indentCount < 0)
			indentCount = 0;
		if (!inl)
			nextline();
		stream << closeObject;
	}
	virtual void writeOpenArray(const bool &varsize, const long long &size, const bool &inl) {
		stream << openArray;
		++indentCount;
		if (!inl)
			nextline();
	}
	virtual void writeCloseArray(const bool &varsize, const long long &size, const bool &inl) {		
		if (--indentCount < 0)
			indentCount = 0;
		if (!inl)
			nextline();
		stream << closeArray;
	}
	virtual void writeNextValue(const bool &first, const bool &nl) {
		if (!first) {
			stream << valueSeparator;
			if (nl)
				nextline();
		}
	}
	virtual void writeNull() {
		stream << "null";
	}
	virtual void writeValue(const char * const &value, const bool &iskey = false) {
		stream << "\"" << value << "\"";
		if (iskey)
			stream << keySeparator;
	}

	virtual void writeValue(const bool &value, const bool &iskey = false) {
		stream << (value ? "true" : "false");
		if (iskey)
			stream << keySeparator;
	}

	virtual void writeValue(const char &value, const bool &iskey = false) {
		writeValue((int)value, iskey);
	}

	virtual void writeValue(const short &value, const bool &iskey = false) {
		writeValue((int)value, iskey);
	}

	virtual void writeValue(const int &value, const bool &iskey = false) {
		sprintf(tmp, "% 3i", value);
		stream << tmp;
		if (iskey)
			stream << keySeparator;
	}

	virtual void writeValue(const long &value, const bool &iskey = false) {
		sprintf(tmp, "% 3i", value);
		stream << tmp;
		if (iskey)
			stream << keySeparator;
	}

	virtual void writeValue(const float &value, const bool &iskey = false) {
		if ((value * 0) != 0) {
			const unsigned int t = (*(unsigned int*)&value) & 0xfeffffff;
			sprintf(tmp, "% 8f", *(float*)&t);
		} else
			sprintf(tmp, "% 8f", value);
		stream << tmp;
		if (iskey)
			stream << keySeparator;
	}

	virtual void writeValue(const double &value, const bool &iskey = false) {
		sprintf(tmp, "% 8f", value);
		stream << tmp;
		if (iskey)
			stream << keySeparator;
	}

	virtual void writeValue(const unsigned char &value, const bool &iskey = false) {
		writeValue((int)value, iskey);
	}
	virtual void writeValue(const unsigned short &value, const bool &iskey = false) {
		writeValue((int)value, iskey);
	}
	virtual void writeValue(const unsigned int &value, const bool &iskey = false) {
		sprintf(tmp, "% 3i", value);
		stream << tmp;
		if (iskey)
			stream << keySeparator;
	}
	virtual void writeValue(const unsigned long &value, const bool &iskey = false) {
		sprintf(tmp, "% 3i", value);
		stream << tmp;
		if (iskey)
			stream << keySeparator;
	}
};

}

#endif //JSON_JSONWRITER_H