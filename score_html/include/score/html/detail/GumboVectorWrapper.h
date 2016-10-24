#pragma once

#include <sys/types.h>

struct GumboVector_s;
struct GumboInternalNode;

namespace score { namespace html { namespace detail {

class GumboVectorWrapper {
 protected:
  const GumboVector_s *gumboVec_ {nullptr};
 public:
  GumboVectorWrapper();
  GumboVectorWrapper(const GumboVector_s*);
  bool good() const;
  operator bool() const;
  const GumboInternalNode* at(size_t idx) const;
  const GumboInternalNode* operator[](size_t idx) const;
  size_t size() const;

  class Iterator {
    const GumboVectorWrapper *parent_;
    size_t idx_;
   public:
    Iterator(const GumboVectorWrapper*, size_t);
    bool operator!=(const Iterator&other) const;
    Iterator& operator++();
    Iterator operator++(int);
    const GumboInternalNode* operator*();
    const GumboInternalNode* operator->();
  };
  friend class Iterator;

  Iterator begin() const;
  Iterator end() const;
};


}}} // score::html::detail

