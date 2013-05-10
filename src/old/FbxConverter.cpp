#include "FbxConverter.h"
#include "gameplay/Scene.h"
#include "G3djMeshPart.h"
#include "gameplay/Transform.h"
#include <sstream>


using namespace gameplay;

namespace fbxconv {

	FbxConverter::FbxConverter(FbxConverterConfig config){
		g3djFile = new G3djFile();
		this->config = config;
		this->groupAnimation = NULL;
	}

	FbxConverter::~FbxConverter(){
		if(g3djFile){
			delete g3djFile;
			g3djFile = NULL;
		}
	}

	G3djFile* FbxConverter::load(const char* fileName){
		FbxManager* sdkManager = FbxManager::Create();
		FbxIOSettings *ios = FbxIOSettings::Create(sdkManager, IOSROOT);
		sdkManager->SetIOSettings(ios);
		FbxImporter* importer = FbxImporter::Create(sdkManager,"");
    
		if (!importer->Initialize(fileName, -1, sdkManager->GetIOSettings()))
		{
			LOG(1, "Call to FbxImporter::Initialize() failed.\n");
			LOG(1, "Error returned: %s\n\n", importer->GetLastErrorString());
			exit(-1);
		}
    
		FbxScene* fbxScene = FbxScene::Create(sdkManager,"__FBX_SCENE__");

		printf("Loading FBX file.\n");
		importer->Import(fbxScene);
		importer->Destroy();

		// Determine if animations should be grouped. Do we want this?!
		/*if (isGroupAnimationPossible(fbxScene))
		{
			if (promptUserGroupAnimations())
			{
				autoGroupAnimations = true;
			}
		}*/

		printf("Loading Scene.\n");
		loadScene(fbxScene);
		printf("Loading animations.\n");
		loadAnimations(fbxScene);
		sdkManager->Destroy();

		/*
		printf("Optimizing GamePlay Binary.");
		_gamePlayFile.adjust();
		if (_autoGroupAnimations)
		{
			_gamePlayFile.groupMeshSkinAnimations();
		}
    
		std::string outputFilePath = arguments.getOutputFilePath();

		if (arguments.textOutputEnabled())
		{
			int pos = outputFilePath.find_last_of('.');
			if (pos > 2)
			{
				std::string path = outputFilePath.substr(0, pos);
				path.append(".xml");
				LOG(1, "Saving debug file: %s\n", path.c_str());
				if (!_gamePlayFile.saveText(path))
				{
					LOG(1, "Error writing text file: %s\n", path.c_str());
				}
			}
		}
		else
		{
			LOG(1, "Saving binary file: %s\n", outputFilePath.c_str());
			if (!_gamePlayFile.saveBinary(outputFilePath))
			{
				LOG(1, "Error writing binary file: %s\n", outputFilePath.c_str());
			}
		}*/

		return g3djFile;
	}

	void FbxConverter::loadScene(FbxScene* fbxScene){
		Scene* scene = new Scene();
		scene->setId(fbxScene->GetName());
		if (scene->getId().length() == 0)
		{
			scene->setId("__SCENE__");
		}

		// Load all of the nodes and their contents.
		FbxNode* rootNode = fbxScene->GetRootNode();
		if (rootNode)
		{
			printf("Triangulate.\n");
			triangulateRecursive(rootNode);

			printf("Load nodes.\n");
			// Don't include the FBX root node in the GPB.
			const int childCount = rootNode->GetChildCount();
			for (int i = 0; i < childCount; ++i)
			{
				G3djNode* node = loadNode(rootNode->GetChild(i), true);
				if (node)
				{
					scene->add(node);
				}
			}
		}

		// Load the MeshSkin information from the scene's poses.
		loadBindShapes(fbxScene);

		// Find the ambient light of the scene
		FbxColor ambientColor = fbxScene->GetGlobalSettings().GetAmbientColor();
		scene->setAmbientColor((float)ambientColor.mRed, (float)ambientColor.mGreen, (float)ambientColor.mBlue);
    
		// Assign the first camera node (if there is one) in the scene as the active camera
		// This ensures that if there's a camera in the scene that it is assigned as the 
		// active camera.
		// TODO: add logic to find the "active" camera node in the fbxScene
		scene->setActiveCameraNode(scene->getFirstCameraNode());
    
		g3djFile->addScene(scene);
	}

	void FbxConverter::triangulateRecursive(FbxNode* fbxNode){
		// Triangulate all NURBS, patch and mesh under this node recursively.
		FbxNodeAttribute* nodeAttribute = fbxNode->GetNodeAttribute();

		if (nodeAttribute)
		{
			if (nodeAttribute->GetAttributeType() == FbxNodeAttribute::eMesh ||
				nodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbs ||
				nodeAttribute->GetAttributeType() == FbxNodeAttribute::eNurbsSurface ||
				nodeAttribute->GetAttributeType() == FbxNodeAttribute::ePatch)
			{
				FbxGeometryConverter converter(fbxNode->GetFbxManager());
				converter.TriangulateInPlace(fbxNode);
			}
		}

		const int childCount = fbxNode->GetChildCount();
		for (int childIndex = 0; childIndex < childCount; ++childIndex)
		{
			triangulateRecursive(fbxNode->GetChild(childIndex));
		}
	}

