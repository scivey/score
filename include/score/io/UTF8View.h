#pragma once

#include <string>
#include "score/io/UTF8Iterator.h"
#include "score/io/UTF8ViewBase.h"

namespace score { namespace io {

class UTF8View: public UTF8ViewBase<UTF8View, UTF8Iterator> {
 public:
  using iterator = UTF8Iterator;
  using parent_type = UTF8ViewBase<UTF8View, UTF8Iterator>;
  template<typename ...Args>
  UTF8View(Args... args)
    : parent_type(std::forward<Args>(args)...) {}
};

}} // score::io

