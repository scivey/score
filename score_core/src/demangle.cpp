#include "score/demangle.h"
#include "score/vendored/folly/Demangle.h"

namespace score {

std::string demangle(const std::type_info& type) {
  return score::vendored::folly::demangle(type);
}

} // score