	G3djNode* FbxConverter::loadNode(FbxNode* fbxNode, bool isRoot){
		G3djNode* node = NULL;

		// Check if this node has already been loaded
		const char* id = fbxNode->GetName();
		if (id && strlen(id) > 0)
		{
			node = g3djFile->getNodeFlatList(fbxNode->GetName());
			if (node)
			{
				return node;
			}
		}
		node = new G3djNode();
		if (id)
		{
			node->setId(id);
		}
		if(isRoot) // add to hierarchy
			g3djFile->addNode(node);
		// always add node to flatlist
		g3djFile->addNodeFlatList(node);

		transformNode(fbxNode, node);
    
		// We'll ignore light & camera for now
		// loadCamera(fbxNode, node);
		// loadLight(fbxNode, node);
		loadModel(fbxNode, node);

		if (fbxNode->GetSkeleton())
		{
			// Indicate that this is a joint node for the purpose of debugging.
			// The XML debug output will print that this node is a joint.
			node->setIsJoint(true);
		}

		// Load child nodes
		const int childCount = fbxNode->GetChildCount();
		for (int i = 0; i < childCount; ++i)
		{
			G3djNode* child = loadNode(fbxNode->GetChild(i), false);
			if (child)
			{
				node->addChild(child);
			}
		}
		return node;
	}

	void FbxConverter::transformNode(FbxNode* fbxNode, G3djNode* node){
		FbxAMatrix matrix;

		if (fbxNode->GetCamera() || fbxNode->GetLight())
		{
			// TODO: Why is this necessary for Camera and Light?
			matrix.SetTRS(fbxNode->LclTranslation.Get(), fbxNode->LclRotation.Get(), fbxNode->LclScaling.Get());
		}
		else
		{
			matrix = fbxNode->EvaluateLocalTransform();
		}

		float m[16];
		Matrix tmatrix;
		copyMatrix(matrix, m);
		copyMatrix(matrix, tmatrix);
		node->setTransformMatrix(m);

		Vector3 translation;
		Vector3 scale;
		Quaternion rotation;

		tmatrix.decompose(&scale, &rotation, &translation);

		// Extended note stuff
		node->setTranslation(translation.x, translation.y, translation.z);
		node->setRotation(rotation.x, rotation.y, rotation.z, rotation.w);
		node->setScale(scale.x, scale.y, scale.z);
	}

	void FbxConverter::loadModel(FbxNode* fbxNode, Node* node){
		FbxMesh* fbxMesh = fbxNode->GetMesh();
		if (!fbxMesh)
		{
			return;
		}
		if (fbxMesh->IsTriangleMesh())
		{
			Mesh* mesh = loadMesh(fbxMesh);
			Model* model = new Model();
			model->setMesh(mesh);
			node->setModel(model);
			loadSkin(fbxMesh, model);
			if (model->getSkin())
			{
				// TODO: explain
				node->resetTransformMatrix();
			}
		}
	}

	Mesh* FbxConverter::loadMesh(FbxMesh* fbxMesh){
		// Check if this mesh has already been loaded.
		Mesh* mesh = getMesh(fbxMesh->GetUniqueID());
		if (mesh)
		{
			return mesh;
		}
		mesh = new Mesh();
		// GamePlay requires that a mesh have a unique ID but FbxMesh doesn't have a string ID.
		const char* name = fbxMesh->GetNode()->GetName();
		if (name)
		{
			std::string id(name);
			id.append("_Mesh");
			mesh->setId(id);
		}

		// The number of mesh parts is equal to the number of materials that affect this mesh.
		// There is always at least one mesh part.
		std::vector<G3djMeshPart*> meshParts;
		const int materialCount = fbxMesh->GetNode()->GetMaterialCount();
		int meshPartSize = (materialCount > 0) ? materialCount : 1;
		for (int i = 0; i < meshPartSize; ++i)
		{
			G3djMeshPart* meshPart = new G3djMeshPart();

			std::stringstream ss;
			ss << "part" << i;

			meshPart->setId(ss.str());

			meshParts.push_back(meshPart);
		}

		// Load the individual Materials for this node, if they haven't already been loaded
		for(int i=0; i<fbxMesh->GetNode()->GetMaterialCount(); i++){
			FbxSurfaceMaterial *fbxMaterial = fbxMesh->GetNode()->GetMaterial(i);

			if(g3djFile->getMaterial(fbxMaterial->GetName()) == NULL){
				// material doesn't exist, add
				G3djMaterial *material;

				// we need a name for the material
				const char* id = fbxMaterial->GetName();
				if((!id) || strlen(id) == 0){
					std::stringstream ss;
					ss << "material" << i;
					id = ss.str().c_str();
				}

				//Get the implementation to see if it's a shader.
				if(GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_HLSL) 
					|| GetImplementation(fbxMaterial, FBXSDK_IMPLEMENTATION_CGFX)){
						// We don't support shaders
						printf("Materials with shaders aren't supported. Skipping..");
						continue;
				}
				else if(fbxMaterial->GetClassId().Is(FbxSurfacePhong::ClassId)) {
					
					material = new G3djMaterial(id, PHONG);

					// cast to phong
					FbxSurfacePhong *phongMaterial = ((FbxSurfacePhong *)fbxMaterial);
					
					// set the material values
					material->setDiffuse(phongMaterial->Diffuse.Get().mData[0], 
										 phongMaterial->Diffuse.Get().mData[1],
										 phongMaterial->Diffuse.Get().mData[2]);

					material->setAmbient(phongMaterial->Ambient.Get().mData[0], 
										 phongMaterial->Ambient.Get().mData[1],
										 phongMaterial->Ambient.Get().mData[2]);

					material->setEmissive(phongMaterial->Emissive.Get().mData[0], 
										 phongMaterial->Emissive.Get().mData[1],
										 phongMaterial->Emissive.Get().mData[2]);

					material->setSpecular(phongMaterial->Specular.Get().mData[0], 
										 phongMaterial->Specular.Get().mData[1],
										 phongMaterial->Specular.Get().mData[2]);

					material->setShininess(phongMaterial->Shininess.Get());
					// Opacity = 1.0f - TransparencyFactor
					material->setOpacity(1.0f - phongMaterial->TransparencyFactor.Get());

					g3djFile->addMaterial(material);
				}
				else if(fbxMaterial->GetClassId().Is(FbxSurfaceLambert::ClassId)) {
					material = new G3djMaterial(id, LAMBERT);

					// cast to phong
					FbxSurfaceLambert *lambertMaterial = ((FbxSurfaceLambert *)fbxMaterial);
					
					// set the material values
					material->setDiffuse(lambertMaterial->Diffuse.Get().mData[0], 
										 lambertMaterial->Diffuse.Get().mData[1],
										 lambertMaterial->Diffuse.Get().mData[2]);

					material->setAmbient(lambertMaterial->Ambient.Get().mData[0], 
										 lambertMaterial->Ambient.Get().mData[1],
										 lambertMaterial->Ambient.Get().mData[2]);

					material->setEmissive(lambertMaterial->Emissive.Get().mData[0], 
										 lambertMaterial->Emissive.Get().mData[1],
										 lambertMaterial->Emissive.Get().mData[2]);

					// Opacity = 1.0f - TransparencyFactor
					material->setOpacity(1.0f - lambertMaterial->TransparencyFactor.Get());

					g3djFile->addMaterial(material);
				}
				else
					printf("Unknown type of material.\n");

				int textureCount = FbxLayerElement::sTypeTextureCount;
				for(int i=0; i<textureCount; i++){
					FbxProperty fbxProperty = fbxMaterial->FindProperty(FbxLayerElement::sTextureChannelNames[i]);
					if(fbxProperty.IsValid())
						loadTexturesFromProperty(fbxProperty, material);
				}
			}
		}

