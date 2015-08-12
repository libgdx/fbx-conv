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
#ifndef FBXCONV_LOG_CODES_H
#define FBXCONV_LOG_CODES_H

namespace fbxconv {
namespace log {
	int num_codes = 0;

#define LOG_ADD_CODE(code) const int code = num_codes++;

LOG_ADD_CODE(iNoError)

LOG_ADD_CODE(iVersion)
LOG_ADD_CODE(iNameAndVersion)

LOG_ADD_CODE(eCommandLineUnknownOption)
LOG_ADD_CODE(eCommandLineUnknownArgument)
LOG_ADD_CODE(eCommandLineMissingInputFile)
LOG_ADD_CODE(eCommandLineInvalidVertexWeight)
LOG_ADD_CODE(eCommandLineInvalidBoneCount)
LOG_ADD_CODE(eCommandLineInvalidVertexCount)
LOG_ADD_CODE(eCommandLineUnknownFiletype)

LOG_ADD_CODE(sSourceLoad)
LOG_ADD_CODE(pSourceLoadFbxImport)
LOG_ADD_CODE(wSourceLoadFbxNodeRrSs)
LOG_ADD_CODE(eSourceLoadGeneral)
LOG_ADD_CODE(eSourceLoadFiletypeUnknown)
LOG_ADD_CODE(eSourceLoadFbxSdk)

LOG_ADD_CODE(sSourceConvert)
LOG_ADD_CODE(sSourceConvertFbxTriangulate)
LOG_ADD_CODE(iSourceConvertFbxMeshInfo)
LOG_ADD_CODE(wSourceConvertFbxDuplicateNodeId)
LOG_ADD_CODE(wSourceConvertFbxInvalidBone)
LOG_ADD_CODE(wSourceConvertFbxAdditiveBones)
LOG_ADD_CODE(wSourceConvertFbxCantTriangulate)
LOG_ADD_CODE(wSourceConvertFbxNoPolyPart)
LOG_ADD_CODE(wSourceConvertFbxExceedsIndices)
LOG_ADD_CODE(wSourceConvertFbxExceedsBones)
LOG_ADD_CODE(wSourceConvertFbxZeroWeights)
LOG_ADD_CODE(wSourceConvertFbxDuplicateMesh)
LOG_ADD_CODE(wSourceConvertFbxEmptyMeshpart)
LOG_ADD_CODE(wSourceConvertFbxNoMaterial)
LOG_ADD_CODE(wSourceConvertFbxNoPartMaterial)
LOG_ADD_CODE(wSourceConvertFbxMaterialNotFound)
LOG_ADD_CODE(wSourceConvertFbxMaterialUnsupported)
LOG_ADD_CODE(wSourceConvertFbxMaterialHLSL)
LOG_ADD_CODE(wSourceConvertFbxMaterialCgFX)
LOG_ADD_CODE(wSourceConvertFbxLayeredTexture)
LOG_ADD_CODE(wSourceConvertFbxSkipPropname)
LOG_ADD_CODE(wSourceConvertFbxInvalidMesh)
LOG_ADD_CODE(eSourceConvert)

LOG_ADD_CODE(sSourceClose)
LOG_ADD_CODE(eSourceClose)

LOG_ADD_CODE(sExportToG3DB)
LOG_ADD_CODE(sExportToG3DJ)
LOG_ADD_CODE(sExportClose)
LOG_ADD_CODE(eExportFiletypeUnknown)

LOG_ADD_CODE(iModelInfoNull)
LOG_ADD_CODE(iModelInfoStart)
LOG_ADD_CODE(iModelInfoID)
LOG_ADD_CODE(iModelInfoVersion)
LOG_ADD_CODE(iModelInfoMeshesSummary)
LOG_ADD_CODE(iModelInfoNodesSummary)
LOG_ADD_CODE(iModelInfoMaterialsSummary)

} }

#endif //FBXCONV_LOG_CODES_H