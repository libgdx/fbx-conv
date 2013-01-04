#ifndef G3DJWRITER_H
#define G3DJWRITER_H

#include "JSONWriter.h"
#include "G3djFile.h"

#define G3DJ_VERSION "1.0"

namespace fbxconv {
	class G3djWriter {
	public:
		G3djWriter();
		~G3djWriter();

		void exportG3dj(G3djFile *file, const char* filePath);
	private:
		JSONWriter* writer;

		void writeAttributes(Mesh* mesh);
		void writeVertices(Mesh* mesh);
		void writeMeshParts(Mesh* mesh);
		void writeMaterial(G3djMaterial *material);
		void writeNodeRecursive(G3djNode* node);

		const char* getPrimitiveTypeString(int primitiveTypeId);
		const char* getMaterialTypeString(int primitiveTypeId);
		const char* getTextureUseString(int textureUse);
	};
};

#endif