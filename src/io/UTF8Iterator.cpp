#include "aliens/io/UTF8Iterator.h"

namespace aliens { namespace io {

uint32_t UTF8Iterator::dereference() const {
  return vendored::utf8::unchecked::peek_next(position_);
}

}} // aliens::io