		// Find the blend weights and blend indices if this mesh is skinned.
		std::vector<std::vector<Vector2> > weights;
		bool hasSkin = loadBlendWeights(fbxMesh, weights);
    
		// Get list of uv sets for mesh
		FbxStringList uvSetNameList;
		fbxMesh->GetUVSetNames(uvSetNameList);
		const int uvSetCount = uvSetNameList.GetCount();

		int vertexIndex = 0;
		FbxVector4* controlPoints = fbxMesh->GetControlPoints();
		const int polygonCount = fbxMesh->GetPolygonCount();
		for (int polyIndex = 0; polyIndex < polygonCount; ++polyIndex)
		{
			const int polygonSize = fbxMesh->GetPolygonSize(polyIndex);
			for (int posInPoly = 0; posInPoly < polygonSize; ++posInPoly)
			{
				int controlPointIndex = fbxMesh->GetPolygonVertex(polyIndex, posInPoly);
				Vertex vertex;

				FbxVector4& position = controlPoints[controlPointIndex];
				vertex.position.x = (float)position[0];
				vertex.position.y = (float)position[1];
				vertex.position.z = (float)position[2];

				// Load tex coords for all uv sets
				for (int uvSetIndex = 0; uvSetIndex < uvSetCount; ++uvSetIndex)
				{
					const FbxGeometryElementUV* uvElement = fbxMesh->GetElementUV(uvSetNameList.GetStringAt(uvSetIndex));
					if (uvElement)
						loadTextureCoords(fbxMesh, uvElement, uvSetIndex, polyIndex, posInPoly, vertexIndex, &vertex);
				}

				// Load other data
				loadNormal(fbxMesh, vertexIndex, controlPointIndex, &vertex);
				loadTangent(fbxMesh, vertexIndex, controlPointIndex, &vertex);
				loadBinormal(fbxMesh, vertexIndex, controlPointIndex, &vertex);
				loadVertexColor(fbxMesh, vertexIndex, controlPointIndex, &vertex);

				if (hasSkin)
				{
					loadBlendData(weights[controlPointIndex], &vertex);
				}

				// Determine which mesh part this vertex index should be added to based on the material that affects it.
				int meshPartIndex = 0;
				const int elementMaterialCount = fbxMesh->GetElementMaterialCount();

				// Should find a better way to do this, not every vertex :/
				if(elementMaterialCount == 0){
					// one material for the entire mesh, easy
					FbxGeometryElementMaterial* elementMaterial = fbxMesh->GetElementMaterial(0);

					FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(elementMaterial->GetIndexArray().GetAt(0));    
					int matId = elementMaterial->GetIndexArray().GetAt(0);
					if(matId >= 0)
					{
						meshParts[meshPartIndex]->setMaterialId(fbxMaterial->GetName()); 
					}
				}
				else {
					// we have multiple materials for the mesh
					for (int k = 0; k < elementMaterialCount; ++k)
					{
						FbxGeometryElementMaterial* elementMaterial = fbxMesh->GetElementMaterial(k);
						meshPartIndex = elementMaterial->GetIndexArray().GetAt(polyIndex);

						FbxSurfaceMaterial* fbxMaterial = fbxMesh->GetNode()->GetMaterial(meshPartIndex);
						
						if(meshPartIndex >= 0)
						{
							meshParts[meshPartIndex]->setMaterialId(fbxMaterial->GetName());
						}
					}
				}

				// Add the vertex to the mesh if it hasn't already been added and find the vertex index.
				unsigned int index;
				if (mesh->contains(vertex))
				{
					index = mesh->getVertexIndex(vertex);
				}
				else
				{
					index = mesh->addVertex(vertex);
				}
				meshParts[meshPartIndex]->addIndex(index);
				vertexIndex++;
			}
		}

