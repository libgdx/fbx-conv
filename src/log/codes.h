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
LOG_ADD_CODE(wSourceConvertFbxExceedsIndices)
LOG_ADD_CODE(wSourceConvertFbxExceedsBones)
LOG_ADD_CODE(eSourceConvert)
LOG_ADD_CODE(eSourceConvertFbxNoMaterial)

LOG_ADD_CODE(sSourceClose)
LOG_ADD_CODE(eSourceClose)

LOG_ADD_CODE(sExportToG3DB)
LOG_ADD_CODE(sExportToG3DJ)
LOG_ADD_CODE(sExportClose)
LOG_ADD_CODE(eExportFiletypeUnknown)

} }

#endif //FBXCONV_LOG_CODES_H