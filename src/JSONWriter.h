#ifndef JSONWRITER_H
#define JSONWRITER_H

#include <stdio.h>

namespace fbxconv {

	class JSONWriter{
	public:
		JSONWriter(const char* filePath);
		~JSONWriter();

		void openArray(const char* key);
		void openArray(const char* key, bool newLine);
		void closeArray();
		void closeArray(bool newLine);

		void openObject();
		void closeObject();

		void nextValue(bool newLine);

		void writeStringPair(const char* key, const char* value);

		void writeRawString(const char* value);
		void writeString(const char* value);
		void writeFloat(float value);
		void writeInteger(int value);

		void printTabs();
	private:
		FILE* file;
		int currentIndention;

		void newLine();
		void indent() { currentIndention++; }
		void dedent() { currentIndention--; if(currentIndention < 0) currentIndention = 0; }
	};
}

#endif