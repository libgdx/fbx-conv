#include "FbxConverter.h"
#include "G3djWriter.h"

using namespace fbxconv;
using namespace gameplay;

int main(int argc, const char** argv) {
	const char* file = "samples/softimage/cube_anim_2clips_ascii.fbx";

	FbxConverterConfig config = FbxConverterConfig();
	config.flipV = false;

	FbxConverter converter(config);
	G3djFile *g3djFile = converter.load(file);

	printf("Exporting to json.\n");
	G3djWriter *writer = new G3djWriter();
	writer->exportG3dj(g3djFile, "test.g3dj");

	printf("Done.\n");

	exit(0);
}