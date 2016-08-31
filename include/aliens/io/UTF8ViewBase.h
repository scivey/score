#pragma once
#include <string>

namespace aliens { namespace io {

template<typename TSubType, typename TIter>
class UTF8ViewBase {
 public:
  using subtype = TSubType;
  using subtype_iterator = TIter;

 protected:
  const char *textStart_ {nullptr};
  const char *textEnd_ {nullptr};

 public:
  UTF8ViewBase(const std::string &text)
    : textStart_(text.data()),
      textEnd_(text.data() + text.size()) {}

  UTF8ViewBase(const char *text, size_t len)
    : textStart_(text),
      textEnd_(text + len) {}

  UTF8ViewBase(const char *start, const char *end)
    : textStart_(start),
      textEnd_(end) {}

  size_t bytes() const {
    uintptr_t startPtr = (uintptr_t) textStart_;
    uintptr_t endPtr = (uintptr_t) textEnd_;
    return endPtr - startPtr;
  }

  subtype_iterator begin() const {
    return subtype_iterator(textStart_, textEnd_, 0);
  }

  subtype_iterator end() const {
    return subtype_iterator(textEnd_, textEnd_, bytes());
  }
};


}} // aliens::io
