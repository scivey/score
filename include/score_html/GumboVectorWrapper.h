#pragma once

#include "score_html/vendored/gumbo-parser/gumbo.h"

namespace score { namespace html {

class GumboVectorWrapper {
 protected:
  const GumboVector *gumboVec_ {nullptr};
 public:
  GumboVectorWrapper();
  GumboVectorWrapper(const GumboVector *);
  bool good() const;
  operator bool() const;
  const GumboNode* at(size_t idx) const;
  const GumboNode* operator[](size_t idx) const;
  size_t size() const;

  class Iterator {
    const GumboVectorWrapper *parent_;
    size_t idx_;
   public:
    Iterator(const GumboVectorWrapper*, size_t);
    bool operator!=(const Iterator&other) const;
    Iterator& operator++();
    Iterator operator++(int);
    const GumboNode* operator*();
    const GumboNode* operator->();
  };
  friend class Iterator;

  Iterator begin() const;
  Iterator end() const;
};


}} // score::html

