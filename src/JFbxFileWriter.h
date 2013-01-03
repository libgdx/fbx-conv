#ifndef JFBXFILEWRITER_H
#define JFBXFILEWRITER_H

#define JSONFORMAT_VERSION	"1.0"

#include <stdio.h>

namespace fbxconv {
	class JFbxFileWriter{
	public:
		JFbxFileWriter() { currentIndention = 0; }

		virtual void openObject() = 0;
		virtual void closeObject() = 0;

		virtual void openArray(const char* key) = 0;
		virtual void closeArray() = 0;

		void indent() { currentIndention++; }
		void dedent() { currentIndention--; if(currentIndention < 0) currentIndention = 0; }

		virtual void nextValue(bool newLine) = 0;
		virtual void newLine() = 0;

		virtual void writeStringPair(const char* key, const char* value) = 0;

		virtual void writeRawString(const char* value) = 0;
		virtual void writeString(const char* value) = 0;
		virtual void writeFloat(float value) = 0;
		virtual void writeInteger(int value) = 0;
	protected:
		int currentIndention;
		FILE *file;

		virtual void printTabs() = 0;
	};
};

#endif