#include "aliens/FixedBuffer.h"
#include <sstream>

namespace aliens { namespace detail {

std::string buffToString(char *buff, size_t buffLen) {
  std::ostringstream result;
  char *current = buff;
  for (size_t i = 0; i < buffLen; i++) {
    char c = *current;
    if (c == '\0') {
      break;
    }
    if (c == '\n') {
      if (i < (buffLen - 1)) {
        result << c;
      } else {
        break;
      }
    } else {
      result << c;
    }
    current++;
  }
  return result.str();
}

}} // aliens::detail
