#pragma once
#include "score/encodings/HeapBufferHandle.h"
#include "score/encodings/Encoding.h"
#include <memory>
#include <exception>
#include "score/macros.h"

namespace score { namespace encodings {


struct IConvOptions {
  Encoding toEncoding {Encoding::UTF_8};
  Encoding fromEncoding {Encoding::UTF_8};
};

}} // score::encodings