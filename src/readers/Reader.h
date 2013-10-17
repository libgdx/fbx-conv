#ifdef _MSC_VER
#pragma once
#endif //_MSC_VER
#ifndef FBXCONV_READERS_READER_H
#define FBXCONV_READERS_READER_H

#include "../modeldata/Model.h"

using namespace fbxconv::modeldata;

namespace fbxconv {
namespace readers {

class Reader {
public:
	virtual ~Reader() {}
	virtual bool load(Settings *settings) = 0;
	virtual bool convert(Model * const &model) = 0;
};

} // readers
} // fbxconv

#endif //FBXCONV_READERS_READER_H