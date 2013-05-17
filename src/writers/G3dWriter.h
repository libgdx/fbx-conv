#ifdef _MSC_VER 
#pragma once
#endif
#ifndef FBXCONV_WRITERS_G3DWRITER_H
#define FBXCONV_WRITERS_G3DWRITER_H

#include "../json/BaseJSONWriter.h"
#include "../modeldata/Model.h"

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace writers {

class G3dWriter {
public:
	json::BaseJSONWriter &writer;

	G3dWriter(json::BaseJSONWriter &writer) : writer(writer) { }

	void writeModel(const Model * const &model) {
		writer.obj(6);
		writer << "version" = "1.0";
		writer << "id" = model->id;
		writer.val("meshes").is(); writeMeshes(model->meshes);
		writer << "materials" = model->materials;
		writer << "nodes" = model->nodes;
		writer << "animations" = model->animations;
		writer.end();
	}

	void writeMeshes(const std::vector<Mesh *> &meshes) {
		writer.arr(meshes.size());
		for (std::vector<Mesh *>::const_iterator it = meshes.begin(); it != meshes.end(); ++it)
			writeMesh(*it);
		writer.end();
	}

	void writeMesh(const Mesh * const &mesh) {
		writer.obj(3);
		writer.val("attributes").is(); writeAttributes(mesh->attributes);
		writer.val("vertices").is().data(mesh->vertices, mesh->vertexSize);
		writer.val("parts").is(); writeMeshParts(mesh->parts);
		writer.end();
	}

	void writeAttributes(const Attributes &attributes) {
		const unsigned int len = attributes.length();
		writer.arr(len, 8);
		for (unsigned int i = 0; i < len; i++)
			writer.val(attributes.name(i));
		writer.end();
	}

	void writeMeshParts(const std::vector<MeshPart *> &parts) {
		writer.arr(parts.size());
		for (std::vector<MeshPart *>::const_iterator it = parts.begin(); it != parts.end(); ++it)
			writeMeshPart(*it);
		writer.end();
	}

	void writeMeshPart(const MeshPart * const &part) {
		writer << json::obj(3);
		writer << "id" = part->id;
		writer << "type" = "";// getPrimitiveTypeString(part->primitiveType);
		writer << "indices" = part->indices;
		writer << json::end;
	}


};

} }
#endif //FBXCONV_WRITERS_G3DWRITER_H