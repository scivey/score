#pragma once
#include <string>
#include "score/Optional.h"
#include "score/encodings/Encoding.h"
#include "score/Try.h"

namespace score { namespace encodings {

const char *detectEncodingStr(const std::string &text);
Try<Encoding> detectEncoding(const std::string &text);

}} // score::encodings
