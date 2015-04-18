/*******************************************************************************
 * Copyright 2011 See AUTHORS file.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 ******************************************************************************/
/** @author Xoppa */
#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_FBXMESHINFO_H
#define FBXCONV_READERS_FBXMESHINFO_H

#include <fbxsdk.h>
#include "../modeldata/Model.h"
#include <sstream>
#include <map>
#include <algorithm>
#include <functional>
#include <assert.h>
#include "util.h"
#include "matrix3.h"
#include "../log/log.h"

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace readers {
	struct FbxMeshInfo {
		// The source mesh of which the values below are extracted
		FbxMesh * const mesh;
		// The ID of the mesh (shape)
		const std::string id;
		// The maximum amount of blend weights per vertex
		const unsigned int maxVertexBlendWeightCount;
		// The actual amount of blend weights per vertex (<= maxVertexBlendWeightCount)
		unsigned int vertexBlendWeightCount;
		// Whether to use maxVertexBlendWeightCount even if the actual amount of vertex weights is less than that
		const bool forceMaxVertexBlendWeightCount;
		// Whether the required minimum amount of bones (per triangle) exceeds the specified maxNodePartBoneCount
		bool bonesOverflow;
		// The vertex attributes
		Attributes attributes;
		// Whether to use packed colors
		const bool usePackedColors;
		// The number of polygon (triangles if triangulated)
		const unsigned int polyCount;
		// The number of control points within the mesh
		const unsigned int pointCount;
		// The control points within the mesh
		const FbxVector4 * const points;
		// The number of texture coordinates within the mesh
		const unsigned int uvCount;
		// The number of element materials
		const int elementMaterialCount;
		// The number of mash parts within the mesh
		int meshPartCount;
		// The applied skin or 0 if not available
		FbxSkin * const skin;
		// The blendweights per control point
		std::vector<BlendWeight> *pointBlendWeights;
		// The collection of bones per mesh part
		std::vector<BlendBonesCollection> partBones;
		// Mapping between the polygon and the index of its meshpart
		unsigned int * const polyPartMap;
		// Mapping between the polygon and the index of its weight bones within its meshpart
		unsigned int * const polyPartBonesMap;
		// The UV bounds per part per uv coords
		float * partUVBounds;
		// The mapping name of each uv to identify the cooresponding texture
		std::string uvMapping[8];

		const FbxLayerElementArrayTemplate<FbxVector4> *normals;
		const FbxLayerElementArrayTemplate<int> *normalIndices;
		bool normalOnPoint;

		const FbxLayerElementArrayTemplate<FbxVector4> *tangents;
		const FbxLayerElementArrayTemplate<int> *tangentIndices;
		bool tangentOnPoint;

		const FbxLayerElementArrayTemplate<FbxVector4> *binormals;
		const FbxLayerElementArrayTemplate<int> *binormalIndices;
		bool binormalOnPoint;

		const FbxLayerElementArrayTemplate<FbxColor> *colors;
		const FbxLayerElementArrayTemplate<int> *colorIndices;
		bool colorOnPoint;

		const FbxLayerElementArrayTemplate<FbxVector2>*uvs[8];
		const FbxLayerElementArrayTemplate<int> *uvIndices[8];
		bool uvOnPoint[8];

		fbxconv::log::Log *log;

		FbxMeshInfo(fbxconv::log::Log *log, FbxMesh * const &mesh, const bool &usePackedColors, const unsigned int &maxVertexBlendWeightCount, const bool &forceMaxVertexBlendWeightCount, const unsigned int &maxNodePartBoneCount)
			: mesh(mesh), log(log),
			usePackedColors(usePackedColors),
			maxVertexBlendWeightCount(maxVertexBlendWeightCount), 
			vertexBlendWeightCount(0),
			forceMaxVertexBlendWeightCount(forceMaxVertexBlendWeightCount),
			pointCount(mesh->GetControlPointsCount()),
			polyCount(mesh->GetPolygonCount()),
			points(mesh->GetControlPoints()),
			elementMaterialCount(mesh->GetElementMaterialCount()),
			uvCount((unsigned int)(mesh->GetElementUVCount() > 8 ? 8 : mesh->GetElementUVCount())),
			pointBlendWeights(0),
			skin((maxNodePartBoneCount > 0 && maxVertexBlendWeightCount > 0 && (unsigned int)mesh->GetDeformerCount(FbxDeformer::eSkin) > 0) ? static_cast<FbxSkin*>(mesh->GetDeformer(0, FbxDeformer::eSkin)) : 0),
			bonesOverflow(false),
			polyPartMap(polyCount > 0 ? new unsigned int[polyCount] : 0),
			polyPartBonesMap(polyCount > 0 ? new unsigned int[polyCount] : 0),
			id(getID(mesh))
		{
			meshPartCount = calcMeshPartCount();
			partBones = std::vector<BlendBonesCollection>(meshPartCount, BlendBonesCollection(maxNodePartBoneCount));
			partUVBounds = meshPartCount * uvCount > 0 ? new float[4 * meshPartCount * uvCount] : 0;
			memset(polyPartMap, -1, sizeof(unsigned int) * polyCount);
			memset(polyPartBonesMap, 0, sizeof(unsigned int) * polyCount);
			if (partUVBounds)
				memset(partUVBounds, -1, sizeof(float) * 4 * meshPartCount * uvCount);

			if (skin) {
				fetchVertexBlendWeights();
				fetchMeshPartsAndBones();
			}
			else
				fetchMeshParts();

			fetchAttributes();
			cacheAttributes();
			fetchUVInfo();
		}

		~FbxMeshInfo() {
			if (pointBlendWeights)
				delete[] pointBlendWeights;
			if (polyPartMap)
				delete[] polyPartMap;
			if (polyPartBonesMap)
				delete[] polyPartBonesMap;
			if (partUVBounds)
				delete[] partUVBounds;
		}

		inline FbxCluster *getBone(const unsigned int &idx) {
			return skin ? skin->GetCluster(idx) : 0;
		}

		inline void getPosition(float * const &data, unsigned int &offset, const unsigned int &point) const {
			const FbxVector4 &position = points[point];
			data[offset++] = (float)position[0];
			data[offset++] = (float)position[1];
			data[offset++] = (float)position[2];
		}

		inline void getNormal(FbxVector4 * const &out, const unsigned int &polyIndex, const unsigned int &point) const {
			((FbxLayerElementArray*)normals)->GetAt<FbxVector4>(normalOnPoint ? (normalIndices ? (*normalIndices)[point] : point) : (normalIndices ? (*normalIndices)[polyIndex]: polyIndex), out);
			//return normalOnPoint ? (*normals)[normalIndices ? (*normalIndices)[point] : point] : (*normals)[normalIndices ? (*normalIndices)[polyIndex]: polyIndex];
		}

		inline void getNormal(float * const &data, unsigned int &offset, const unsigned int &polyIndex, const unsigned int &point) const {
			static FbxVector4 tmpV4;
			getNormal(&tmpV4, polyIndex, point);
			data[offset++] = (float)tmpV4.mData[0];
			data[offset++] = (float)tmpV4.mData[1];
			data[offset++] = (float)tmpV4.mData[2];
		}

		inline void getTangent(FbxVector4 * const &out, const unsigned int &polyIndex, const unsigned int &point) const {
			((FbxLayerElementArray*)tangents)->GetAt<FbxVector4>(tangentOnPoint ? (tangentIndices ? (*tangentIndices)[point] : point) : (tangentIndices ? (*tangentIndices)[polyIndex] : polyIndex), out);
			//return tangentOnPoint ? (*tangents)[tangentIndices ? (*tangentIndices)[point] : point] : (*tangents)[tangentIndices ? (*tangentIndices)[polyIndex] : polyIndex];
		}

		inline void getTangent(float * const &data, unsigned int &offset, const unsigned int &polyIndex, const unsigned int &point) const {
			static FbxVector4 tmpV4;
			getTangent(&tmpV4, polyIndex, point);
			data[offset++] = (float)tmpV4.mData[0];
			data[offset++] = (float)tmpV4.mData[1];
			data[offset++] = (float)tmpV4.mData[2];
		}

		inline void getBinormal(FbxVector4* const &out, const unsigned int &polyIndex, const unsigned int &point) const {
			((FbxLayerElementArray*)binormals)->GetAt<FbxVector4>(binormalOnPoint ? (binormalIndices ? (*binormalIndices)[point] : point) : (binormalIndices ? (*binormalIndices)[polyIndex] : polyIndex), out);
			//return binormalOnPoint ? (*binormals)[binormalIndices ? (*binormalIndices)[point] : point] : (*binormals)[binormalIndices ? (*binormalIndices)[polyIndex] : polyIndex];
		}

		inline void getBinormal(float * const &data, unsigned int &offset, const unsigned int &polyIndex, const unsigned int &point) const {
			static FbxVector4 tmpV4;
			getBinormal(&tmpV4, polyIndex, point);
			data[offset++] = (float)tmpV4.mData[0];
			data[offset++] = (float)tmpV4.mData[1];
			data[offset++] = (float)tmpV4.mData[2];
		}

		inline void getColor(FbxColor * const &out, const unsigned int &polyIndex, const unsigned int &point) const {
			((FbxLayerElementArray*)colors)->GetAt<FbxColor>(colorOnPoint ? (colorIndices ? (*colorIndices)[point] : point) : (colorIndices ? (*colorIndices)[polyIndex] : polyIndex), out);
			//return colorOnPoint ? (*colors)[colorIndices ? (*colorIndices)[point] : point] : (*colors)[colorIndices ? (*colorIndices)[polyIndex] : polyIndex];
		}

		inline void getColor(float * const &data, unsigned int &offset, const unsigned int &polyIndex, const unsigned int &point) const {
			static FbxColor tmpC;
			getColor(&tmpC, polyIndex, point);
			data[offset++] = (float)tmpC.mRed;
			data[offset++] = (float)tmpC.mGreen;
			data[offset++] = (float)tmpC.mBlue;
			data[offset++] = (float)tmpC.mAlpha;
		}

		inline void getColorPacked(float * const &data, unsigned int &offset, const unsigned int &polyIndex, const unsigned int &point) const {
			static FbxColor tmpC;
			getColor(&tmpC, polyIndex, point);
			unsigned int packedColor = ((unsigned int)(255.*tmpC.mAlpha)<<24) | ((unsigned int)(255.*tmpC.mBlue)<<16) | ((unsigned int)(255.*tmpC.mGreen)<<8) | ((unsigned int)(255.*tmpC.mRed));
			data[offset++] = *(float*)&packedColor;
		}

		inline void getUV(FbxVector2 * const &out, const unsigned int &uvIndex, const unsigned int &polyIndex, const unsigned int &point) const {
			((FbxLayerElementArray*)uvs[uvIndex])->GetAt(uvOnPoint[uvIndex] ? (uvIndices[uvIndex] ? (*uvIndices[uvIndex])[point] : point) : (uvIndices[uvIndex] ? (*uvIndices[uvIndex])[polyIndex] : polyIndex), out);
			//return uvOnPoint[uvIndex] ? (*uvs[uvIndex]).GetAt(uvIndices[uvIndex] ? (*uvIndices[uvIndex])[point] : point) : (*uvs[uvIndex]).GetAt(uvIndices[uvIndex] ? (*uvIndices[uvIndex])[polyIndex] : polyIndex);
		}

		inline void getUV(float * const &data, unsigned int &offset, const unsigned int &uvIndex, const unsigned int &polyIndex, const unsigned int &point, const Matrix3<float> &transform) const {
			static FbxVector2 uv;
			getUV(&uv, uvIndex, polyIndex, point);
			data[offset++] = (float)uv.mData[0];
			data[offset++] = (float)uv.mData[1];
			transform.transform(data[offset-2], data[offset-1]);
		}

		inline void getBlendWeight(float * const &data, unsigned int &offset, const unsigned int &weightIndex, const unsigned int &poly, const unsigned int &polyIndex, const unsigned int &point) const {
			const std::vector<BlendWeight> &weights = pointBlendWeights[point];
			const unsigned int s = (unsigned int)weights.size();
			const BlendBones &bones = partBones[polyPartMap[poly]].bones[polyPartBonesMap[poly]];
			data[offset++] = weightIndex < s ? (float)bones.idx(weights[weightIndex].index) : 0.f;
			data[offset++] = weightIndex < s ? weights[weightIndex].weight : 0.f;
		}

		inline void getVertex(float * const &data, unsigned int &offset, const unsigned int &poly, const unsigned int &polyIndex, const unsigned int &point, const Matrix3<float> * const &uvTransforms) const {
			if (attributes.hasPosition())
				getPosition(data, offset, point);
			if (attributes.hasNormal())
				getNormal(data, offset, polyIndex, point);
			if (attributes.hasColor())
				getColor(data, offset, polyIndex, point);
			if (attributes.hasColorPacked())
				getColorPacked(data, offset, polyIndex, point);
			if (attributes.hasTangent())
				getTangent(data, offset, polyIndex, point);
			if (attributes.hasBinormal())
				getBinormal(data, offset, polyIndex, point);
			for (unsigned int i = 0; i < uvCount; i++)
				getUV(data, offset, i, polyIndex, point, uvTransforms[i]);
			for (unsigned int i = 0; i < vertexBlendWeightCount; i++)
				getBlendWeight(data, offset, i, poly, polyIndex, point);
		}

		inline void getVertex(float * const &data, const unsigned int &poly, const unsigned int &polyIndex, const unsigned int &point, const Matrix3<float> * const &uvTransforms) const {
			unsigned int offset = 0;
			getVertex(data, offset, poly, polyIndex, point, uvTransforms);
		}
	private:
		static std::string getID(FbxMesh * const &mesh) {
			static int idCounter = 0;
			const char *name = mesh->GetName();
			std::stringstream ss;
			if (name != 0 && strlen(name) > 1)
				ss << name;
			else
				ss << "shape" << (++idCounter);
			return ss.str();
		}
		
		unsigned int calcMeshPartCount() {
			int mp, mpc = 0;
			for (unsigned int poly = 0; poly < polyCount; poly++) {
				mp = -1;
				for (int i = 0; i < elementMaterialCount && mp < 0; i++)
					mp = mesh->GetElementMaterial(i)->GetIndexArray()[poly];
				if (mp >= mpc)
					mpc = mp+1;
			}
			if (mpc == 0)
				mpc = 1;
			return mpc;
		}

		void fetchAttributes() {
			attributes.hasPosition(true);
			attributes.hasNormal(mesh->GetElementNormalCount() > 0);
			attributes.hasColor((!usePackedColors) && (mesh->GetElementVertexColorCount() > 0));
			attributes.hasColorPacked(usePackedColors && (mesh->GetElementVertexColorCount() > 0));
			attributes.hasTangent(mesh->GetElementTangentCount() > 0);
			attributes.hasBinormal(mesh->GetElementBinormalCount() > 0);
			for (unsigned int i = 0; i < 8; i++)
				attributes.hasUV(i, i < uvCount);
			for (unsigned int i = 0; i < 8; i++)
				attributes.hasBlendWeight(i, i < vertexBlendWeightCount);
		}

		void cacheAttributes() {
			// Cache normals, whether they are indexed and if they are located on control points or polygon points.
			normals = attributes.hasNormal() ? &(mesh->GetElementNormal()->GetDirectArray()) : 0;
			normalIndices = attributes.hasNormal() && mesh->GetElementNormal()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(mesh->GetElementNormal()->GetIndexArray()) : 0;
			normalOnPoint = attributes.hasNormal() ? mesh->GetElementNormal()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;

			// Cache tangents, whether they are indexed and if they are located on control points or polygon points.
			tangents = attributes.hasTangent() ? &(mesh->GetElementTangent()->GetDirectArray()) : 0;
			tangentIndices = attributes.hasTangent() && mesh->GetElementTangent()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(mesh->GetElementTangent()->GetIndexArray()) : 0;
			tangentOnPoint = attributes.hasTangent() ? mesh->GetElementTangent()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;
			
			// Cache binormals, whether they are indexed and if they are located on control points or polygon points.
			binormals = attributes.hasBinormal() ? &(mesh->GetElementBinormal()->GetDirectArray()) : 0;
			binormalIndices = attributes.hasBinormal() && mesh->GetElementBinormal()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(mesh->GetElementBinormal()->GetIndexArray()) : 0;
			binormalOnPoint = attributes.hasBinormal() ? mesh->GetElementBinormal()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;

			// Cache colors, whether they are indexed and if they are located on control points or polygon points.
			colors = (attributes.hasColor() || attributes.hasColorPacked()) ? &(mesh->GetElementVertexColor()->GetDirectArray()) : 0;
			colorIndices = (attributes.hasColor() || attributes.hasColorPacked()) && mesh->GetElementVertexColor()->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? 
					&(mesh->GetElementVertexColor()->GetIndexArray()) : 0;
			colorOnPoint = (attributes.hasColor() || attributes.hasColorPacked()) ? mesh->GetElementVertexColor()->GetMappingMode() == FbxGeometryElement::eByControlPoint : false;

			// Cache uvs, whether they are indexed and if they are located on control points or polygon points.
			for (unsigned int i = 0; i < uvCount; i++) {
				uvs[i] = &(mesh->GetElementUV(i)->GetDirectArray());
				uvIndices[i] = mesh->GetElementUV(i)->GetReferenceMode() == FbxGeometryElement::eIndexToDirect ? &(mesh->GetElementUV(i)->GetIndexArray()) : 0;
				uvOnPoint[i] = mesh->GetElementUV(i)->GetMappingMode() == FbxGeometryElement::eByControlPoint;
			}
		}

		void fetchVertexBlendWeights() {
			pointBlendWeights = new std::vector<BlendWeight>[pointCount];
			const int &clusterCount = skin->GetClusterCount();
			// Fetch the blend weights per control point
			for (int i = 0; i < clusterCount; i++) {
				const FbxCluster * const &cluster = skin->GetCluster(i);
				const int &indexCount = cluster->GetControlPointIndicesCount();
				const int * const &clusterIndices = cluster->GetControlPointIndices();
				const double * const &clusterWeights = cluster->GetControlPointWeights();
				for (int j = 0; j < indexCount; j++) {
					if (clusterIndices[j] < 0 || clusterIndices[j] >= (int)pointCount || clusterWeights[j] == 0.0)
						continue;
					pointBlendWeights[clusterIndices[j]].push_back(BlendWeight((float)clusterWeights[j], i));
				}
			}
			// Sort the weights, so the most significant weights are first, remove unneeded weights and normalize the remaining
			bool error = false;
			for (unsigned int i = 0; i < pointCount; i++) {
				std::sort(pointBlendWeights[i].begin(), pointBlendWeights[i].end(), std::greater<BlendWeight>());
				if (pointBlendWeights[i].size() > maxVertexBlendWeightCount)
					pointBlendWeights[i].resize(maxVertexBlendWeightCount);
				float len = 0.f;
				for (std::vector<BlendWeight>::const_iterator itr = pointBlendWeights[i].begin(); itr != pointBlendWeights[i].end(); ++itr)
					len += (*itr).weight;
				if (len == 0.f)
					error = true;
				else
					for (std::vector<BlendWeight>::iterator itr = pointBlendWeights[i].begin(); itr != pointBlendWeights[i].end(); ++itr)
						(*itr).weight /= len;
				if (pointBlendWeights[i].size() > vertexBlendWeightCount)
					vertexBlendWeightCount = (unsigned int)pointBlendWeights[i].size();
			}
			if (vertexBlendWeightCount > 0 && forceMaxVertexBlendWeightCount)
				vertexBlendWeightCount = maxVertexBlendWeightCount;
			if (error)
				log->warning(log::wSourceConvertFbxZeroWeights);
		}

		void fetchMeshPartsAndBones() {
			std::vector<std::vector<BlendWeight>*> polyWeights;
			for (unsigned int poly = 0; poly < polyCount; poly++) {
				int mp = -1;
				for (int i = 0; i < elementMaterialCount && mp < 0; i++)
					mp = mesh->GetElementMaterial(i)->GetIndexArray()[poly];
				if (mp < 0 || mp >= meshPartCount) {
					polyPartMap[poly] = -1;
					log->warning(log::wSourceConvertFbxNoPolyPart, mesh->GetName(), poly);
				}
				else {
					polyPartMap[poly] = mp;
					const unsigned int polySize = mesh->GetPolygonSize(poly);
					polyWeights.clear();
					for (unsigned int i = 0; i < polySize; i++)
						polyWeights.push_back(&pointBlendWeights[mesh->GetPolygonVertex(poly, i)]);
					const int sp = partBones[mp].add(polyWeights);
					polyPartBonesMap[poly] = sp < 0 ? 0 : (unsigned int)sp;
					if (sp < 0)
						bonesOverflow = true;
				}
			}
		}

		void fetchMeshParts() {
			int mp;
			for (unsigned int poly = 0; poly < polyCount; poly++) {
				mp = -1;
				for (int i = 0; i < elementMaterialCount && mp < 0; i++)
					mp = mesh->GetElementMaterial(i)->GetIndexArray()[poly];
				if (mp < 0 || mp >= meshPartCount) {
					polyPartMap[poly] = -1;
					log->warning(log::wSourceConvertFbxNoPolyPart, mesh->GetName(), poly);
				}
				else
					polyPartMap[poly] = mp;
			}
		}

		void fetchUVInfo() {
			FbxStringList uvSetNames;
			mesh->GetUVSetNames(uvSetNames);
			for (unsigned int i = 0; i < uvCount; i++)
				uvMapping[i] = uvSetNames.GetItemAt(i)->mString.Buffer();

			if (partUVBounds == 0 || uvCount == 0)
				return;
			FbxVector2 uv;
			int mp;
			unsigned int idx, pidx = 0, v = 0;
			for (unsigned int poly = 0; poly < polyCount; poly++) {
				mp = polyPartMap[poly];

				const unsigned int polySize = mesh->GetPolygonSize(poly);
				for (unsigned int i = 0; i < polySize; i++) {
					v = mesh->GetPolygonVertex(poly, i);
					if (mp >= 0) {
						for (unsigned int j = 0; j < uvCount; j++) {
							getUV(&uv, j, pidx, v);
							idx = 4 * (mp * uvCount + j);
							if (*(int*)&partUVBounds[idx]==-1 || uv.mData[0] < partUVBounds[idx])
								partUVBounds[idx] = (float)uv.mData[0];
							if (*(int*)&partUVBounds[idx+1]==-1 || uv.mData[1] < partUVBounds[idx+1])
								partUVBounds[idx+1] = (float)uv.mData[1];
							if (*(int*)&partUVBounds[idx+2]==-1 || uv.mData[0] > partUVBounds[idx+2])
								partUVBounds[idx+2] = (float)uv.mData[0];
							if (*(int*)&partUVBounds[idx+3]==-1 || uv.mData[1] > partUVBounds[idx+3])
								partUVBounds[idx+3] = (float)uv.mData[1];
						}
					}
					pidx++;
				}
			}
		}
	};
} }
#endif //FBXCONV_READERS_FBXMESHINFO_H