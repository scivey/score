#pragma once

#include <string>
#include "score/io/UTF8IndexIterator.h"
#include "score/io/UTF8ViewBase.h"

namespace score { namespace io {

class UTF8IndexView: public UTF8ViewBase<
    UTF8IndexView, UTF8IndexIterator> {
 public:
  using iterator = UTF8IndexIterator;
  using parent_type = UTF8ViewBase<UTF8IndexView, UTF8IndexIterator>;

  template<typename ...Args>
  UTF8IndexView(Args... args)
    : parent_type(std::forward<Args>(args)...) {}
};

}} // score::io


