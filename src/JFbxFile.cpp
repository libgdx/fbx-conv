#include "JFbxFile.h"
#include "JFbxJSONWriter.h"

using namespace gameplay;

namespace fbxconv {
	JFbxFile::JFbxFile(){
	}

	JFbxFile::~JFbxFile(){
	}

	void JFbxFile::exportWith(JFbxFileWriter* writer){
		writer->openObject();

		writer->writeStringPair("version", JSONFORMAT_VERSION);
		writer->nextValue(true);

		// Write Meshes
		writer->openArray("meshes");

		// meshes
		for (std::map<std::string, Mesh*>::const_iterator iter = meshes.begin(); iter != meshes.end(); ++iter)
		{
			Mesh *mesh = iter->second;
			writer->openObject();

			// write ID out first
			writer->writeStringPair("id", mesh->getId().c_str());
			writer->nextValue(true);

			// then the attributes
			writer->openArray("attributes");
				writeAttributes(mesh, writer);
			writer->closeArray();
			writer->nextValue(true);

			// then the interleaved vertices
			writer->openArray("vertices");
				writeVertices(mesh, writer);
			writer->closeArray();
			writer->nextValue(true);

			// then the mesh parts (optional)
			if(mesh->parts.size() > 0){
				writer->openArray("parts");
					writeMeshParts(mesh, writer);
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

	void JFbxFile::writeAttributes(Mesh * mesh, JFbxFileWriter* writer){
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

	void JFbxFile::writeVertices(Mesh* mesh, JFbxFileWriter* writer){
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

	void JFbxFile::writeMeshParts(Mesh* mesh, JFbxFileWriter* writer){
		for (std::vector<MeshPart*>::iterator i = mesh->parts.begin(); i != mesh->parts.end(); ++i)
		{
			MeshPart *meshPart = (*i);
			writer->writeStringPair("id", meshPart->getId().c_str());
			writer->nextValue(true);

			writer->writeStringPair("type", getPrimitiveTypeString(meshPart->_primitiveType));
			writer->nextValue(true);

			writer->openArray("indices");
			for(int i=0; i<meshPart->getIndicesCount(); i++){
				writer->writeInteger(meshPart->getIndex(i));
				writer->nextValue(false);
			}
			writer->closeArray();
		}
	}

	const char* JFbxFile::getPrimitiveTypeString(int primitiveTypeId){
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

	void JFbxFile::addScene(Scene* scene){
		this->scene = scene;
	}

	void JFbxFile::addNode(Node* node){
		nodes[node->getId()] = node;
	}

	void JFbxFile::addMesh(Mesh* mesh){
		meshes[mesh->getId()] = mesh;
	}

	Node* JFbxFile::getNode(const char* nodeId){
		return nodes[nodeId];
	}

	Mesh* JFbxFile::getMesh(const char* meshId){
		return meshes[meshId];
	}
};