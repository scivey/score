#pragma once
#include <string>
#include "score/Optional.h"
#include "score/encodings/Encoding.h"

namespace score { namespace encodings {

const char *detectEncodingStr(const std::string &text);
Optional<Encoding> detectEncoding(const std::string &text);

}} // score::encodings
