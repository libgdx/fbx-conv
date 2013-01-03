#include "G3djWriter.h"
#include <sstream>

namespace fbxconv {

	G3djWriter::G3djWriter(){
	
	}

	G3djWriter::~G3djWriter(){
	
	}

	void G3djWriter::exportG3dj(G3djFile *file, const char* filePath){
		writer = new JSONWriter(filePath);

		writer->openObject();

		writer->writeStringPair("version", G3DJ_VERSION);
		writer->nextValue(true);

		// Write Meshes
		writer->openArray("meshes");

		// meshes
		for (unsigned int i=0; i<file->getMeshCount(); i++)
		{
			Mesh *mesh = file->getMesh(i);
			writer->openObject();

			// write ID out first
			writer->writeStringPair("id", mesh->getId().c_str());
			writer->nextValue(true);

			// then the attributes
			writer->openArray("attributes");
				writeAttributes(mesh);
			writer->closeArray();
			writer->nextValue(true);

			// then the interleaved vertices
			writer->openArray("vertices");
				writeVertices(mesh);
			writer->closeArray();
			writer->nextValue(true);

			// then the mesh parts (optional)
			if(mesh->parts.size() > 0){
				writer->openArray("parts");
					writeMeshParts(mesh);
				writer->closeArray();
			}
			writer->closeObject();
			writer->nextValue(true);
		}

		writer->closeArray();
		writer->nextValue(true);

		// Write Materials
		writer->openArray("materials");
		writer->closeArray();
		writer->nextValue(true);

		// Write Nodes
		writer->openArray("nodes");
		writer->closeArray();

		// Main object closing
		writer->closeObject();
	}

	void G3djWriter::writeAttributes(Mesh * mesh){
		if (mesh->getVertexCount() > 0 )
		{
			for(int i=0; i<mesh->getVertexElementCount(); i++){
				if(i>0)
					writer->nextValue(false);
				VertexElement element = mesh->getVertexElement(i);
				writer->writeString(element.usageStr(element.usage));
			}
		}
	}

	void G3djWriter::writeVertices(Mesh* mesh){
		if (mesh->getVertexCount() > 0 )
		{
			for(int i=0; i<mesh->getVertexCount(); i++){
				if(i>0)
					writer->nextValue(false);
				Vertex vertex = mesh->getVertex(i);

				// position
				writer->writeFloat(vertex.position.x); 
				writer->nextValue(false);
				writer->writeFloat(vertex.position.y);
				writer->nextValue(false);
				writer->writeFloat(vertex.position.z);
				
				if(vertex.hasNormal){
					writer->nextValue(false);
					writer->writeFloat(vertex.normal.x);
					writer->nextValue(false);
					writer->writeFloat(vertex.normal.y);
					writer->nextValue(false);
					writer->writeFloat(vertex.normal.z);
				}
				if(vertex.hasTangent){
					writer->nextValue(false);
					writer->writeFloat(vertex.tangent.x);
					writer->nextValue(false);
					writer->writeFloat(vertex.tangent.y);
					writer->nextValue(false);
					writer->writeFloat(vertex.tangent.z);
				}
				if(vertex.hasBinormal){
					writer->nextValue(false);
					writer->writeFloat(vertex.binormal.x);
					writer->nextValue(false);
					writer->writeFloat(vertex.binormal.y);
					writer->nextValue(false);
					writer->writeFloat(vertex.binormal.z);
				}
				for (unsigned int i = 0; i < MAX_UV_SETS; ++i)
				{
					if (vertex.hasTexCoord[i])
					{
						writer->nextValue(false);
						writer->writeFloat(vertex.texCoord[i].x);
						writer->nextValue(false);
						writer->writeFloat(vertex.texCoord[i].y);
					}
				}
				if(vertex.hasDiffuse){
					writer->nextValue(false);
					writer->writeFloat(vertex.diffuse.x);
					writer->nextValue(false);
					writer->writeFloat(vertex.diffuse.y);
					writer->nextValue(false);
					writer->writeFloat(vertex.diffuse.z);
				}
				if(vertex.hasWeights){
					// weights
					writer->nextValue(false);
					writer->writeFloat(vertex.blendWeights.x);
					writer->nextValue(false);
					writer->writeFloat(vertex.blendWeights.y);
					writer->nextValue(false);
					writer->writeFloat(vertex.blendWeights.z);
					writer->nextValue(false);
					writer->writeFloat(vertex.blendWeights.w);
					// indices
					writer->nextValue(false);
					writer->writeFloat(vertex.blendIndices.x);
					writer->nextValue(false);
					writer->writeFloat(vertex.blendIndices.y);
					writer->nextValue(false);
					writer->writeFloat(vertex.blendIndices.z);
					writer->nextValue(false);
					writer->writeFloat(vertex.blendIndices.w);
				}
			}
		}
	}

	void G3djWriter::writeMeshParts(Mesh* mesh){
		int partNumber = 0;

		for (std::vector<MeshPart*>::iterator i = mesh->parts.begin(); i != mesh->parts.end(); ++i)
		{
			MeshPart *meshPart = (*i);

			if(partNumber > 0)
				writer->nextValue(true);

			std::stringstream ss;
			ss << "part" << partNumber;

			writer->openObject();
			writer->writeStringPair("id", ss.str().c_str());
			writer->nextValue(true);

			writer->writeStringPair("type", getPrimitiveTypeString(meshPart->_primitiveType));
			writer->nextValue(true);

			writer->openArray("indices");
			for(int i=0; i<meshPart->getIndicesCount(); i++){
				writer->writeInteger(meshPart->getIndex(i));
				writer->nextValue(false);
			}
			writer->closeArray();
			writer->closeObject();
		}
	}

	const char* G3djWriter::getPrimitiveTypeString(int primitiveTypeId){
		switch(primitiveTypeId){
		case 0:
			return "POINTS";
		case 1:
			return "LINES";
		case 3:
			return "LINE_STRIP";
		case 4:
			return "TRIANGLES";
		case 5:
			return "TRIANGLE_STRIP";
		default:
            return "UNKNOWN";
		}
	}
};