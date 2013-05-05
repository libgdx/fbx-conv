#ifdef _MSC_VER 
#pragma once
#endif
#ifndef EXPORTER_H
#define EXPORTER_H

#include "JSONWriter.h"
#include "../modeldata/Model.h"

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace writers {
	class G3djWriter {
	public:
		void exportModel(const modeldata::Model *model, JSONWriter *writer) {
			writer->openObject();
			writer->writeStringPair("version", "1.0");
			writer->nextValue(true);
			writer->openArray("meshes");
			for (std::vector<modeldata::Mesh *>::const_iterator itr = model->meshes.begin(); itr != model->meshes.end(); ++itr) {
				if (itr != model->meshes.begin())
					writer->nextValue(true);
				writer->openObject();
				writer->openArray("attributes", false);
				const unsigned int ac = (*itr)->attributes.length();
				for (unsigned int i = 0; i < ac; i++) {
					if (i != 0)
						writer->nextValue(false);
					writer->writeString((*itr)->attributes.name(i));
				}
				writer->closeArray(false);
				writer->nextValue(true);
				writer->openArray("vertices");
				for (unsigned int v = 0; v < (*itr)->vertices.size(); v++) {
					if (v != 0)
						writer->nextValue(v%(*itr)->vertexSize==0);
					writer->writeFloat((*itr)->vertices[v]);
				}
				writer->closeArray();
				if (!(*itr)->parts.empty()) {
					writer->nextValue(true);
					writer->openArray("parts");
					for (std::vector<modeldata::MeshPart *>::const_iterator ptr = (*itr)->parts.begin(); ptr != (*itr)->parts.end(); ++ptr) {
						if (ptr != (*itr)->parts.begin())
							writer->nextValue(true);
						writer->openObject();
						writer->writeStringPair("id", (*ptr)->id.c_str());
						writer->nextValue(true);
						writer->writeStringPair("type", getPrimitiveTypeString((*ptr)->primitiveType));
						writer->nextValue(true);
						writer->openArray("indices");
						for (unsigned int i = 0; i < (*ptr)->indices.size(); i++) {
							if (i > 0)
								writer->nextValue(i%32==0);
							writer->writeInteger((*ptr)->indices[i]);
						}
						writer->closeArray();
						writer->closeObject();
					}
					writer->closeArray();
				}
				writer->closeObject();
			}
			writer->closeArray();
			if (!model->materials.empty()) {
				writer->nextValue(true);
				writer->openArray("materials");
				for (std::vector<modeldata::Material *>::const_iterator itr = model->materials.begin(); itr != model->materials.end(); ++itr) {
					if (itr != model->materials.begin())
						writer->nextValue(true);
					writer->openObject();
					writer->writeStringPair("id", (*itr)->id.c_str());
					if ((*itr)->diffuse[0] != 0.f || (*itr)->diffuse[1] != 0.f || (*itr)->diffuse[2] != 0.f) {
						writer->nextValue(true);
						writer->openArray("diffuse", false);
						writer->writeFloat((*itr)->diffuse[0]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->diffuse[1]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->diffuse[2]);
						writer->closeArray(false);
					}
					if ((*itr)->ambient[0] != 0.f || (*itr)->ambient[1] != 0.f || (*itr)->ambient[2] != 0.f) {
						writer->nextValue(true);
						writer->openArray("ambient", false);
						writer->writeFloat((*itr)->ambient[0]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->ambient[1]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->ambient[2]);
						writer->closeArray(false);
					}
					if ((*itr)->emissive[0] != 0.f || (*itr)->emissive[1] != 0.f || (*itr)->emissive[2] != 0.f) {
						writer->nextValue(true);
						writer->openArray("emissive", false);
						writer->writeFloat((*itr)->emissive[0]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->emissive[1]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->emissive[2]);
						writer->closeArray(false);
					}
					if ((*itr)->opacity != 0.f) {
						writer->nextValue(true);
						writer->writeFloatPair("opacity", (*itr)->opacity);
					}
					if ((*itr)->specular[0] != 0.f || (*itr)->specular[1] != 0.f || (*itr)->specular[2] != 0.f) {
						writer->nextValue(true);
						writer->openArray("specular", false);
						writer->writeFloat((*itr)->specular[0]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->specular[1]);
						writer->nextValue(false);
						writer->writeFloat((*itr)->specular[2]);
						writer->closeArray(false);
					}
					if ((*itr)->shininess != 0.f) {
						writer->nextValue(true);
						writer->writeFloatPair("shininess", (*itr)->shininess);
					}
					if (!(*itr)->textures.empty()) {
						writer->nextValue(true);
						writer->openArray("textures", true);
						for (std::vector<modeldata::Material::Texture *>::const_iterator ttr = (*itr)->textures.begin(); ttr != (*itr)->textures.end(); ++ttr) {
							if (ttr != (*itr)->textures.begin())
								writer->nextValue(true);
							writer->openObject();
							writer->writeStringPair("id", (*ttr)->id.c_str());
							writer->nextValue(true);
							writer->writeStringPair("filename", (*ttr)->path.c_str());
							if ((*ttr)->uvTranslation[0] != 0.f || (*ttr)->uvTranslation[1] != 0.f) {
								writer->nextValue(true);
								writer->openArray("uvtranslation", false);
								writer->writeFloat((*ttr)->uvTranslation[0]);
								writer->nextValue(false);
								writer->writeFloat((*ttr)->uvTranslation[1]);
								writer->closeArray(false);
							}
							if ((*ttr)->uvScale[0] != 1.f || (*ttr)->uvScale[1] != 1.f) {
								writer->nextValue(true);
								writer->openArray("uvscaling", false);
								writer->writeFloat((*ttr)->uvScale[0]);
								writer->nextValue(false);
								writer->writeFloat((*ttr)->uvScale[1]);
								writer->closeArray(false);
							}
							writer->nextValue(true);
							writer->writeStringPair("type", getTextureUseString((*ttr)->usage));
							writer->closeObject();
						}
						writer->closeArray();
					}
					writer->closeObject();
				}
				writer->closeArray();
			}
			writer->nextValue(true);
			writer->openArray("nodes");
			bool first = true;
			for (std::vector<modeldata::Node *>::const_iterator itr = model->nodes.begin(); itr != model->nodes.end(); ++itr) {
				//if ((*itr)->hasPartsRecursive()) {
					if (first)
						first = false;
					else
						writer->nextValue(true);
					exportNode(*itr, writer);
				//}
			}
			writer->closeArray(true);
			//if (!model->animations.empty()) {
				writer->nextValue(true);
				writer->openArray("animations");
				for (std::vector<modeldata::Animation *>::const_iterator itr = model->animations.begin(); itr != model->animations.end(); ++itr) {
					if (itr != model->animations.begin())
						writer->nextValue(true);
					writer->openObject();
					writer->writeStringPair("id", (*itr)->id.c_str());
					writer->nextValue(true);
					writer->openArray("bones", true);
					for (std::vector<modeldata::NodeAnimation *>::const_iterator ntr = (*itr)->nodeAnimations.begin(); ntr != (*itr)->nodeAnimations.end(); ++ntr) {
						if (ntr != (*itr)->nodeAnimations.begin())
							writer->nextValue(true);
						writer->openObject();
						writer->writeStringPair("boneId", (*ntr)->node->id.c_str());
						writer->nextValue(true);
						writer->openArray("keyframe", true);
						for (std::vector<modeldata::Keyframe *>::const_iterator ktr = (*ntr)->keyframes.begin(); ktr != (*ntr)->keyframes.end(); ++ktr) {
							if (ktr != (*ntr)->keyframes.begin())
								writer->nextValue(true);
							writer->openObject();
							writer->writeFloatPair("keytime", (*ktr)->time);
							if ((*ntr)->translate) {
								writer->nextValue(true);
								writer->openArray("translation", false);
								writer->writeFloat((*ktr)->translation[0]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->translation[1]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->translation[2]);
								writer->closeArray(false);
							}
							if ((*ntr)->rotate) {
								writer->nextValue(true);
								writer->openArray("rotation", false);
								writer->writeFloat((*ktr)->rotation[0]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->rotation[1]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->rotation[2]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->rotation[3]);
								writer->closeArray(false);
							}
							if ((*ntr)->scale) {
								writer->nextValue(true);
								writer->openArray("scale", false);
								writer->writeFloat((*ktr)->scale[0]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->scale[1]);
								writer->nextValue(false);
								writer->writeFloat((*ktr)->scale[2]);
								writer->closeArray(false);
							}
							writer->closeObject();
						}
						writer->closeArray(true);
						writer->closeObject();
					}
					writer->closeArray(true);
					writer->closeObject();
				}
				writer->closeArray();
			//}
			writer->closeObject();
		}

		void exportNode(const modeldata::Node *node, JSONWriter *writer) {
			writer->openObject();
			writer->writeStringPair("id", node->id.c_str());
			if (node->transform.translation[0] != 0.f || node->transform.translation[1] != 0.f || node->transform.translation[2] != 0.f) {
				writer->nextValue(true);
				writer->openArray("translation", false);
				writer->writeFloat(node->transform.translation[0]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.translation[1]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.translation[2]);
				writer->closeArray(false);
			}
			if (node->transform.scale[0] != 1.f || node->transform.scale[1] != 1.f || node->transform.scale[2] != 1.f) {
				writer->nextValue(true);
				writer->openArray("scale", false);
				writer->writeFloat(node->transform.scale[0]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.scale[1]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.scale[2]);
				writer->closeArray(false);
			}
			if (node->transform.rotation[0] != 0.f || node->transform.rotation[1] != 0.f || node->transform.rotation[2] != 0.f || node->transform.rotation[3] != 1.f) {
				writer->nextValue(true);
				writer->openArray("rotation", false);
				writer->writeFloat(node->transform.rotation[0]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.rotation[1]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.rotation[2]);
				writer->nextValue(false);
				writer->writeFloat(node->transform.rotation[3]);
				writer->closeArray(false);
			}
			if (!node->parts.empty()) {
				writer->nextValue(true);
				writer->openArray("parts");
				for (std::vector<NodePart *>::const_iterator itr = node->parts.begin(); itr != node->parts.end(); ++itr) {
					if (itr != node->parts.begin())
						writer->nextValue(true);
					writer->openObject();
					writer->writeStringPair("meshpartid", (*itr)->meshPart == NULL ? "" : (*itr)->meshPart->id.c_str());
					writer->nextValue(true);
					writer->writeStringPair("materialid", (*itr)->material == NULL ? "" : (*itr)->material->id.c_str());
					if (!(*itr)->bones.empty()) {
						writer->nextValue(true);
						writer->openArray("bones", false);
						for (std::vector<const modeldata::Node *>::const_iterator ntr = (*itr)->bones.begin(); ntr != (*itr)->bones.end(); ++ntr) {
							if (ntr != (*itr)->bones.begin())
								writer->nextValue(false);
							writer->writeString((*ntr)==0?"":(*ntr)->id.c_str());
						}
						writer->closeArray(false);
					}
					writer->closeObject();
				}
				writer->closeArray(true);
			}
			if (!node->children.empty()) {
				writer->nextValue(true);
				writer->openArray("children");
				for (std::vector<modeldata::Node *>::const_iterator itr = node->children.begin(); itr != node->children.end(); ++itr) {
					writer->nextValue(true);
					exportNode(*itr, writer);
				}
				writer->closeArray(true);
			}
			writer->closeObject();
		}

		const char* getPrimitiveTypeString(const int &primitiveTypeId) const {
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

		const char* getTextureUseString(const int &textureUse) const {
			switch(textureUse){
			case 0:
				return "STANDARD";
			case 1:
				return "SHADOWMAP";
			case 2:
				return "LIGHTMAP";
			case 3:
				return "SPHERICAL_REFLEXION";
			case 4:
				return "SPHERE_REFLEXION";
			case 5:
				return "BUMPMAP";
			default:
				return "UNKNOWN";
			}
		}
	};
} }

#endif //EXPORTER_H