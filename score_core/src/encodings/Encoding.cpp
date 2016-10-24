#include "score/encodings/Encoding.h"
#include "score/encodings/EncodingException.h"
#include "score/ExceptionWrapper.h"
#include "score/io/Scstring.h"
#include "score/macros.h"

#include <map>

using std::map;
using std::ostringstream;

using score::encodings::Encoding;
using score::io::Scstring;

#include "score/encodings/detail/Encoding-list.h"
#define X(encoding_sym, encoding_name) \
  { Encoding::encoding_sym, encoding_name }

map<Encoding, const char*> encodingToStringMap {
  SCORE_ENCODING_LIST_DETAIL
};

#undef X

#define X(encoding_sym, encoding_name) \
  { score::io::Scstring { encoding_name }, Encoding::encoding_sym }

map<score::io::Scstring, Encoding> stringToEncodingMap {
  SCORE_ENCODING_LIST_DETAIL
};

#undef X
#undef SCORE_ENCODING_LIST_DETAIL

namespace score { namespace encodings {


const char* cStringOfEncoding(Encoding enc) {
  auto found = encodingToStringMap.find(enc);
  SDCHECK(found != encodingToStringMap.end());
  return found->second;
}

std::string stringOfEncoding(Encoding enc) {
  return std::string(cStringOfEncoding(enc));
}

Try<Encoding> encodingFromName(const char *buff, size_t buffLen) {
  Scstring toFind {buff, buffLen};
  auto found = stringToEncodingMap.find(toFind);
  if (found == stringToEncodingMap.end()) {
    ostringstream msg;
    msg << "Could not find encoding : '" << toFind << "'";
    return Try<Encoding>{ makeExceptionWrapper<BadEncodingName>(msg.str()) };
  }
  return Try<Encoding> {found->second};
}

}} // score::encodings