		const size_t meshpartsSize = meshParts.size();
		for (size_t i = 0; i < meshpartsSize; ++i)
		{
			mesh->addMeshPart(meshParts[i]);
		}

		// The order that the vertex elements are add to the list matters.
		// It should be the same order as how the Vertex data is written.

		// Position
		mesh->addVetexAttribute(POSITION, Vertex::POSITION_COUNT);

		const Vertex& vertex = mesh->vertices[0];
		// Normals
		if (vertex.hasNormal)
		{
			mesh->addVetexAttribute(NORMAL, Vertex::NORMAL_COUNT);
		}
		// Tangents
		if (vertex.hasTangent)
		{
			mesh->addVetexAttribute(TANGENT, Vertex::TANGENT_COUNT);
		}
		// Binormals
		if (vertex.hasBinormal)
		{
			mesh->addVetexAttribute(BINORMAL, Vertex::BINORMAL_COUNT);
		}
		// Texture Coordinates
		for (unsigned int i = 0; i < MAX_UV_SETS; ++i)
		{
			if (vertex.hasTexCoord[i])
			{
				mesh->addVetexAttribute(TEXCOORD0 + i, Vertex::TEXCOORD_COUNT);
			}
		}
		// Diffuse Color
		if (vertex.hasDiffuse)
		{
			mesh->addVetexAttribute(COLOR, Vertex::DIFFUSE_COUNT);
		}
		// Skinning BlendWeights BlendIndices
		if (vertex.hasWeights)
		{
			mesh->addVetexAttribute(BLENDWEIGHTS, Vertex::BLEND_WEIGHTS_COUNT);
			mesh->addVetexAttribute(BLENDINDICES, Vertex::BLEND_INDICES_COUNT);
		}

