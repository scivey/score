#pragma once
#include "score/encodings/Encoding.h"

namespace score { namespace encodings {

class EncodingNormalizer {
 protected:
  Encoding targetEncoding_;
 public:
  EncodingNormalizer(Encoding target = Encoding::UTF8);
  std::string normalize(const std::string &text);
};


}} // score::encodings