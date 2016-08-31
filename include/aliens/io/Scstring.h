#pragma once

#include <string>
#include <sstream>
#include "aliens/vendored/seastar/core/sstring.hh"

namespace aliens { namespace io {

template<typename TChar, typename SizeType, SizeType MaxSize>
class ScstringBase : public vendored::seastar::basic_sstring<TChar, SizeType, MaxSize> {
 public:
  using parent_type = vendored::seastar::basic_sstring<TChar, SizeType, MaxSize>;

  template<typename ...Args>
  ScstringBase(Args&& ...args)
    : parent_type(std::forward<Args>(args)...){}

  std::string copyToStdString() const {
    std::ostringstream oss;
    for (auto c: *this) {
      oss << c;
    }
    return oss.str();
  }
  int compare(const TChar* strData, SizeType strLen) const {
    return strncmp(this->data(), strData, std::min(this->size(), strLen));
  }
  bool operator==(const std::string& other) const {
    return compare(other.data(), other.size()) == 0;
  }
  bool operator!=(const std::string& other) const {
    return compare(other.data(), other.size()) != 0;
  }
};

using Scstring = ScstringBase<char, uint32_t, 31>;

}} // aliens::io


namespace std {

template<typename TChar, typename TSize, TSize MaxSize, typename TOther>
bool operator==(const aliens::io::ScstringBase<TChar, TSize, MaxSize> &scStr,
    const TOther &other) {
  return scStr == other;
}
}
