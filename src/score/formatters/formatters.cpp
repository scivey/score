#include "score/formatters/formatters.h"

namespace score { namespace formatters {

std::ostream& operator<<(std::ostream &oss, __uint128_t num128) {
  uint64_t low = (uint64_t) num128;
  uint64_t high = (num128 >> 64);
  oss << "__uint128_t(low=" << low << ", high=" << high << ")";
  return oss;
}

}} // score::formatters
