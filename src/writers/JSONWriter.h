#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <stdio.h>
#include <cmath>
#include <fbxsdk/core/math/fbxmath.h>

namespace fbxconv {
namespace writers {

	class JSONWriter{
	public:
		JSONWriter(const char* filePath);
		~JSONWriter();

		void JSONWriter::openArray(bool newLine = true);
		void openArray(const char* key, bool newLine = true);
		void closeArray();
		void closeArray(bool newLine);

		void openObject();
		void closeObject();

		void nextValue(bool newLine);

		void writeStringPair(const char* key, const char* value);
		void writeIntegerPair(const char* key, const long long &value);
		void writeFloatPair(const char* key, float value);
		void writeFloatPair(const char* key, double value) {
			writeFloatPair(key, (float)value);
		}

		void writeRawString(const char* value);
		void writeString(const char* value);
		void writeFloat(const float &value) {
			writeFloat((double)value);
		}
		void writeFloat(const double &value);
		void writeInteger(const long long &value);
		void writeInteger(const long &value) {
			writeInteger((long long)value); 
		}
		void writeInteger(const int &value) {
			writeInteger((long long)value); 
		}
		void writeInteger(const unsigned long &value) {
			writeInteger((long long)value); 
		}
		void writeInteger(const unsigned int &value) {
			writeInteger((long long)value); 
		}
		void writeHex(const unsigned long &value);
		void writeHex(const unsigned int &value) {
			writeHex((unsigned long)value); 
		}
		void printTabs();
	private:
		FILE* file;
		int currentIndention;

		void newLine();
		void indent() { currentIndention++; }
		void dedent() { currentIndention--; if(currentIndention < 0) currentIndention = 0; }
	};
} }

#endif