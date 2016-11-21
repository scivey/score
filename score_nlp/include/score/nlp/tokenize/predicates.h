#pragma once
#include <string>
#include "score/io/ByteStringPiece.h"
#include "score/nlp/Language.h"
#include "score/nlp/UTF8UTextRef.h"
#include "score/nlp/tokenize/ICUToken.h"

namespace score { namespace nlp { namespace tokenize {

namespace detail {
bool isWordTokenImpl(const char *data, size_t dataLen);
} // detail

template<typename T,
  typename = decltype(std::declval<T>().c_str()),
  typename = decltype(std::declval<T>().size())
>
bool isWordToken(const T& strRef) {
  return detail::isWordTokenImpl(strRef.c_str(), strRef.size());
}

bool isWordToken(const ICUToken& toke);


}}} // score::nlp::tokenize
