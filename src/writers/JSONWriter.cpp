#include "JSONWriter.h"

namespace fbxconv {
namespace writers {
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

	void JSONWriter::openArray(bool newLine) {
		openArray(0, newLine);
	}

	void JSONWriter::openArray(const char* key, bool newLine){
		if (key)
			fprintf(file, "\"%s\" : [", key);
		else
			fprintf(file, "[");
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

	void JSONWriter::writeIntegerPair(const char* key, const long long &value){
		fprintf(file, "\"%s\" : %d", key, value);
	}

	void JSONWriter::writeFloatPair(const char* key, float value){
		fprintf(file, "\"%s\" : %f", key, value);
	}

	void JSONWriter::writeFloat(const double &value){
		/* if(value == 1.0f)
			fprintf(file, "%s", "1.0");
		else if(value == -1.0f)
			fprintf(file, "%s", "-1.0");
		else if(value == 0.0f)
			fprintf(file, "%s", "0.0");
		else */
			fprintf(file, "% 8f", value);
	}

	void JSONWriter::writeInteger(const long long &value){
		fprintf(file, "% 3i", value);
	}

	void JSONWriter::writeHex(const unsigned long &value) {
		fprintf(file, "\"0x%08X\"", value);
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
} }