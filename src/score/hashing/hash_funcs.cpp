#include "score/hashing/hash_funcs.h"
#include "score/vendored/smhasher/City.h"
#include "score/vendored/smhasher/MurmurHash3.h"

#include <array>

using namespace std;
using namespace score::vendored::smhasher;

namespace score { namespace hashing {

uint64_t cityHash64(const char* ptr, size_t textLen, uint64_t seed) {
  return CityHash64WithSeed(ptr, textLen, seed);
}
uint64_t cityHash64(const char* ptr, size_t textLen) {
  return CityHash64(ptr, textLen);
}

uint64_t cityHash64(const std::string &text, uint64_t seed) {
  return cityHash64(text.data(), text.size(), seed);
}
uint64_t cityHash64(const std::string& text) {
  return cityHash64(text.data(), text.size());
}

pair<uint64_t, uint64_t> cityHash128(const char *ptr, size_t textLen) {
  return CityHash128(ptr, textLen);
}

pair<uint64_t, uint64_t> cityHash128(const char *ptr, size_t textLen, pair<uint64_t, uint64_t> seed) {
  return CityHash128WithSeed(ptr, textLen, seed);
}

pair<uint64_t, uint64_t> cityHash128(const std::string &text) {
  return cityHash128(text.data(), text.size());
}

pair<uint64_t, uint64_t> cityHash128(const std::string &text, pair<uint64_t, uint64_t> seed) {
  return cityHash128(text.data(), text.size(), seed);
}
uint64_t city128To64(pair<uint64_t, uint64_t> value) {
  return Hash128to64(value);
}

uint64_t city128To64(uint64_t x, uint64_t y) {
  return Hash128to64(std::make_pair(x, y));
}

uint32_t murmur3_32(const char *text, size_t textLen, uint32_t seed) {
  uint32_t result = 0;
  MurmurHash3_x86_32((void*) text, textLen, seed, (void*) &result);
  return result;
}

uint32_t murmur3_32(const char *text, size_t textLen) {
  return murmur3_32(text, textLen, 0);
}

uint32_t murmur3_32(const string &text) {
  return murmur3_32(text.data(), text.size());
}

uint32_t murmur3_32(const string &text, uint32_t seed) {
  return murmur3_32(text.data(), text.size(), seed);
}

pair<uint64_t, uint64_t> murmur3_128(const char *ptr, size_t textLen, uint32_t seed) {
  array<uint64_t, 2> result;
  MurmurHash3_x64_128((void*) ptr, textLen, seed, (void*) &result);
  return std::make_pair(result[0], result[1]);
}

pair<uint64_t, uint64_t> murmur3_128(const char *ptr, size_t textLen) {
  return murmur3_128(ptr, textLen, 0);
}

pair<uint64_t, uint64_t> murmur3_128(const string &text) {
  return murmur3_128(text.data(), text.size());
}

pair<uint64_t, uint64_t> murmur3_128(const string &text, uint32_t seed) {
  return murmur3_128(text.data(), text.size(), seed);
}

}} // score::hashing
