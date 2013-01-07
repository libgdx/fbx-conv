#include "G3djWriter.h"
#include "G3djMeshPart.h"
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
			if(i>0)
				writer->nextValue(true);

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
		}

		writer->closeArray();
		writer->nextValue(true);

		// Write Materials
		if(file->getMaterialCount() > 0){
			writer->openArray("materials");

			for(unsigned int i=0; i<file->getMaterialCount(); i++){
				G3djMaterial* material = file->getMaterial(i);
			
				if(i>0)
					writer->nextValue(true);

				writeMaterial(material);
			}

			writer->closeArray();
			writer->nextValue(true);
		}

		// Write Nodes
		writer->openArray("nodes");

		for(unsigned int i=0; i<file->getNodeCount(); i++){
			G3djNode* node = file->getNode(i);

			if(i>0)
				writer->nextValue(true);

			writeNodeRecursive(node);
		}

		writer->closeArray();
		writer->nextValue(true);

		// Write Animations
		writer->openArray("animations");

		for(unsigned int i=0; i<file->getAnimationClipCount(); i++){
			AnimationClip* animationClip = file->getAnimationClip(i);

			if(i>0)
				writer->nextValue(true);

			writeAnimationClip(animationClip);
		}

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

			writer->openObject();
			writer->writeStringPair("id", meshPart->getId().c_str());
			writer->nextValue(true);

			writer->writeStringPair("type", getPrimitiveTypeString(meshPart->_primitiveType));
			writer->nextValue(true);

			writer->openArray("indices");
			for(int i=0; i<meshPart->getIndicesCount(); i++){
				if(i>0)
					writer->nextValue(false);
				writer->writeInteger(meshPart->getIndex(i));				
			}
			writer->closeArray();
			writer->closeObject();
		}
	}

	void G3djWriter::writeMaterial(G3djMaterial* material){
		writer->openObject();

		writer->writeStringPair("id", material->getId().c_str());
		writer->nextValue(true);

		writer->writeStringPair("type", getMaterialTypeString(material->getMaterialType()));
		writer->nextValue(true);

		writer->openArray("diffuse", false);
			writer->writeFloat(material->getDiffuse().x);
			writer->nextValue(false);
			writer->writeFloat(material->getDiffuse().y);
			writer->nextValue(false);
			writer->writeFloat(material->getDiffuse().z);
		writer->closeArray(false);
		writer->nextValue(true);

		writer->openArray("ambient", false);
			writer->writeFloat(material->getAmbient().x);
			writer->nextValue(false);
			writer->writeFloat(material->getAmbient().y);
			writer->nextValue(false);
			writer->writeFloat(material->getAmbient().z);
		writer->closeArray(false);
		writer->nextValue(true);

		writer->openArray("emissive", false);
			writer->writeFloat(material->getEmissive().x);
			writer->nextValue(false);
			writer->writeFloat(material->getEmissive().y);
			writer->nextValue(false);
			writer->writeFloat(material->getEmissive().z);
		writer->closeArray(false);
		writer->nextValue(true);

		writer->writeFloatPair("opacity", material->getOpacity());

		if(material->getMaterialType() == MATERIALTYPE::PHONG){
			writer->nextValue(true);

			writer->openArray("specular", false);
				writer->writeFloat(material->getSpecular().x);
				writer->nextValue(false);
				writer->writeFloat(material->getSpecular().y);
				writer->nextValue(false);
				writer->writeFloat(material->getSpecular().z);
			writer->closeArray(false);
			writer->nextValue(true);

			writer->writeFloatPair("shininess", material->getShininess());
		}

		if(material->getTextureCount() > 0){
			writer->nextValue(true);
			writer->openArray("textures", true);

			for(int i=0; i<material->getTextureCount(); i++){
				Texture* texture = material->getTexture(i);

				if(i>0)
					writer->nextValue(true);

				writer->openObject();

				writer->writeStringPair("id", texture->getId());
				writer->nextValue(true);
				writer->writeStringPair("filename", texture->getRelativePath());
				writer->nextValue(true);

				writer->openArray("uvtranslation", false);
					writer->writeFloat(texture->uvTranslation.x);
					writer->nextValue(false);
					writer->writeFloat(texture->uvTranslation.y);
				writer->closeArray(false);
				writer->nextValue(true);

				writer->openArray("uvscaling", false);
					writer->writeFloat(texture->uvScale.x);
					writer->nextValue(false);
					writer->writeFloat(texture->uvScale.y);
				writer->closeArray(false);
				writer->nextValue(true);

				writer->writeStringPair("type", getTextureUseString(texture->textureUse));

				writer->closeObject();
			}

			writer->closeArray();
		}

		writer->closeObject();
	}

	void G3djWriter::writeNodeRecursive(G3djNode* node){
		writer->openObject();

		writer->writeStringPair("id", node->getId().c_str());
		writer->nextValue(true);
		
		writer->openArray("translation", false);
			Vector3 translation = node->getTranslation();

			writer->writeFloat(translation.x);
			writer->nextValue(false);
			writer->writeFloat(translation.y);
			writer->nextValue(false);
			writer->writeFloat(translation.z);
		writer->closeArray(false);
		writer->nextValue(true);

		writer->openArray("rotation", false);
			Quaternion rotation = node->getRotation();

			writer->writeFloat(rotation.x);
			writer->nextValue(false);
			writer->writeFloat(rotation.y);
			writer->nextValue(false);
			writer->writeFloat(rotation.z);
			writer->nextValue(false);
			writer->writeFloat(rotation.w);
		writer->closeArray(false);
		writer->nextValue(true);

		writer->openArray("scale", false);
			Vector3 scale = node->getScale();

			writer->writeFloat(scale.x);
			writer->nextValue(false);
			writer->writeFloat(scale.y);
			writer->nextValue(false);
			writer->writeFloat(scale.z);
		writer->closeArray(false);

		if(node->getModel() != NULL){
			writer->nextValue(true);
			writer->writeStringPair("mesh", node->getModel()->getMesh()->getId().c_str());

			if(node->getModel()->getMesh()->parts.size() > 0){
				writer->nextValue(true);

				writer->openArray("materials", true);

				Mesh* mesh = node->getModel()->getMesh();
				for (std::vector<MeshPart*>::iterator i = mesh->parts.begin(); i != mesh->parts.end(); ++i)
				{
					G3djMeshPart* meshPart = dynamic_cast<G3djMeshPart*>(*i);
					if(i != mesh->parts.begin())
						writer->nextValue(true);

					writer->openObject();

					writer->writeStringPair("meshpartid", meshPart->getId().c_str());
					writer->nextValue(true);
					writer->writeStringPair("materialid", meshPart->getMaterialId()); 

					writer->closeObject();
				}

				writer->closeArray(true);
			}
		}
		
		if(node->hasChildren()){
			writer->nextValue(true);

			writer->openArray("children", true);

			// children
			for (G3djNode* cnode = dynamic_cast<G3djNode*>(node->getFirstChild()); cnode != NULL; cnode = dynamic_cast<G3djNode*>(cnode->getNextSibling()))
			{
				if(cnode != node->getFirstChild())
					writer->nextValue(true);
				writeNodeRecursive(cnode);
			}

			writer->closeArray();
		}

		writer->closeObject();
	}

	void G3djWriter::writeAnimationClip(AnimationClip* animationClip){
		writer->openObject();

		writer->writeStringPair("id", animationClip->getClipId());
		writer->nextValue(true);
		writer->openArray("bones", true);

		for(int i=0; i<animationClip->getAnimationCount(); i++){
			G3djAnimation* animation = animationClip->getAnimation(i);

			if(i>0)
				writer->nextValue(true);

			writer->openObject();
			writer->writeStringPair("boneId", animation->getBoneId());
			writer->nextValue(true);

			writer->openArray("keyframes", true);

			for(int j=0; j<animation->getKeyframeCount(); j++){
				Keyframe* keyframe = animation->getKeyframe(j);

				if(j>0)
					writer->nextValue(true);

				writer->openObject();

				writer->writeFloatPair("keytime", keyframe->keytime);
				writer->nextValue(true);

				writer->openArray("translation", false);
				writer->writeFloat(keyframe->translation.x);
				writer->nextValue(false);
				writer->writeFloat(keyframe->translation.y);
				writer->nextValue(false);
				writer->writeFloat(keyframe->translation.z);
				writer->closeArray(false);
				writer->nextValue(true);

				writer->openArray("rotation", false);
				writer->writeFloat(keyframe->rotation.x);
				writer->nextValue(false);
				writer->writeFloat(keyframe->rotation.y);
				writer->nextValue(false);
				writer->writeFloat(keyframe->rotation.z);
				writer->nextValue(false);
				writer->writeFloat(keyframe->rotation.w);
				writer->closeArray(false);
				writer->nextValue(true);

				writer->openArray("scale", false);
				writer->writeFloat(keyframe->scale.x);
				writer->nextValue(false);
				writer->writeFloat(keyframe->scale.y);
				writer->nextValue(false);
				writer->writeFloat(keyframe->scale.z);
				writer->closeArray(false);

				writer->closeObject();
			}

			writer->closeArray(true);

			writer->closeObject();
		}

		writer->closeArray(true);

		writer->closeObject();
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

	const char* G3djWriter::getMaterialTypeString(int materialType){
		switch(materialType){
		case 0:
			return "LAMBERT";
		case 1:
			return "PHONG";
		default:
            return "UNKNOWN";
		}
	}

	const char* G3djWriter::getTextureUseString(int textureUse){
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