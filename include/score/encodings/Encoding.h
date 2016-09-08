#pragma once
#include <string>
#include "score/Maybe.h"

namespace score { namespace encodings {

enum class Encoding {
  UTF8, GB18030, ASCII, WINDOWS_1252
};

std::string stringOfEncoding(Encoding enc);
Maybe<Encoding> encodingOfString(const std::string&);


}} // score::encodings

