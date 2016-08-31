#include "score/hashing/util.h"

namespace score { namespace hashing {

// stolen from boost::hash
size_t hashCombine(size_t seed, size_t hashedVal) {
  seed ^= hashedVal + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  return seed;
}

}} // score::hashing
