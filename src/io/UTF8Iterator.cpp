#include "score/io/UTF8Iterator.h"

namespace score { namespace io {

uint32_t UTF8Iterator::dereference() const {
  return vendored::utf8::unchecked::peek_next(position_);
}

}} // score::io
