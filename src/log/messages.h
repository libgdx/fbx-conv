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
#ifndef FBXCONV_LOG_MESSAGES_H
#define FBXCONV_LOG_MESSAGES_H

#include "log.h"

namespace fbxconv {
namespace log {

LOG_START_MESSAGES(DefaultMessages)

LOG_SET_MSG(iNoError,							"No error")

LOG_SET_MSG(iVersion,							"%d.%02d.%s x%d (%s)")
LOG_SET_MSG(iNameAndVersion,					"FBX to G3Dx converter, version %d.%02d.%04d x%d %s, FBXSDK %d.%02d")

LOG_SET_MSG(eCommandLineUnknownOption,			"Unknown command line option: %s")
LOG_SET_MSG(eCommandLineUnknownArgument,		"Unknown command line argument: %s")
LOG_SET_MSG(eCommandLineMissingInputFile,		"Missing input file")
LOG_SET_MSG(eCommandLineInvalidVertexWeight,	"Maximum vertex weights must be between 0 and 8")
LOG_SET_MSG(eCommandLineInvalidBoneCount,		"Maximum bones per nodepart must be greater or equal to the maximum vertex weights")
LOG_SET_MSG(eCommandLineInvalidVertexCount,		"Maximum vertex count must be between 0 and 32k")
LOG_SET_MSG(eCommandLineUnknownFiletype,		"Unknown filetype: %s")

LOG_SET_MSG(sSourceLoad,						"Loading source file")
LOG_SET_MSG(pSourceLoadFbxImport,				"Import FBX %01.2f%% %s")
LOG_SET_MSG(wSourceLoadFbxNodeRrSs,				"[%s] Node uses RrSs mode, transformation might be incorrect")
LOG_SET_MSG(eSourceLoadGeneral,					"Error loading source file")
LOG_SET_MSG(eSourceLoadFiletypeUnknown,			"Unknown source filetype")
LOG_SET_MSG(eSourceLoadFbxSdk,					"FBX SDK encountered an error (%d): %s")

LOG_SET_MSG(sSourceConvert,						"Converting source file")
LOG_SET_MSG(sSourceConvertFbxTriangulate,		"[%s] Triangulating %s geometry")
LOG_SET_MSG(iSourceConvertFbxMeshInfo,			"[%s] polygons: %d (%d indices), control points: %d")
LOG_SET_MSG(wSourceConvertFbxDuplicateNodeId,	"[%s] Duplicate node id, skipping the node and all it's child nodes")
LOG_SET_MSG(wSourceConvertFbxInvalidBone,		"[%s] Skipping invalid bone: %s")
LOG_SET_MSG(wSourceConvertFbxAdditiveBones,		"[%s] Additive bones not supported (yet)")
LOG_SET_MSG(wSourceConvertFbxCantTriangulate,	"[%s] Skipping geometry, because it can't be triangulated")
LOG_SET_MSG(wSourceConvertFbxNoPolyPart,		"[%s] No material found for polygon %d")
LOG_SET_MSG(wSourceConvertFbxExceedsIndices,	"Mesh contains more indices (%d) than the specified maximum (%d)")
LOG_SET_MSG(wSourceConvertFbxExceedsBones,		"Mesh contains more blendweights per polygon than the specified maximum.")
LOG_SET_MSG(wSourceConvertFbxZeroWeights,		"Mesh contains vertices with zero bone weights.")
LOG_SET_MSG(wSourceConvertFbxDuplicateMesh,		"[%s] Skipping geometry with duplicate mesh")
LOG_SET_MSG(wSourceConvertFbxNoPartMaterial,	"[%s] Skipping %d parts without material, using placeholder material")
LOG_SET_MSG(wSourceConvertFbxNoMaterial,		"[%s] Skipping geometry without material")
LOG_SET_MSG(wSourceConvertFbxMaterialNotFound,	"[%s] Material not found")
LOG_SET_MSG(wSourceConvertFbxEmptyMeshpart,		"[%s] Skipping empty node part, material: '%s'")
LOG_SET_MSG(wSourceConvertFbxMaterialUnsupported,	"[%s] Material doesn\'t extend FbxSurfaceLambert, replaced with RED diffuse")
LOG_SET_MSG(wSourceConvertFbxMaterialHLSL,		"[%s] Material HLSL shading not supported, replaced with RED diffuse")
LOG_SET_MSG(wSourceConvertFbxMaterialCgFX,		"[%s] Material CgFX shading not supported, replaced with RED diffuse")
LOG_SET_MSG(wSourceConvertFbxLayeredTexture,	"[%s] Layered texture blending not supported, assuming full opacity")
LOG_SET_MSG(wSourceConvertFbxSkipPropname,		"[%s] Skipping propName '%s'")
LOG_SET_MSG(wSourceConvertFbxInvalidMesh,		"[%s] Skipping invalid mesh")
LOG_SET_MSG(eSourceConvert,						"Error converting source file")

LOG_SET_MSG(sSourceClose,						"Closing source file")
LOG_SET_MSG(eSourceClose,						"Error closing source file")

LOG_SET_MSG(sExportToG3DB,						"Exporting to G3DB file: %s")
LOG_SET_MSG(sExportToG3DJ,						"Exporting to G3DJ file: %s")
LOG_SET_MSG(sExportClose,						"Closing exported file")
LOG_SET_MSG(eExportFiletypeUnknown,				"Unknown target filetype")

LOG_SET_MSG(iModelInfoNull,						"Model is null")
LOG_SET_MSG(iModelInfoStart,					"Listing model information:")
LOG_SET_MSG(iModelInfoID,						"ID        : %s")
LOG_SET_MSG(iModelInfoVersion,					"Version   : Hi=%d, Lo=%d")
LOG_SET_MSG(iModelInfoMeshesSummary,			"Meshes    : %d (%d vertices, %d parts, %d indices)")
LOG_SET_MSG(iModelInfoNodesSummary,				"Nodes     : %d root, %d total, %d parts")
LOG_SET_MSG(iModelInfoMaterialsSummary,			"Materials : %d (%d textures)")

LOG_END_MESSAGES()

} }

#endif //FBXCONV_LOG_MESSAGES_H
