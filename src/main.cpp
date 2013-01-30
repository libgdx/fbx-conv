#include "FbxConverter.h"
#include "G3djWriter.h"
#include <string>

using namespace fbxconv;
using namespace gameplay;

int main(int argc, const char** argv) {
	if (argc == 1)
	{
		printf("Missing: file to convert\n");
		return 1;
	}
	std::string file = argv[1];

	FbxConverterConfig config = FbxConverterConfig();
	config.flipV = false;

	FbxConverter converter(config);
	G3djFile *g3djFile = converter.load(file.c_str());

	printf("Exporting to json.\n");
	G3djWriter *writer = new G3djWriter();

	// either use the second parameter as output filename, or if there is no
	// second parameter, then use the file/path of the input file with a new
	// "g3dj" extension
	std::string outputFile;
	if (argc == 3)
		outputFile = argv[2];
	else
	{
		size_t extensionStartPos = file.find_last_of('.');
		outputFile = file;
		if (extensionStartPos != std::string::npos)
		{
			outputFile.erase(extensionStartPos, std::string::npos);
			outputFile.append(".g3dj");
		}
		else
			outputFile.append(".g3dj");
	}
	
	writer->exportG3dj(g3djFile, outputFile.c_str());

	printf("Done.\n");

	return 0;
}