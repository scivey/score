#pragma once
#include "score/io/UTF8IndexView.h"
#include "score/io/UTF8IndexIterator.h"
#include <string>

namespace score { namespace nlp { namespace tokenize {

class TokenView {
 public:
  using index_view = io::UTF8IndexView;
  using index_iter = io::UTF8IndexIterator;

 protected:
  index_view utf8View_;

 public:
  TokenView(const std::string &text);
  class Iterator {
   protected:
    index_iter start_;
    index_iter end_;
   public:
    Iterator(index_iter start, index_iter end);
    bool operator==(const Iterator &other) const;
    bool operator!=(const Iterator &other) const;
    bool good();
    operator bool();
    std::pair<size_t, size_t> operator*();
    Iterator& operator++();
    Iterator operator++(int);
  };
  Iterator begin();
  Iterator end();
};

}}} // score::nlp::tokenize
