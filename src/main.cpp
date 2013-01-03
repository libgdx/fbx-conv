#include "FbxConverter.h"
#include "JFbxJSONWriter.h"

using namespace fbxconv;
using namespace gameplay;

int main(int argc, const char** argv) {
	const char* file = "samples/cube.fbx";

	FbxConverter converter;
	converter.load(file);

	printf("Exporting to json.\n");
	JFbxJSONWriter *jsonWriter = new JFbxJSONWriter("test.json");
	converter.exportWith(jsonWriter);

	delete jsonWriter;

	printf("Done.\n");

	exit(0);
}