		g3djFile->addMesh(mesh);
		saveMesh(fbxMesh->GetUniqueID(), mesh);
		return mesh;
	}

	Mesh* FbxConverter::getMesh(FbxUInt64 meshId)
	{
		// Check if this mesh was already loaded.
		std::map<FbxUInt64, Mesh*>::iterator it = meshes.find(meshId);
		if (it != meshes.end())
		{
			return it->second;
		}
		return NULL;
	}

	void FbxConverter::saveMesh(FbxUInt64 meshId, Mesh* mesh)
	{
		assert(mesh);
		if (!getMesh(meshId))
		{
			meshes[meshId] = mesh;
		}
	}

	void FbxConverter::loadTexturesFromProperty(FbxProperty fbxProperty, G3djMaterial* material){
		int textureCount = fbxProperty.GetSrcObjectCount(FbxTexture::ClassId);

		for(int i=0; i<textureCount; i++){
			// Try cast to LayeredTexture
			FbxLayeredTexture *layeredTexture = FbxCast<FbxLayeredTexture>(fbxProperty.GetSrcObject(FbxLayeredTexture::ClassId, i));

			if(layeredTexture){
				// We have a layered texture, oh joy
			}
			else {
				// Non-layered, get it from the property
				FbxTexture* fbxTexture = FbxCast <FbxTexture>(fbxProperty.GetSrcObject(FbxTexture::ClassId, i));
				
				// Only support file textures for now
                FbxFileTexture* fbxFileTexture = FbxCast<FbxFileTexture>(fbxTexture);
                if(fbxFileTexture)
                {
					// TODO: This only caters for same material texture reuse, but we probably should support global.
					if(material->getTexture(fbxTexture->GetName()) == NULL){
						Texture* texture = new Texture();

						texture->setId(fbxTexture->GetName());
						// Extract fileName from path, is this cross-platform safe?
						std::string absolutePath(fbxFileTexture->GetFileName());
						unsigned int npos = absolutePath.find_last_of("/\\");

						// Assumes files to be in the same folder as fbx, hrm
						texture->setRelativePath(absolutePath.substr(npos+1).c_str());
						texture->uvScale.set(fbxTexture->GetUVScaling().mData[0], fbxTexture->GetUVScaling().mData[1]);
						texture->uvTranslation.set(fbxTexture->GetUVTranslation().mData[0],fbxTexture->GetUVTranslation().mData[1]);
						texture->textureUse = fbxTexture->GetTextureUse();

						material->addTexture(texture);
					}
                }
			}
		}
	}

	bool FbxConverter::loadBlendWeights(FbxMesh* fbxMesh, std::vector<std::vector<Vector2> >& weights)
	{
		assert(fbxMesh);
		const int vertexCount = fbxMesh->GetControlPointsCount();

		FbxSkin* fbxSkin = NULL;
		const int deformerCount = fbxMesh->GetDeformerCount();
		for (int i = 0; i < deformerCount; ++i)
		{
			FbxDeformer* deformer = fbxMesh->GetDeformer(i);
			if (deformer->GetDeformerType() == FbxDeformer::eSkin)
			{
				fbxSkin = static_cast<FbxSkin*>(deformer);
				weights.resize(vertexCount);

				const int clusterCount = fbxSkin->GetClusterCount();
				for (int j = 0; j < clusterCount; ++j)
				{
					FbxCluster* cluster = fbxSkin->GetCluster(j);
					assert(cluster);
					const int vertexIndexCount = cluster->GetControlPointIndicesCount();
					for (int k = 0; k < vertexIndexCount; ++k)
					{
						int index = cluster->GetControlPointIndices()[k];
						if (index >= vertexCount)
						{
							continue;
						}

						double weight = cluster->GetControlPointWeights()[k];
						if (weight == 0.0)
						{
							continue;
						}
						weights[index].push_back(Vector2((float)j, (float)weight));
					}
				}
				// Only the first skin deformer will be loaded.
				// There probably won't be more than one.
				break;
			}
		}
		return fbxSkin != NULL;
	}

	void FbxConverter::loadTextureCoords(FbxMesh* fbxMesh, const FbxGeometryElementUV* uvs, int uvSetIndex, int polyIndex, int posInPoly, int meshVertexIndex, Vertex* vertex)
	{
		assert(fbxMesh && polyIndex >=0 && posInPoly >= 0);

		const bool useIndex = uvs->GetReferenceMode() != FbxGeometryElement::eDirect;
		const int indexCount = useIndex ? uvs->GetIndexArray().GetCount() : 0;
		int uvIndex = -1;

		switch (uvs->GetMappingMode())
		{
		case FbxGeometryElement::eByControlPoint:
			{
				// Get the index of the current vertex in control points array
				int polyVertIndex = fbxMesh->GetPolygonVertex(polyIndex, posInPoly);

				// The UV index depends on the reference mode
				uvIndex = useIndex ? uvs->GetIndexArray().GetAt(polyVertIndex) : polyVertIndex;
			}
			break;

		case FbxGeometryElement::eByPolygonVertex:
			if (meshVertexIndex < indexCount)
			{
				uvIndex = useIndex ? uvs->GetIndexArray().GetAt(meshVertexIndex) : meshVertexIndex;
			}
			break;

		default:
			// Only support eByPolygonVertex and eByControlPoint mappings
			break;
		}

		vertex->hasTexCoord[uvSetIndex] = true;

		// Store UV information in vertex
		if (uvIndex != -1)
		{
			FbxVector2 uvValue = uvs->GetDirectArray().GetAt(uvIndex);
			vertex->texCoord[uvSetIndex].x = (float)uvValue[0];
			if(config.flipV)
				vertex->texCoord[uvSetIndex].y = 1.0f-(float)uvValue[1];
			else
				vertex->texCoord[uvSetIndex].y = (float)uvValue[1];
		}
	}

	void FbxConverter::loadNormal(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex)
	{
		if (fbxMesh->GetElementNormalCount() > 0)
		{
			// Get only the first
			FbxGeometryElementNormal* normal = fbxMesh->GetElementNormal(0);
			FbxGeometryElement::EMappingMode mappingMode = normal->GetMappingMode();
			if (mappingMode == FbxGeometryElement::eByControlPoint)
			{
				switch (normal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxVector4 vec4 = normal->GetDirectArray().GetAt(controlPointIndex);
						vertex->hasNormal = true;
						vertex->normal.x = (float)vec4[0];
						vertex->normal.y = (float)vec4[1];
						vertex->normal.z = (float)vec4[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = normal->GetIndexArray().GetAt(controlPointIndex);
						FbxVector4 vec4 = normal->GetDirectArray().GetAt(id);
						vertex->hasNormal = true;
						vertex->normal.x = (float)vec4[0];
						vertex->normal.y = (float)vec4[1];
						vertex->normal.z = (float)vec4[2];
					}
					break;
				default:
					break;
				}
			}
			else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				switch (normal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxVector4 vec4 = normal->GetDirectArray().GetAt(vertexIndex);
						vertex->hasNormal = true;
						vertex->normal.x = (float)vec4[0];
						vertex->normal.y = (float)vec4[1];
						vertex->normal.z = (float)vec4[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = normal->GetIndexArray().GetAt(vertexIndex);
						FbxVector4 vec4 = normal->GetDirectArray().GetAt(id);
						vertex->hasNormal = true;
						vertex->normal.x = (float)vec4[0];
						vertex->normal.y = (float)vec4[1];
						vertex->normal.z = (float)vec4[2];
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void FbxConverter::loadTangent(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex)
	{
		if (fbxMesh->GetElementTangentCount() > 0)
		{
			// Get only the first tangent
			FbxGeometryElementTangent* tangent = fbxMesh->GetElementTangent(0);
			FbxGeometryElement::EMappingMode mappingMode = tangent->GetMappingMode();
			if (mappingMode == FbxGeometryElement::eByControlPoint)
			{
				switch (tangent->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxVector4 vec4 = tangent->GetDirectArray().GetAt(controlPointIndex);
						vertex->hasTangent = true;
						vertex->tangent.x = (float)vec4[0];
						vertex->tangent.y = (float)vec4[1];
						vertex->tangent.z = (float)vec4[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = tangent->GetIndexArray().GetAt(controlPointIndex);
						FbxVector4 vec4 = tangent->GetDirectArray().GetAt(id);
						vertex->hasTangent = true;
						vertex->tangent.x = (float)vec4[0];
						vertex->tangent.y = (float)vec4[1];
						vertex->tangent.z = (float)vec4[2];
					}
					break;
				default:
					break;
				}
			}
			else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				switch (tangent->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxVector4 vec4 = tangent->GetDirectArray().GetAt(vertexIndex);
						vertex->hasTangent = true;
						vertex->tangent.x = (float)vec4[0];
						vertex->tangent.y = (float)vec4[1];
						vertex->tangent.z = (float)vec4[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = tangent->GetIndexArray().GetAt(vertexIndex);
						FbxVector4 vec4 = tangent->GetDirectArray().GetAt(id);
						vertex->hasTangent = true;
						vertex->tangent.x = (float)vec4[0];
						vertex->tangent.y = (float)vec4[1];
						vertex->tangent.z = (float)vec4[2];
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void FbxConverter::loadBinormal(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex)
	{
		if (fbxMesh->GetElementBinormalCount() > 0)
		{
			// Get only the first binormal.
			FbxGeometryElementBinormal* binormal = fbxMesh->GetElementBinormal(0);
			FbxGeometryElement::EMappingMode mappingMode = binormal->GetMappingMode();

			if (mappingMode == FbxGeometryElement::eByControlPoint)
			{
				switch (binormal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxVector4 vec4 = binormal->GetDirectArray().GetAt(controlPointIndex);
						vertex->hasBinormal = true;
						vertex->binormal.x = (float)vec4[0];
						vertex->binormal.y = (float)vec4[1];
						vertex->binormal.z = (float)vec4[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = binormal->GetIndexArray().GetAt(controlPointIndex);
						FbxVector4 vec4 = binormal->GetDirectArray().GetAt(id);
						vertex->hasBinormal = true;
						vertex->binormal.x = (float)vec4[0];
						vertex->binormal.y = (float)vec4[1];
						vertex->binormal.z = (float)vec4[2];
					}
					break;
				default:
					break;
				}
			}
			else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				switch (binormal->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxVector4 vec4 = binormal->GetDirectArray().GetAt(vertexIndex);
						vertex->hasBinormal = true;
						vertex->binormal.x = (float)vec4[0];
						vertex->binormal.y = (float)vec4[1];
						vertex->binormal.z = (float)vec4[2];
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = binormal->GetIndexArray().GetAt(vertexIndex);
						FbxVector4 vec4 = binormal->GetDirectArray().GetAt(id);
						vertex->hasBinormal = true;
						vertex->binormal.x = (float)vec4[0];
						vertex->binormal.y = (float)vec4[1];
						vertex->binormal.z = (float)vec4[2];
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void FbxConverter::loadVertexColor(FbxMesh* fbxMesh, int vertexIndex, int controlPointIndex, Vertex* vertex)
	{
		if (fbxMesh->GetElementVertexColorCount() > 0)
		{
			// Get only the first vertex color.
			FbxGeometryElementVertexColor* vertexColor = fbxMesh->GetElementVertexColor(0);
			FbxGeometryElement::EMappingMode mappingMode = vertexColor->GetMappingMode();
			if (mappingMode == FbxGeometryElement::eByControlPoint)
			{
				switch (vertexColor->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxColor color = vertexColor->GetDirectArray().GetAt(controlPointIndex);

						vertex->hasDiffuse = true;
						vertex->diffuse.x = (float)color.mRed;
						vertex->diffuse.y = (float)color.mGreen;
						vertex->diffuse.z = (float)color.mBlue;
						vertex->diffuse.w = (float)color.mAlpha;
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = vertexColor->GetIndexArray().GetAt(controlPointIndex);
						FbxColor color = vertexColor->GetDirectArray().GetAt(id);

						vertex->hasDiffuse = true;
						vertex->diffuse.x = (float)color.mRed;
						vertex->diffuse.y = (float)color.mGreen;
						vertex->diffuse.z = (float)color.mBlue;
						vertex->diffuse.w = (float)color.mAlpha;
					}
					break;
				default:
					break;
				}
			}
			else if (mappingMode == FbxGeometryElement::eByPolygonVertex)
			{
				switch (vertexColor->GetReferenceMode())
				{
				case FbxGeometryElement::eDirect:
					{
						FbxColor color = vertexColor->GetDirectArray().GetAt(vertexIndex);

						vertex->hasDiffuse = true;
						vertex->diffuse.x = (float)color.mRed;
						vertex->diffuse.y = (float)color.mGreen;
						vertex->diffuse.z = (float)color.mBlue;
						vertex->diffuse.w = (float)color.mAlpha;
					}
					break;
				case FbxGeometryElement::eIndexToDirect:
					{
						int id = vertexColor->GetIndexArray().GetAt(vertexIndex);
						FbxColor color = vertexColor->GetDirectArray().GetAt(id);

						vertex->hasDiffuse = true;
						vertex->diffuse.x = (float)color.mRed;
						vertex->diffuse.y = (float)color.mGreen;
						vertex->diffuse.z = (float)color.mBlue;
						vertex->diffuse.w = (float)color.mAlpha;
					}
					break;
				default:
					break;
				}
			}
		}
	}

	void FbxConverter::loadBlendData(const std::vector<Vector2>& vertexWeights, Vertex* vertex)
	{
		size_t size = vertexWeights.size();

		if (size >= 1)
		{
			vertex->hasWeights= true;
			vertex->blendIndices.x = vertexWeights[0].x;
			vertex->blendWeights.x = vertexWeights[0].y;
		}
		if (size >= 2)
		{
			vertex->blendIndices.y = vertexWeights[1].x;
			vertex->blendWeights.y = vertexWeights[1].y;
		}
		if (size >= 3)
		{
			vertex->blendIndices.z = vertexWeights[2].x;
			vertex->blendWeights.z = vertexWeights[2].y;
		}
		if (size >= 4)
		{
			vertex->blendIndices.w = vertexWeights[3].x;
			vertex->blendWeights.w = vertexWeights[3].y;
		}
		//vertex->normalizeBlendWeight();
	}

	void FbxConverter::loadSkin(FbxMesh* fbxMesh, Model* model){
		const int deformerCount = fbxMesh->GetDeformerCount();
		for (int i = 0; i < deformerCount; ++i)
		{
			FbxDeformer* deformer = fbxMesh->GetDeformer(i);
			if (deformer->GetDeformerType() == FbxDeformer::eSkin)
			{
				FbxSkin* fbxSkin = static_cast<FbxSkin*>(deformer);

				MeshSkin* skin = new MeshSkin();

				std::vector<std::string> jointNames;
				std::vector<Node*> joints;
				std::vector<Matrix> bindPoses;

				const int clusterCount = fbxSkin->GetClusterCount();
				for (int j = 0; j < clusterCount; ++j)
				{
					FbxCluster* cluster = fbxSkin->GetCluster(j);
					assert(cluster);
					FbxNode* linkedNode = cluster->GetLink();
					if (linkedNode && linkedNode->GetSkeleton())
					{
						const char* jointName = linkedNode->GetName();
						assert(jointName);
						jointNames.push_back(jointName);
						Node* joint = loadNode(linkedNode, false);
						assert(joint);
						joints.push_back(joint);

						FbxAMatrix matrix;
						cluster->GetTransformLinkMatrix(matrix);
						Matrix m;
						copyMatrix(matrix.Inverse(), m);
						bindPoses.push_back(m);
					}
				}
				skin->setJointNames(jointNames);
				skin->setJoints(joints);
				skin->setBindPoses(bindPoses);
				model->setSkin(skin);
				break;
			}
		}
	}

	void FbxConverter::loadBindShapes(FbxScene* fbxScene){
		float m[16];
		const int poseCount = fbxScene->GetPoseCount();
		for (int i = 0; i < poseCount; ++i)
		{
			FbxPose* pose = fbxScene->GetPose(i);
			assert(pose);
			if (pose->IsBindPose() && pose->GetCount() > 0)
			{
				FbxNode* fbxNode = pose->GetNode(0);
				if (fbxNode->GetMesh() != NULL)
				{
					Node* node = g3djFile->getNodeFlatList(fbxNode->GetName());
					assert(node && node->getModel());

					Model* model = node->getModel();
					if (model && model->getSkin())
					{
						MeshSkin* skin = model->getSkin();
						copyMatrix(pose->GetMatrix(0), m);
						skin->setBindShape(m);
					}
				}
			}
		}
	}

	void FbxConverter::loadAnimations(FbxScene* fbxScene){
		FbxAnimEvaluator* evaluator = fbxScene->GetEvaluator();
		if (!evaluator)
			return;
		FbxAnimStack* animStack = evaluator->GetContext();
		if (!animStack)
			return;

		for (int i = 0; i < fbxScene->GetSrcObjectCount(FBX_TYPE(FbxAnimStack)); ++i)
		{
			FbxAnimStack* animStack = FbxCast<FbxAnimStack>(fbxScene->GetSrcObject(FBX_TYPE(FbxAnimStack), i));
			int nbAnimLayers = animStack->GetMemberCount(FBX_TYPE(FbxAnimLayer));

			AnimationClip* animationClip = new AnimationClip();
			animationClip->setClipId(animStack->GetName());

			for (int l = 0; l < nbAnimLayers; ++l)
			{
				FbxAnimLayer* animLayer = animStack->GetMember(FBX_TYPE(FbxAnimLayer), l);
				loadAnimationLayer(animLayer, fbxScene->GetRootNode(), animationClip);
			}

			g3djFile->addAnimationClip(animationClip);
		}
	}

	void FbxConverter::loadAnimationLayer(FbxAnimLayer* fbxAnimLayer, FbxNode* fbxNode, AnimationClip* clip)
	{
		bool animationGroupId = false;
		const char* name = fbxNode->GetName();
		
		G3djAnimation* animation = new G3djAnimation();
		animation->setId(name);
		animation->setBoneId(name);
		
		loadAnimationChannels(fbxAnimLayer, fbxNode, animation, clip);

		const int childCount = fbxNode->GetChildCount();
		for (int modelCount = 0; modelCount < childCount; ++modelCount)
		{
			loadAnimationLayer(fbxAnimLayer, fbxNode->GetChild(modelCount), clip);
		}
	}

	void FbxConverter::loadAnimationChannels(FbxAnimLayer* animLayer, FbxNode* fbxNode, G3djAnimation* animation, AnimationClip* clip)
	{
		const char* name = fbxNode->GetName();
		//Node* node = _gamePlayFile.getNode(name);

		// Determine which properties are animated on this node
		// Find the transform at each key frame
		// TODO: Ignore properties that are not animated (scale, rotation, translation)
		// This should result in only one animation channel per animated node.

		float startTime = FLT_MAX, stopTime = -1.0f, frameRate = -FLT_MAX;
		bool tx = false, ty = false, tz = false, rx = false, ry = false, rz = false, sx = false, sy = false, sz = false;
		FbxAnimCurve* animCurve = NULL;
		animCurve = getCurve(fbxNode->LclTranslation, animLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (animCurve)
		{
			tx = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclTranslation, animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (animCurve)
		{
			ty = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclTranslation, animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (animCurve)
		{
			tz = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclRotation, animLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (animCurve)
		{
			rx = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclRotation, animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (animCurve)
		{
			ry = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclRotation, animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (animCurve)
		{
			rz = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclScaling, animLayer, FBXSDK_CURVENODE_COMPONENT_X);
		if (animCurve)
		{
			sx = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclScaling, animLayer, FBXSDK_CURVENODE_COMPONENT_Y);
		if (animCurve)
		{
			sy = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}
		animCurve = getCurve(fbxNode->LclScaling, animLayer, FBXSDK_CURVENODE_COMPONENT_Z);
		if (animCurve)
		{
			sz = true;
			findMinMaxTime(animCurve, &startTime, &stopTime, &frameRate);
		}

		if (!(sx || sy || sz || rx || ry || rz || tx || ty || tz))
			return; // no animation channels

		assert(startTime != FLT_MAX);
		assert(stopTime >= 0.0f);

		// Evaulate animation curve in increments of frameRate and populate channel data.
		FbxAMatrix fbxMatrix;
		Matrix matrix;
		float increment = 1000.0f / frameRate;
		for (float time = startTime; time <= stopTime; time += increment)
		{
			// Clamp time to stopTime
			time = std::min(time, stopTime);

			// Evalulate the animation at this time
			FbxTime kTime;
			kTime.SetMilliSeconds((FbxLongLong)time);
			fbxMatrix = fbxNode->EvaluateLocalTransform(kTime);
			copyMatrix(fbxMatrix, matrix);

			// Decompose the evalulated transformation matrix into separate
			// scale, rotation and translation.
			Vector3 scale;
			Quaternion rotation;
			Vector3 translation;
			matrix.decompose(&scale, &rotation, &translation);
			rotation.normalize();

			// Append keyframe data to animation
			Keyframe* keyframe = new Keyframe();
			keyframe->keytime = time;
			keyframe->translation.set(translation);
			keyframe->rotation.set(rotation);
			keyframe->scale.set(scale);

			animation->addKeyframe(keyframe);
		}

		clip->addAnimation(animation);
	}

	void FbxConverter::copyMatrix(const FbxMatrix& fbxMatrix, float* matrix)
	{
		int i = 0;
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				matrix[i++] = (float)fbxMatrix.Get(row, col);
			}
		}
	}

	void FbxConverter::copyMatrix(const FbxMatrix& fbxMatrix, Matrix& matrix)
	{
		int i = 0;
		for (int row = 0; row < 4; ++row)
		{
			for (int col = 0; col < 4; ++col)
			{
				matrix.m[i++] = (float)fbxMatrix.Get(row, col);
			}
		}
	}

	bool FbxConverter::isGroupAnimationPossible(FbxScene* fbxScene)
	{
		FbxNode* rootNode = fbxScene->GetRootNode();
		if (rootNode)
		{
			if (isGroupAnimationPossible(rootNode))
				return true;
		}
		return false;
	}

	bool FbxConverter::isGroupAnimationPossible(FbxNode* fbxNode)
	{
		if (fbxNode)
		{
			FbxMesh* fbxMesh = fbxNode->GetMesh();
			if (isGroupAnimationPossible(fbxMesh))
				return true;
			const int childCount = fbxNode->GetChildCount();
			for (int i = 0; i < childCount; ++i)
			{
				if (isGroupAnimationPossible(fbxNode->GetChild(i)))
					return true;
			}
		}
		return false;
	}

	bool FbxConverter::isGroupAnimationPossible(FbxMesh* fbxMesh)
	{
		if (fbxMesh)
		{
			const int deformerCount = fbxMesh->GetDeformerCount();
			for (int i = 0; i < deformerCount; ++i)
			{
				FbxDeformer* deformer = fbxMesh->GetDeformer(i);
				if (deformer->GetDeformerType() == FbxDeformer::eSkin)
				{
					FbxSkin* fbxSkin = static_cast<FbxSkin*>(deformer);
					if (fbxSkin)
					{
						return true;
					}
				}
			}
		}
		return false;
	}

	FbxAnimCurve* FbxConverter::getCurve(FbxPropertyT<FbxDouble3>& prop, FbxAnimLayer* animLayer, const char* pChannel)
	{
	#if FBXSDK_VERSION_MAJOR == 2013 && FBXSDK_VERSION_MINOR == 1
		return prop.GetCurve<FbxAnimCurve>(animLayer, pChannel);
	#else
		return prop.GetCurve(animLayer, pChannel);
	#endif
	}

	void FbxConverter::findMinMaxTime(FbxAnimCurve* animCurve, float* startTime, float* stopTime, float* frameRate)
	{
		FbxTime start, stop;
		FbxTimeSpan timeSpan;
		animCurve->GetTimeInterval(timeSpan);
		start = timeSpan.GetStart();
		stop = timeSpan.GetStop();
		*startTime = std::min(*startTime, (float)start.GetMilliSeconds());
		*stopTime = std::max(*stopTime, (float)stop.GetMilliSeconds());
		*frameRate = std::max(*frameRate, (float)stop.GetFrameRate(FbxTime::eDefaultMode));
	}
};

