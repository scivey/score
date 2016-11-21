#include "score/nlp/tokenize/predicates.h"
#include "score/nlp/Language.h"
#include "score/nlp/UTF8UTextRef.h"
#include "score/io/UTF8View.h"
#include "score/unicode/support.h"

using namespace std;

namespace score { namespace nlp { namespace tokenize {

using io::UTF8View;
using io::ByteStringPiece;

namespace detail {

bool isWordTokenImpl(const char *data, size_t dataLen) {
  if ((data == nullptr) || (dataLen == 0)) {
    return false;
  }
  UTF8View uView {data, dataLen};
  auto start = uView.begin();
  if (start == uView.end()) {
    return false;
  }
  uint32_t codePoint = *start;
  return unicode::isLetterPoint(codePoint);
}

} // detail
bool isWordToken(const ICUToken& toke) {
  return isWordToken<io::ByteStringPiece>(toke.toByteStringPiece());
}


}}} // score::nlp::tokenize
