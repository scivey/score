#pragma once
#include <string>
#include "score/Try.h"
#include "score/encodings/EncodingException.h"

namespace score { namespace encodings {

#include "score/encodings/detail/Encoding-list.h"

#define X(encoding_sym, encoding_name) encoding_sym
enum class Encoding {
  SCORE_ENCODING_LIST_DETAIL
};
#undef X
#undef SCORE_ENCODING_LIST_DETAIL

const char* cStringOfEncoding(Encoding enc);
std::string stringOfEncoding(Encoding enc);

Try<Encoding> encodingFromName(const char *buff, size_t buffLen);
Try<Encoding> encodingFromName(const char *buff);

template<typename TStr>
Try<Encoding> encodingFromName(const TStr &aString) {
  return encodingFromName(aString.c_str(), aString.size());
}


}} // score::encodings

