#pragma once
#include <tuple>
#include <vector>
#include <string>

namespace score { namespace hashing {

uint64_t cityHash64(const char* ptr, size_t textLen, uint64_t seed);
uint64_t cityHash64(const char* ptr, size_t textLen);
uint64_t cityHash64(const std::string&, uint64_t seed);
uint64_t cityHash64(const std::string&);
std::pair<uint64_t, uint64_t> cityHash128(const char *ptr, size_t textLen);
std::pair<uint64_t, uint64_t> cityHash128(const char *ptr, size_t textLen, std::pair<uint64_t, uint64_t> seed);
std::pair<uint64_t, uint64_t> cityHash128(const std::string&);
std::pair<uint64_t, uint64_t> cityHash128(const std::string&, std::pair<uint64_t, uint64_t> seed);
uint64_t city128To64(std::pair<uint64_t, uint64_t>);
uint64_t city128To64(uint64_t, uint64_t);

uint32_t murmur3_32(const char *ptr, size_t textLen);
uint32_t murmur3_32(const char *ptr, size_t textLen, uint32_t seed);
uint32_t murmur3_32(const std::string&);
uint32_t murmur3_32(const std::string&, uint32_t seed);
std::pair<uint64_t, uint64_t> murmur3_128(const char *ptr, size_t textLen);
std::pair<uint64_t, uint64_t> murmur3_128(const char *ptr, size_t textLen, uint32_t seed);
std::pair<uint64_t, uint64_t> murmur3_128(const std::string&);
std::pair<uint64_t, uint64_t> murmur3_128(const std::string&, uint32_t seed);

}} // score::hashing

