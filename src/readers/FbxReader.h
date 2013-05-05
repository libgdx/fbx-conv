#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_FBXREADER_H
#define FBXCONV_READERS_FBXREADER_H

#include <fbxsdk.h>
#include "../modeldata/Model.h"
#include <sstream>
#include <map>
#include <algorithm>
#include <assert.h>
#include "util.h"

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace readers {
	struct FbxReader {
		typedef void (*TextureCallback)(Material::Texture *texture);
		TextureCallback textureCallback;
		/** The last error */
		const char * error;
		/** The maximum allowed amount of vertices in one mesh, only used when deciding to merge meshes. */
		unsigned int maxVertexCount;
		/** The maximum allowed amount of bone weights in one vertex, if a vertex exceeds this amount it will clipped on importance. */
		unsigned int maxVertexBoneCount;
		/** Always use maxVertexBoneCount for blendWeights, this might help merging meshes. */
		bool forceMaxVertexBoneCount;
		/** The maximum allowed amount of bones in one nodepart, if a meshpart exceeds this amount it will be split up in parts. */
		unsigned int maxNodePartBoneCount;
		/** Whether to flip the y-component of textures coordinates. */
		bool flipV;
		/** Whether to print (debug) progress information. */
		bool verbose;
#define DBGLOG(...)					\
	if (verbose) {					\
		printf("FBXREADER: ");		\
		printf(__VA_ARGS__);		\
		printf("\n");				\
	}
		
		std::map<NodePart *, std::pair<const FbxSkin *, BlendBones>> nodePartBones;

		FbxReader() {
			flipV = false;
			error = 0;
			textureCallback = 0;
			maxVertexCount = (1 << 15) - 1;
			maxVertexBoneCount = 4;
			maxNodePartBoneCount = (1 << 15) - 1;
			forceMaxVertexBoneCount = false;
		}

		FbxScene *openFbxFile(const char *filename) {
			DBGLOG("Creating FbxManager");
			FbxManager *fbxManager = FbxManager::Create();
			fbxManager->SetIOSettings(FbxIOSettings::Create(fbxManager, IOSROOT));
			DBGLOG("Creating FbxImporter");
			FbxImporter* importer = FbxImporter::Create(fbxManager, "");

			DBGLOG("Initializing importer for file '%s'", filename);
			if (!importer->Initialize(filename, -1, fbxManager->GetIOSettings())) {
				setError(importer->GetLastErrorString());
				importer->Destroy();
				fbxManager->Destroy();
				return 0;
			}

			DBGLOG("Creating FbxScene");
			FbxScene* fbxScene = FbxScene::Create(fbxManager,"__FBX_SCENE__");
			DBGLOG("Importing FbxScene");
			importer->Import(fbxScene);
			importer->Destroy();
			return fbxScene;
		}

		void closeFbxFile(FbxScene *fbxScene) {
			DBGLOG("Destroying FbxManager");
			fbxScene->GetFbxManager()->Destroy();
		}

		/** List the textures actually used within the specified node (or the root node if no node is given) and its subnodes */
		bool listTextures(const FbxScene *fbxScene, std::vector<std::string> &out, FbxNode *node = 0) {
			if (node == 0)
				node = fbxScene->GetRootNode();

			const FbxMesh *mesh = node->GetMesh();
			if (mesh) {
				const int mc = node->GetMaterialCount();
				for (int m = 0; m < mc ; m++) {
					const FbxSurfaceMaterial *material = node->GetMaterial(m);
					const int textureCount = FbxLayerElement::sTypeTextureCount;
					for (int i = 0; i < textureCount; i++) {
						FbxProperty prop = material->FindProperty(FbxLayerElement::sTextureChannelNames[i]);
						if (prop.IsValid()) {
							const int n = prop.GetSrcObjectCount<FbxFileTexture>();
							for (int j = 0; j < n; j++) {
								FbxFileTexture *texture = prop.GetSrcObject<FbxFileTexture>(j);
								if (texture) {
									std::string fn = texture->GetFileName();
									if (std::find(out.begin(), out.end(), fn)==out.end())
										out.push_back(fn);
								}
							}
						}
					}
				}
			}

			const int cc = node->GetChildCount();
			for (int i = 0; i < cc; i++)
				if (!listTextures(fbxScene, out, node->GetChild(i)))
					return false;
			return true;
		}

		/** Load a model from a FBX file */
		bool load(Model *model, const char *filename, Node *parent = 0) {
			FbxScene* fbxScene = openFbxFile(filename);
			bool result = load(model, fbxScene, parent);
			closeFbxFile(fbxScene);
			return result;
		}

		inline bool setError(const char * error) {
			this->error = error;
			return false;
		}

		/** Load a model from a FBX scene */
		bool load(Model *model, const FbxScene *fbxScene, Node *parent = 0) {
			if (model->id.empty())
				model->id = fbxScene->GetName();
			FbxNode *root = fbxScene->GetRootNode();
			nodePartBones.clear();
			const int n = root->GetChildCount();
			for (int i = 0; i < n; i++)
				if (!addNode(model, root->GetChild(i), parent))
					return false;
			for (std::map<NodePart *, std::pair<const FbxSkin *, BlendBones>>::iterator itr = nodePartBones.begin(); itr != nodePartBones.end(); ++itr) {
				const unsigned int n = (*itr).second.second.size();
				for (unsigned int i = 0; i < n; i++) {
					const int blendIndex = (*itr).second.second[i];
					(*itr).first->bones.push_back(parent == 0 ? model->getNode((*itr).second.first->GetCluster(blendIndex)->GetLink()->GetName()) : 
															parent->getChild((*itr).second.first->GetCluster(blendIndex)->GetLink()->GetName()));
				}
			}
			addAnimations(model, fbxScene);
			return true;
		}

		/** Add a FBX node as a child node of the parent model node (or as root node if parent is null) */
		bool addNode(Model *model, FbxNode *source, Node *parent = 0) {
			const char *id = source->GetName();
			DBGLOG("Add node '%s'",id);
			// If the model already contains a node with the same name, simply copy that node.
			const Node *existing = id ? model->getNode(id) : 0;
			Node *node = existing ? new Node(*existing) : new Node(id);

			if (parent)
				parent->children.push_back(node);
			else
				model->nodes.push_back(node);

			return existing ? true : setNode(model, node, source);
		}

		/** Set a mode node to the FBX node and add the required resources to the model */
		bool setNode(Model *model, Node *node, FbxNode *source) {
			FbxAMatrix *m = &(source->EvaluateLocalTransform());
			FbxVector4 *translate = &m->GetT();
			node->transform.translation[0] = (float)(*translate)[0];
			node->transform.translation[1] = (float)(*translate)[1];
			node->transform.translation[2] = (float)(*translate)[2];
			FbxQuaternion *rotation = &m->GetQ();
			node->transform.rotation[0] = (float)(*rotation)[0];
			node->transform.rotation[1] = (float)(*rotation)[1];
			node->transform.rotation[2] = (float)(*rotation)[2];
			node->transform.rotation[3] = (float)(*rotation)[3];
			FbxVector4 *scale = &m->GetS();
			node->transform.scale[0] = (float)(*scale)[0];
			node->transform.scale[1] = (float)(*scale)[1];
			node->transform.scale[2] = (float)(*scale)[2];

			// Triangulate if needed and possible
			const FbxNodeAttribute* attribute = source->GetNodeAttribute();
			const FbxNodeAttribute::EType type = attribute ? attribute->GetAttributeType() : FbxNodeAttribute::eNull;
			if (type == FbxNodeAttribute::eMesh || type == FbxNodeAttribute::eNurbs || type  == FbxNodeAttribute::eNurbsSurface || type  == FbxNodeAttribute::ePatch) {
				DBGLOG("Triangulating node '%s'",source->GetName());
				FbxGeometryConverter converter(source->GetFbxManager());
				converter.TriangulateInPlace(source);
			}

			// Add the mesh (parts) to the node if available
			const FbxMesh *mesh = source->GetMesh();
			if (mesh)
				if (!addMesh(model, node, mesh, PRIMITIVETYPE_TRIANGLES))
					return false;

			// Add the child nodes to the node
			const int n = source->GetChildCount();
			for (int i = 0; i < n; i++)
				if (!addNode(model, source->GetChild(i), node))
					return false;
			return true;
		}

		/** find a mesh in the model capable of storing the specified attributes and at least the specified amount of vertices */
		Mesh *findReusableMesh(Model *model, const unsigned int attributes, const unsigned int vertexCount) {
			for (std::vector<Mesh *>::iterator itr = model->meshes.begin(); itr != model->meshes.end(); ++itr)
				if ((*itr)->attributes == attributes && ((*itr)->vertices.size() / (*itr)->vertexSize) + vertexCount <= maxVertexCount)
					return (*itr);
			return 0;
		}

		/** Add a FBX mesh to a model and it's parts to the node */
		bool addMesh(Model *model, Node *node, const FbxMesh *source, unsigned int primitiveType) {
			// Meshes must be triangulated, this implies that the polygons below always contain three points
			if (!source->IsTriangleMesh())
				return setError("Mesh is not triangulated");
			if (maxVertexBoneCount > 8)
				return setError("Maximum bones per vertex must be less than or equal to 8");

			bool result = true;

			// Mesh parts must have an unique id
			static unsigned long meshPartCounter = 0;
			

			{// Cache node and reuse if possible
				// Node instances are reused
				static std::map<const FbxUInt64, const Node *> cache;
				// If a node with the same mesh is cached, simply copy it's parts to this node
				const FbxUInt64 id = source->GetUniqueID();
				std::map<const FbxUInt64, const Node *>::iterator it = cache.find(id);
				if (it != cache.end()) {
					DBGLOG("A node with the same ID %d already exists, reusing that node", id);
					for (std::vector<NodePart *>::const_iterator itr = it->second->parts.begin(); itr != it->second->parts.end(); ++itr)
						node->parts.push_back(new NodePart(*(*itr)));
					return true;
				}
				// otherwise add this node to the cache
				cache[id] = node;
			}

			const unsigned int vertexCount = source->GetControlPointsCount();
			const unsigned int polyCount = source->GetPolygonCount();
			const FbxVector4 *points = source->GetControlPoints();
			const unsigned int uvCount = (unsigned int)(source->GetElementUVCount() > 8 ? 8 : source->GetElementUVCount());
			const int materialCount = source->GetNode()->GetMaterialCount();
			const int meshPartCount = materialCount == 0 ? 1 : materialCount; // Always add at least one mesh part, even if the mesh doesn't contain one
			const unsigned int elementMaterialCount = source->GetElementMaterialCount() == 0 ? 1 : source->GetElementMaterialCount();

			// Check if the mesh is skinned and if so fetch the first skin (restricted to one skin)
			const FbxSkin *skin = ((unsigned int)source->GetDeformerCount(FbxDeformer::eSkin) > 0) ? static_cast<FbxSkin*>(source->GetDeformer(0, FbxDeformer::eSkin)) : 0;

			std::vector<BlendBonesCollection> partBones(meshPartCount, BlendBonesCollection(maxNodePartBoneCount));
			std::vector<BlendWeight> *weights = 0;
			// map polygons to mesh parts
			unsigned int *polyPartMap = new unsigned int[polyCount];
			memset(polyPartMap, -1, sizeof(unsigned int) * polyCount);
			unsigned int *polySubpartMap = new unsigned int[polyCount];
			memset(polySubpartMap, 0, sizeof(unsigned int) * polyCount);
			unsigned int blendWeightCount = 0;

			if (skin) {
				// Fetch the blend weights per control point
				weights = new std::vector<BlendWeight>[vertexCount];
				const int clusterCount = skin->GetClusterCount();
				for (int i = 0; i < clusterCount; i++) {
					const FbxCluster *cluster = skin->GetCluster(i);
					const int indexCount = cluster->GetControlPointIndicesCount();
					const int *clusterIndices = cluster->GetControlPointIndices();
					const double *clusterWeights = cluster->GetControlPointWeights();
					for (int j = 0; j < indexCount; j++) {
						if (clusterIndices[j] < 0 || clusterIndices[j] >= (int)vertexCount || clusterWeights[j] == 0.0)
							continue;
						weights[clusterIndices[j]].push_back(BlendWeight((float)clusterWeights[j], i));
					}
				}
				// Sort the weights, so the most significant weights are first, remove unneeded weights and normalize the remaining
				for (unsigned int i = 0; i < vertexCount; i++) {
					std::sort(weights[i].begin(), weights[i].end(), std::greater<BlendWeight>());
					if (weights[i].size() > maxVertexBoneCount)
						weights[i].resize(maxVertexBoneCount);
					float len = 0.f;
					for (std::vector<BlendWeight>::const_iterator itr = weights[i].begin(); itr != weights[i].end(); ++itr)
						len += (*itr).weight;
					for (std::vector<BlendWeight>::iterator itr = weights[i].begin(); itr != weights[i].end(); ++itr)
						(*itr).weight /= len;
					if (weights[i].size() > blendWeightCount)
						blendWeightCount = weights[i].size();
				}
				// Get the bones per polygon and divide the meshpart into subparts if needed
				std::vector<std::vector<BlendWeight>*> polyWeights;
				for (unsigned int poly = 0; poly < polyCount; poly++) {
					int mp = -1;
					for (unsigned int i = 0; i < elementMaterialCount && mp < 0; i++)
						mp = source->GetElementMaterial(i)->GetIndexArray()[poly];
					if (mp < 0 || mp >= meshPartCount)
						polyPartMap[poly] = -1;
					else {
						polyPartMap[poly] = mp;
						const unsigned int polySize = source->GetPolygonSize(poly);
						polyWeights.clear();
						for (unsigned int i = 0; i < polySize; i++)
							polyWeights.push_back(&weights[source->GetPolygonVertex(poly, i)]);
						const int sp = partBones[mp].add(polyWeights);
						polySubpartMap[poly] = sp < 0 ? 0 : (unsigned int)sp;
						if (sp < 0)
							result = setError("Too many bones per polygon, incease maxNodePartBoneCount to resolve.");
					}
				}
			} else {
				int mp;
				for (unsigned int poly = 0; poly < polyCount; poly++) {
					mp = -1;
					for (unsigned int i = 0; i < elementMaterialCount && mp < 0; i++)
						mp = source->GetElementMaterial(i)->GetIndexArray()[poly];
					if (mp < 0 || mp >= meshPartCount)
						polyPartMap[poly] = -1;
					else
						polyPartMap[poly] = mp;
				}
			}

			if (blendWeightCount > 0 && forceMaxVertexBoneCount)
				blendWeightCount = maxVertexBoneCount;

			// Check which vertex attributes the mesh contains
			Attributes attributes;
			attributes.hasPosition(true);
			attributes.hasNormal(source->GetElementNormalCount() > 0);
			attributes.hasColor(source->GetElementVertexColorCount() > 0);
			attributes.hasTangent(source->GetElementTangentCount() > 0);
			attributes.hasBinormal(source->GetElementBinormalCount() > 0);
			for (unsigned int i = 0; i < 8; i++)
				attributes.hasUV(i, i < uvCount);
			for (unsigned int i = 0; i < 8; i++)
				attributes.hasBlendWeight(i, i < blendWeightCount);

			unsigned int vertexSize = attributes.size();

			Mesh *mesh;
			// Check if the model contains a mesh with the same attributes, which is small enough for this mesh to be added.
			// If so, reuse that, otherwise create a new mesh. The maximum size is polyCount * 3, the minimum size is vertexCount.
			if (!(mesh = findReusableMesh(model, attributes.value, polyCount * 3))) {
				model->meshes.push_back(mesh = new Mesh());
				mesh->attributes = attributes;
				mesh->vertexSize = vertexSize;
			} else
				DBGLOG("Merging mesh with previous created mesh");

			// The parts and materials this mesh contains
			MeshPart ***parts = new MeshPart**[meshPartCount];
			Material **materials = new Material*[materialCount];

			// Add the mesh parts and materials to the model and create node parts for each pair
			for (int i = 0; i < meshPartCount; i++) {
				const unsigned int n = partBones[i].size();
				const unsigned int m = n < 1 ? 1 : n;
				parts[i] = new MeshPart*[m];
				for (int j = 0; j < m; j++) {
					parts[i][j] = new MeshPart();
					std::stringstream ss;
					ss << "mpart" << ++meshPartCounter;
					parts[i][j]->id = ss.str();
					parts[i][j]->primitiveType = primitiveType;
					mesh->parts.push_back(parts[i][j]);

					NodePart *npart = new NodePart();
					npart->material = i < materialCount ? materials[i] = addMaterial(model, source->GetNode()->GetMaterial(i)) : 0;
					npart->meshPart = parts[i][j];
					node->parts.push_back(npart);

					if (j < n)
						this->nodePartBones[npart] = std::pair<const FbxSkin *, BlendBones>(skin, partBones[i][j]);
				}
			}

			// Cache normals, whether they are indexed and if they are located on control points or polygon points.
			const FbxLayerElementArrayTemplate<FbxVector4> *normals = attributes.hasNormal() ? &(source->GetElementNormal()->GetDirectArray()) : 0;
			const FbxLayerElementArrayTemplate<int> *normalIndices = 
				attributes.hasNormal() && source->GetElementNormal()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(source->GetElementNormal()->GetIndexArray()) : 0;
			const bool normalVertex = attributes.hasNormal() ? source->GetElementNormal()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;

			// Cache tangents, whether they are indexed and if they are located on control points or polygon points.
			const FbxLayerElementArrayTemplate<FbxVector4> *tangents = attributes.hasTangent() ? &(source->GetElementTangent()->GetDirectArray()) : 0;
			const FbxLayerElementArrayTemplate<int> *tangentIndices = 
				attributes.hasTangent() && source->GetElementTangent()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(source->GetElementTangent()->GetIndexArray()) : 0;
			const bool tangentVertex = attributes.hasTangent() ? source->GetElementTangent()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;
			
			// Cache binormals, whether they are indexed and if they are located on control points or polygon points.
			const FbxLayerElementArrayTemplate<FbxVector4> *binormals = attributes.hasBinormal() ? &(source->GetElementBinormal()->GetDirectArray()) : 0;
			const FbxLayerElementArrayTemplate<int> *binormalIndices = 
				attributes.hasBinormal() && source->GetElementBinormal()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(source->GetElementBinormal()->GetIndexArray()) : 0;
			const bool binormalVertex = attributes.hasBinormal() ? source->GetElementBinormal()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;

			// Cache colors, whether they are indexed and if they are located on control points or polygon points.
			const FbxLayerElementArrayTemplate<FbxColor> *colors = attributes.hasColor() ? &(source->GetElementVertexColor()->GetDirectArray()) : 0;
			const FbxLayerElementArrayTemplate<int> *colorIndices = 
				attributes.hasColor() && source->GetElementVertexColor()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(source->GetElementVertexColor()->GetIndexArray()) : 0;
			const bool colorVertex = attributes.hasColor() ? source->GetElementVertexColor()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;

			// Cache uvs, whether they are indexed and if they are located on control points or polygon points.
			const FbxLayerElementArrayTemplate<FbxVector2>*uvs[8];
			bool uvVertex[8];
			const FbxLayerElementArrayTemplate<int> *uvIndices[8];
			for (unsigned int i = 0; i < uvCount; i++) {
				uvs[i] = &(source->GetElementUV(i)->GetDirectArray());
				uvIndices[i] = source->GetElementUV(i)->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(source->GetElementUV(i)->GetIndexArray()) : 0;
				uvVertex[i] = source->GetElementUV(i)->GetMappingMode() == FbxGeometryElement::eByControlPoint;
			}

			// reserve at least the minimum size of the mesh
			mesh->vertices.reserve(mesh->vertices.size() + vertexSize * vertexCount);

			float *vertex = new float[vertexSize];
			const FbxVector4 *normal, *tangent, *binormal;
			const FbxColor *color;
			const FbxVector2 *uv[8];
			unsigned int pidx = 0;
			int mp, sp;
			MeshPart *meshpart;
			Material *material;
			for (unsigned int poly = 0; poly < polyCount; poly++) {
				// find the mesh part this triangle belongs to
				if ((mp = polyPartMap[poly]) < 0)
					continue;
				sp = polySubpartMap[poly];
				meshpart = parts[mp][sp];
				material = mp < materialCount ? materials[mp] : 0;  

				// Add the three points of this triangle
				const unsigned int polySize = source->GetPolygonSize(poly); // always 3
				for (unsigned int i = 0; i < polySize; i++) {
					// v = the control point of this triangle point, pidx = the cummulative index of this triangle point amongst all triangles.
					// If an attribute is located on the control point then v is used, if it is located on the triangle point pidx is used.
					const unsigned int v = source->GetPolygonVertex(poly, i);

					int idx = 0;
					if (attributes.hasPosition()) {
						vertex[idx++] = (float)points[v][0];
						vertex[idx++] = (float)points[v][1];
						vertex[idx++] = (float)points[v][2];
					}
					if (attributes.hasNormal()) {
						normal = normalVertex ? &((*normals)[normalIndices ? (*normalIndices)[v] : v]) : &((*normals)[normalIndices ? (*normalIndices)[pidx]: pidx]);
						vertex[idx++] = (float)(*normal)[0];
						vertex[idx++] = (float)(*normal)[1];
						vertex[idx++] = (float)(*normal)[2];
					}
					if (attributes.hasColor()) {
						color = colorVertex ? &((*colors)[colorIndices ? (*colorIndices)[v] : v]) : &((*colors)[colorIndices ? (*colorIndices)[pidx] : pidx]);
						vertex[idx++] = (float)(*color).mRed;
						vertex[idx++] = (float)(*color).mGreen;
						vertex[idx++] = (float)(*color).mBlue;
						vertex[idx++] = (float)(*color).mAlpha;
					}
					if (attributes.hasTangent()) {
						tangent = tangentVertex ? &((*tangents)[tangentIndices ? (*tangentIndices)[v] : v]) : &((*tangents)[tangentIndices ? (*tangentIndices)[pidx] : pidx]);
						vertex[idx++] = (float)(*tangent)[0];
						vertex[idx++] = (float)(*tangent)[1];
						vertex[idx++] = (float)(*tangent)[2];
					}
					if (attributes.hasBinormal()) {
						binormal = binormalVertex ? &((*binormals)[binormalIndices ? (*binormalIndices)[v] : v]) : &((*binormals)[binormalIndices ? (*binormalIndices)[pidx] : pidx]);
						vertex[idx++] = (float)(*binormal)[0];
						vertex[idx++] = (float)(*binormal)[1];
						vertex[idx++] = (float)(*binormal)[2];
					}
					for (unsigned int j = 0; j < uvCount; j++) {
						uv[j] = uvVertex[j] ? &((*uvs[j]).GetAt(uvIndices[j] ? (*uvIndices[j])[v] : v)) : &((*uvs[j]).GetAt(uvIndices[j] ? (*uvIndices[j])[pidx] : pidx));
						vertex[idx++] = (float)(*uv[j]).mData[0];
						vertex[idx++] = flipV ? 1.f - (float)(*uv[j]).mData[1] : (float)(*uv[j]).mData[1];
						if (material != 0 && j < material->textures.size()) {
							vertex[idx-2] = material->textures[j]->bakeUvTranslation[0] + material->textures[j]->bakeUvScale[0] * (material->textures[j]->bakeUvRotate ? vertex[idx-1] : vertex[idx-2]);
							vertex[idx-1] = material->textures[j]->bakeUvTranslation[1] + material->textures[j]->bakeUvScale[1] * (material->textures[j]->bakeUvRotate ? vertex[idx-2] : vertex[idx-1]);
						}
					}
					if (blendWeightCount > 0) {
						unsigned int s = weights[v].size();
						BlendBones *bones = &partBones[mp][sp];
						for (unsigned int j = 0; j < blendWeightCount; j++) {
							vertex[idx++] = j < s ? bones->idx(weights[v][j].index) : 0.f;
							vertex[idx++] = j < s ? weights[v][j].weight : 0.f;
						}
					}
					// Check if a vertex with the same values already exists in the mesh, otherwise add it.
					const unsigned short index = mesh->add(vertex);
					// Add the index to the mesh part.
					meshpart->indices.push_back(index);
					pidx++;
				}
			}

			delete[] vertex;
			for (unsigned int i = 0; i < meshPartCount; i++)
				delete[] parts[i];
			delete[] parts;
			delete[] polyPartMap;
			delete[] polySubpartMap;
			delete[] materials;
			if (weights)
				delete[] weights;
			return result;
		}

		/** Add the FBX material to the model and returns it. */
		Material *addMaterial(Model *model, const FbxSurfaceMaterial *source) {
			static unsigned long materialCounter = 0;
			std::string id = source->GetName();
			if (id.empty()) {
				// Materials must have name, so generate one
				std::stringstream ss;
				ss << "material" << ++materialCounter;
				id = ss.str();
			} else {
				// Check if a material with same name already exists
				Material *existing = model->getMaterial(id.c_str());
				if (existing) {
					DBGLOG("Reusing existing material '%s'",id.c_str());
					return existing;
				}
			}
			DBGLOG("Adding material '%s'",id.c_str());

			// Create a new material, model is responsable for destroying it
			Material *material = new Material();
			model->materials.push_back(material);
			material->id = id;
			if(GetImplementation(source, FBXSDK_IMPLEMENTATION_HLSL) || GetImplementation(source, FBXSDK_IMPLEMENTATION_CGFX))
				return material; // FIXME: implement materials with shaders?
			
			// Set the lamber and phong components
			if (source->GetClassId().Is(FbxSurfaceLambert::ClassId) || source->GetClassId().Is(FbxSurfacePhong::ClassId)) {
				FbxSurfaceLambert *lambert = (FbxSurfaceLambert*)source;
				material->diffuse[0] = (float)lambert->Diffuse.Get()[0];
				material->diffuse[1] = (float)lambert->Diffuse.Get()[1];
				material->diffuse[2] = (float)lambert->Diffuse.Get()[2];
				material->ambient[0] = (float)lambert->Ambient.Get()[0];
				material->ambient[1] = (float)lambert->Ambient.Get()[1];
				material->ambient[2] = (float)lambert->Ambient.Get()[2];
				material->emissive[0] = (float)lambert->Emissive.Get()[0];
				material->emissive[1] = (float)lambert->Emissive.Get()[1];
				material->emissive[2] = (float)lambert->Emissive.Get()[2];
				material->opacity = 1.f - (float)lambert->TransparencyFactor.Get();
				if (source->GetClassId().Is(FbxSurfacePhong::ClassId)) {
					FbxSurfacePhong *phong = (FbxSurfacePhong*)source;
					material->specular[0] = (float)phong->Specular.Get()[0];
					material->specular[1] = (float)phong->Specular.Get()[1];
					material->specular[2] = (float)phong->Specular.Get()[2];
					material->shininess = (float)phong->Shininess.Get();
				}
			}

			// Add the textures
			const int textureCount = FbxLayerElement::sTypeTextureCount;
			for (int i = 0; i < textureCount; i++) {
				FbxProperty prop = source->FindProperty(FbxLayerElement::sTextureChannelNames[i]);
				if (prop.IsValid()) {
					const int n = prop.GetSrcObjectCount<FbxFileTexture>();
					for (int j = 0; j < n; j++) {
						FbxFileTexture *texture = prop.GetSrcObject<FbxFileTexture>(j);
						if (texture != 0 && material->getTexture(texture->GetName())==0) {
							Material::Texture *t = new Material::Texture();
							t->id = texture->GetName();
							t->path = texture->GetFileName();
							t->uvTranslation[0] = (float)texture->GetUVTranslation().mData[0];
							t->uvTranslation[1] = (float)texture->GetUVTranslation().mData[1];
							t->uvScale[0] = (float)texture->GetUVScaling().mData[0];
							t->uvScale[1] = (float)texture->GetUVScaling().mData[1];
							t->usage = texture->GetTextureUse();
							if (textureCallback != 0)
								textureCallback(t);
							material->textures.push_back(t);
						}
					}
				}
			}
			return material;
		}

		/** Add the animations if any */
		void addAnimations(Model *model, const FbxScene *source) {
			const unsigned int animCount = source->GetSrcObjectCount<FbxAnimStack>();
			for (unsigned int i = 0; i < animCount; i++)
				addAnimation(model, source->GetSrcObject<FbxAnimStack>(i));
		}

		/** Add the specified animation to the model */
		void addAnimation(Model *model, FbxAnimStack *animStack) {
			DBGLOG("Add animation '%s'",animStack->GetName());
			static std::vector<Keyframe *> frames;
			static std::map<FbxNode *, AnimInfo> affectedNodes;
			affectedNodes.clear();
			
			// Could also use animStack->GetLocalTimeSpan and animStack->BakeLayers, but its not guaranteed to be correct
			const int layerCount = animStack->GetMemberCount<FbxAnimLayer>();
			for (int l = 0; l < layerCount; l++) {
				FbxAnimLayer *layer = animStack->GetMember<FbxAnimLayer>(l);
				// For each layer check which node is affected and within what time frame and rate
				const int curveNodeCount = layer->GetSrcObjectCount<FbxAnimCurveNode>();
				for (int n = 0; n < curveNodeCount; n++) {
					FbxAnimCurveNode *curveNode = layer->GetSrcObject<FbxAnimCurveNode>(n);
					// Check which properties on this curve are changed
					const int nc = curveNode->GetDstPropertyCount();
					for (int o = 0; o < nc; o++) {
						FbxProperty *prop = &curveNode->GetDstProperty(o);
						FbxNode *node = static_cast<FbxNode *>(prop->GetFbxObject());
						if (node) {
							FbxString propName = prop->GetName();
							// Only add translation, scaling or rotation
							if (propName != node->LclTranslation.GetName() && propName != node->LclScaling.GetName() && propName != node->LclRotation.GetName())
								continue;
							FbxAnimCurve *curve;
							AnimInfo ts;
							ts.translate = propName == node->LclTranslation.GetName();
							ts.rotate = propName == node->LclRotation.GetName();
							ts.scale = propName == node->LclScaling.GetName();
							if (curve = prop->GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_X))
								updateAnimTime(curve, ts);
							if (curve = prop->GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Y))
								updateAnimTime(curve, ts);
							if (curve = prop->GetCurve(layer, FBXSDK_CURVENODE_COMPONENT_Z))
								updateAnimTime(curve, ts);
							//if (ts.start < ts.stop)
								affectedNodes[node] += ts;
						}
					}
				}
			}

			if (affectedNodes.empty())
				return;

			Animation *animation = new Animation();
			model->animations.push_back(animation);
			animation->id = animStack->GetName();
			animStack->GetScene()->GetEvaluator()->SetContext(animStack);

			// Add the NodeAnimations to the Animation
			for (std::map<FbxNode *, AnimInfo>::const_iterator itr = affectedNodes.begin(); itr != affectedNodes.end(); itr++) {
				Node *node = model->getNode((*itr).first->GetName());
				if (!node)
					continue;
				frames.clear();
				NodeAnimation *nodeAnim = new NodeAnimation();
				animation->nodeAnimations.push_back(nodeAnim);
				nodeAnim->node = node;
				nodeAnim->translate = (*itr).second.translate;
				nodeAnim->rotate = (*itr).second.rotate;
				nodeAnim->scale = (*itr).second.scale;
				const float stepSize = (*itr).second.framerate <= 0.f ? (*itr).second.stop - (*itr).second.start : 1000.f / (*itr).second.framerate;
				FbxTime fbxTime;
				// Calculate all keyframes upfront
				for (float time = (*itr).second.start; time <= (*itr).second.stop; time += stepSize) {
					time = std::min(time, (*itr).second.stop);
					fbxTime.SetMilliSeconds((FbxLongLong)time);
					Keyframe *kf = new Keyframe();
					kf->time = time;
					FbxAMatrix *m = &(*itr).first->EvaluateLocalTransform(fbxTime);
					FbxVector4 *v = &m->GetT();
					kf->translation[0] = (float)v->mData[0];
					kf->translation[1] = (float)v->mData[1];
					kf->translation[2] = (float)v->mData[2];
					FbxQuaternion *q = &m->GetQ();
					kf->rotation[0] = (float)q->mData[0];
					kf->rotation[1] = (float)q->mData[1];
					kf->rotation[2] = (float)q->mData[2];
					kf->rotation[3] = (float)q->mData[3];
					v = &m->GetS();
					kf->scale[0] = (float)v->mData[0];
					kf->scale[1] = (float)v->mData[1];
					kf->scale[2] = (float)v->mData[2];
					frames.push_back(kf);
				}
				// Only add keyframes really needed
				addKeyframes(nodeAnim, frames);
			}
		}

		inline void updateAnimTime(FbxAnimCurve *curve, AnimInfo &ts) {
			FbxTimeSpan fts;
			curve->GetTimeInterval(fts);
			const FbxTime start = fts.GetStart();
			const FbxTime stop = fts.GetStop();
			ts.start = std::min(ts.start, (float)(start.GetMilliSeconds()));
			ts.stop = std::max(ts.stop, (float)stop.GetMilliSeconds());
			// Could check the nunber and type of keys (ie curve->KeyGetInterpolation) to lower the framerate
			ts.framerate = std::max(ts.framerate, (float)stop.GetFrameRate(FbxTime::eDefaultMode));
		}

		void addKeyframes(NodeAnimation *anim, std::vector<Keyframe *> &keyframes) {
			bool translate = false, rotate = false, scale = false;
			// Check which components are actually changed
			for (std::vector<Keyframe *>::const_iterator itr = keyframes.begin(); itr != keyframes.end(); ++itr) {
				if (!translate && !cmp(anim->node->transform.translation, (*itr)->translation, 3))
					translate = true;
				if (!rotate && !cmp(anim->node->transform.rotation, (*itr)->rotation, 3))
					rotate = true;
				if (!scale && !cmp(anim->node->transform.scale, (*itr)->scale, 3))
					scale = true;
			}
			// This allows to only export the values actual needed
			anim->translate = translate;
			anim->rotate = rotate;
			anim->scale = scale;
			if (!keyframes.empty()) {
				anim->keyframes.push_back(keyframes[0]);
				const int last = keyframes.size()-1;
				Keyframe *k1 = keyframes[0], *k2, *k3;
				for (int i = 1; i < last; i++) {
					k2 = keyframes[i];
					k3 = keyframes[i+1];
					// Check if the middle keyframe can be calculated by information, if so dont add it
					if ((translate && !isLerp(k1->translation, k1->time, k2->translation, k2->time, k3->translation, k3->time, 3)) ||
						(rotate && !isLerp(k1->rotation, k1->time, k2->rotation, k2->time, k3->rotation, k3->time, 3)) || // FIXME use slerp for quaternions
						(scale && !isLerp(k1->scale, k1->time, k2->scale, k2->time, k3->scale, k3->time, 3))) {
							anim->keyframes.push_back(k2);
							k1 = k2;
					} else
						delete k2;
				}
				if (last > 0)
					anim->keyframes.push_back(keyframes[last]);
			}
		}

		inline bool cmp(const double &v1, const double &v2, const double &epsilon = 0.000001) {
			const double d = v1 - v2;
			return ((d < 0.f) ? -d : d) < epsilon;
		}

		inline bool cmp(const double *v1, const double *v2, const unsigned int &count) {
			for (unsigned int i = 0; i < count; i++)
				if (!cmp(v1[i],v2[i]))
					return false;
			return true;
		}

		inline bool isLerp(const double *v1, const double &t1, const double *v2, const double &t2, const double *v3, const double &t3, const int size) {
			const double d = (t2 - t1) / (t3 - t1);
			for (int i = 0; i < size; i++)
				if (!cmp(v2[i], v1[i] + d * (v3[i] - v1[i])))
					return false;
			return true;
		}
	};
} }

#endif //FBXCONV_READERS_FBXREADER_H