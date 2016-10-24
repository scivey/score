#pragma once

#include <string>
#include "score/io/UTF8IteratorBase.h"

namespace score { namespace io {

class UTF8IndexIterator : public UTF8IteratorBase<UTF8IndexIterator,
    std::pair<size_t, uint32_t>>  {
 protected:
  using deref_result = std::pair<size_t, uint32_t>;
 public:
  using parent_type = UTF8IteratorBase<UTF8IndexIterator, deref_result>;
  deref_result dereference() const;
  template<typename ...Args>
  UTF8IndexIterator(Args... args)
    : parent_type(std::forward<Args>(args)...) {}
};

}} // score::io

