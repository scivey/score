#include "score/io/UTF8IndexIterator.h"
#include "score/vendored/utf8/utf8/unchecked.h"

using namespace std;

namespace score { namespace io {

pair<size_t, uint32_t> UTF8IndexIterator::dereference() const {
  return std::make_pair(
    distanceFromStart_,
    vendored::utf8::unchecked::peek_next(position_)
  );
}

}} // score::io
