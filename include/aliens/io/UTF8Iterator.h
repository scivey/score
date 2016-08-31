#pragma once

#include <string>
#include "aliens/io/UTF8IteratorBase.h"

namespace aliens { namespace io {

class UTF8Iterator : public UTF8IteratorBase<UTF8Iterator, uint32_t> {
 public:
  using parent_type = UTF8IteratorBase<UTF8Iterator, uint32_t>;
  uint32_t dereference() const;

  template<typename ...Args>
  UTF8Iterator(Args... args)
    : parent_type(std::forward<Args>(args)...) {}
};

}} // aliens::io

