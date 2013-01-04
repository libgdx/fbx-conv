#include "JSONWriter.h"

namespace fbxconv {
	JSONWriter::JSONWriter(const char* fileName) {
		currentIndention = 0;

		file = fopen(fileName, "w");
		if (!file)
		{
			printf("Could not open %s for write mode", fileName);
		}
	}

	JSONWriter::~JSONWriter(){
		fclose(file);
	}

	void JSONWriter::newLine(){
		fprintf(file, "\n");
		printTabs();
	}

	void JSONWriter::nextValue(bool newLine){
		fprintf(file, ",");

		if(newLine)
			this->newLine();
	}

	void JSONWriter::openObject(){
		fprintf(file, "{");
		indent();
		newLine();
	}

	void JSONWriter::closeObject(){
		dedent();
		newLine();

		fprintf(file, "}");
	}

	void JSONWriter::openArray(const char* key){
		openArray(key, true);
	}

	void JSONWriter::openArray(const char* key, bool newLine){
		fprintf(file, "\"%s\" : [", key);
		if(newLine)
		{
			indent();
			this->newLine();
		}
	}

	void JSONWriter::closeArray(){
		closeArray(true);
	}

	void JSONWriter::closeArray(bool newLine){
		if(newLine){
			dedent();
			this->newLine();
		}
		fprintf(file, "]");
	}

	void JSONWriter::writeStringPair(const char* key, const char* value){
		fprintf(file, "\"%s\" : \"%s\"", key, value);
	}

	void JSONWriter::writeFloat(float value){
		if(value == 1.0f)
			fprintf(file, "%s", "1.0");
		else if(value == -1.0f)
			fprintf(file, "%s", "-1.0");
		else if(value == 0.0f)
			fprintf(file, "%s", "0.0");
		else
			fprintf(file, "%f", value);
	}

	void JSONWriter::writeInteger(int value){
		fprintf(file, "%i", value);
	}

	void JSONWriter::writeString(const char* value){
		fprintf(file, "\"%s\"", value);
	}

	void JSONWriter::writeRawString(const char* value){
		fprintf(file, "%s", value);
	}

	void JSONWriter::printTabs() {
		for(int i = 0; i < currentIndention; i++)
			fprintf(file, "\t");
	}
};