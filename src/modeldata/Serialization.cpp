#include "Node.h"
#include "NodePart.h"
#include "Animation.h"
#include "NodeAnimation.h"
#include "Keyframe.h"
#include "Material.h"
#include "Attributes.h"
#include "MeshPart.h"
#include "Mesh.h"
#include "Model.h"

namespace fbxconv {
namespace modeldata {

static const char* getPrimitiveTypeString(const int &primitiveTypeId) {
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

static const char* getTextureUseString(const Material::Texture::Usage &textureUse) {
	switch(textureUse){
	case Material::Texture::Ambient:
		return "AMBIENT";
	case Material::Texture::Bump:
		return "BUMP";
	case Material::Texture::Diffuse:
		return "DIFFUSE";
	case Material::Texture::Emissive:
		return "EMISSIVE";
	case Material::Texture::None:
		return "NONE";
	case Material::Texture::Normal:
		return "NORMAL";
	case Material::Texture::Reflection:
		return "REFLECTION";
	case Material::Texture::Shininess:
		return "SHININESS";
	case Material::Texture::Specular:
		return "SPECULAR";
	case Material::Texture::Transparency:
		return "TRANSPARENCY";
	default:
		return "UNKNOWN";
	}
}

void Model::serialize(json::BaseJSONWriter &writer) const {
	writer.obj(6);
	writer << "version" = version;
	writer << "id" = id;
	writer << "meshes" = meshes;
	writer << "materials" = materials;
	writer << "nodes" = nodes;
	writer << "animations" = animations;
	writer.end();
}

void Mesh::serialize(json::BaseJSONWriter &writer) const {
	writer.obj(3);
	writer << "attributes" = attributes;
	writer.val("vertices").is().data(vertices, vertexSize);
	writer << "parts" = parts;
	writer.end();
}

void Attributes::serialize(json::BaseJSONWriter &writer) const {
	const unsigned int len = length();
	writer.arr(len, 8);
	for (unsigned int i = 0; i < len; i++)
		writer.val(name(i));
	writer.end();
}

void MeshPart::serialize(json::BaseJSONWriter &writer) const {
	writer.obj(3);
	writer << "id" = id;
	writer << "type" = getPrimitiveTypeString(primitiveType);
	writer.val("indices").is().data(indices, 12);
	writer << json::end;
}

void Material::serialize(json::BaseJSONWriter &writer) const {
	writer << json::obj;
	writer << "id" = id;
	if (ambient[0] != 0.f && ambient[1] != 0.f && ambient[2] != 0.f)
		writer << "ambient" = ambient;
	if (diffuse[0] != 0.f && diffuse[1] != 0.f && diffuse[2] != 0.f)
		writer << "diffuse" = diffuse;
	if (emissive[0] != 0.f && emissive[1] != 0.f && emissive[2] != 0.f)
		writer << "emissive" = emissive;
	if (opacity != 1.f)
		writer << "opacity" = opacity;
	if (specular[0] != 0.f && specular[1] != 0.f && specular[2] != 0.f)
		writer << "specular" = specular;
	if (shininess != 0.f)
		writer << "shininess" = shininess;
	if (!textures.empty())
		writer << "textures" = textures;
	writer << json::end;
}

void Material::Texture::serialize(json::BaseJSONWriter &writer) const {
	writer << json::obj;
	writer << "id" = id;
	writer << "filename" = path;
	if (uvTranslation[0] != 0.f || uvTranslation[1] != 0.f)
		writer << "uvtranslation" = uvTranslation;
	if (uvScale[0] != 1.f || uvScale[1] != 1.f)
		writer << "uvscaling" = uvScale;
	writer << "type" = getTextureUseString(usage);
	writer << json::end;
}

void Node::serialize(json::BaseJSONWriter &writer) const {
	writer << json::obj;
	writer << "id" = id;
	if (transform.rotation[0] != 0. || transform.rotation[1] != 0. || transform.rotation[2] != 0. || transform.rotation[3] != 1.)
		writer << "rotation" = transform.rotation;
	if (transform.scale[0] != 1. || transform.scale[1] != 1. || transform.scale[2] != 1.)
		writer << "scale" = transform.scale;
	if (transform.translation[0] != 0. || transform.translation[1] != 0. || transform.translation[2] != 0.)
		writer << "translation" = transform.translation;
	if (!parts.empty())
		writer << "parts" = parts;
	if (!children.empty())
		writer << "children" = children;
	writer << json::end;
}

void NodePart::serialize(json::BaseJSONWriter &writer) const {
	writer << json::obj;
	writer << "meshpartid" = meshPart->id;
	writer << "materialid" = material->id;
	if (!bones.empty()) {
		writer.val("bones").is().arr(bones.size(), 4);
		for (std::vector<const Node *>::const_iterator it = bones.begin(); it != bones.end(); ++it)
			writer << (*it)->id;
		writer.end();
	}
	if (!uvMapping.empty()) {
		writer.val("uvMapping").is().arr(uvMapping.size(), 16);
		for (std::vector<std::vector<const Material::Texture *>>::const_iterator it = uvMapping.begin(); it != uvMapping.end(); ++it) {
			writer.arr((*it).size(), 16);
			for (std::vector<const Material::Texture *>::const_iterator tt = (*it).begin(); tt != (*it).end(); ++tt)
				writer << material->getTextureIndex(*tt);
			writer.end();
		}
		writer.end();
	}
	writer << json::end;
}

void Animation::serialize(json::BaseJSONWriter &writer) const {
	writer.obj(2);
	writer << "id" = id;
	writer << "bones" = nodeAnimations;
	writer.end();
}

void NodeAnimation::serialize(json::BaseJSONWriter &writer) const {
	writer.obj(2);
	writer << "boneId" = node->id;
	writer << "keyframes" = keyframes;
	writer.end();
}

void Keyframe::serialize(json::BaseJSONWriter &writer) const {
	writer << json::obj;
	writer << "keytime" = time;
	if (hasRotation)
		writer << "rotation" = rotation;
	if (hasScale)
		writer << "scale" = scale;
	if (hasTranslation)
		writer << "translation" = translation;
	writer << json::end;
}

} }