#include "JFbxJSONWriter.h"

namespace fbxconv {
	JFbxJSONWriter::JFbxJSONWriter(const char* fileName) : JFbxFileWriter() {
		file = fopen(fileName, "w");
		if (!file)
		{
			printf("Could not open %s for write mode", fileName);
		}
	}

	JFbxJSONWriter::~JFbxJSONWriter(){
		fclose(file);
	}

	void JFbxJSONWriter::newLine(){
		fprintf(file, "\n");
		printTabs();
	}

	void JFbxJSONWriter::nextValue(bool newLine){
		fprintf(file, ",");

		if(newLine)
			this->newLine();
	}

	void JFbxJSONWriter::openObject(){
		fprintf(file, "{");
		indent();
		newLine();
	}

	void JFbxJSONWriter::closeObject(){
		dedent();
		newLine();

		fprintf(file, "}");
	}

	void JFbxJSONWriter::openArray(const char* key){
		fprintf(file, "\"%s\" : [", key);
		indent();
		newLine();
	}

	void JFbxJSONWriter::closeArray(){
		dedent();
		newLine();
		fprintf(file, "]");
	}

	void JFbxJSONWriter::writeStringPair(const char* key, const char* value){
		fprintf(file, "\"%s\" : \"%s\"", key, value);
	}

	void JFbxJSONWriter::writeFloat(float value){
		fprintf(file, "%f", value);
	}

	void JFbxJSONWriter::writeInteger(int value){
		fprintf(file, "%i", value);
	}

	void JFbxJSONWriter::writeString(const char* value){
		fprintf(file, "\"%s\"", value);
	}

	void JFbxJSONWriter::writeRawString(const char* value){
		fprintf(file, "%s", value);
	}

	void JFbxJSONWriter::printTabs() {
		for(int i = 0; i < currentIndention; i++)
			fprintf(file, "\t");
	}
};