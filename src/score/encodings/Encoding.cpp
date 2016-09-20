#include "score/encodings/Encoding.h"

namespace score { namespace encodings {

std::string stringOfEncoding(Encoding enc) {
  switch(enc) {
    case Encoding::UTF8:
      return "utf8";
    case Encoding::GB18030:
      return "gb18030";
    case Encoding::ASCII:
      return "ascii";
    case Encoding::WINDOWS_1252:
      return "windows-1252";
    default:
      return "UNKNOWN_ENCODING";
  }
}

Optional<Encoding> encodingOfString(const std::string &label) {
  Optional<Encoding> result;
  if (label == "gb18030") {
    result.assign(Encoding::GB18030);
  } else if (label == "utf8") {
    result.assign(Encoding::UTF8);
  } else if (label == "ascii") {
    result.assign(Encoding::ASCII);
  } else if (label == "windows-1252") {
    result.assign(Encoding::WINDOWS_1252);
  }
  return result;
}

}} // score::encodings