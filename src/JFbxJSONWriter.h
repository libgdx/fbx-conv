#ifndef JFBXJSONWRITER_H
#define JFBXJSONWRITER_H

#include "JFbxFileWriter.h"

namespace fbxconv {
	class JFbxJSONWriter: public JFbxFileWriter {
	public:
		JFbxJSONWriter(const char* fileName);
		~JFbxJSONWriter();

		void openArray(const char* key);
		void closeArray();

		void openObject();
		void closeObject();

		void nextValue(bool newLine);
		void newLine();

		void writeStringPair(const char* key, const char* value);

		void writeRawString(const char* value);
		void writeString(const char* value);
		void writeFloat(float value);
		void writeInteger(int value);

		void printTabs();
	};
}

#endif