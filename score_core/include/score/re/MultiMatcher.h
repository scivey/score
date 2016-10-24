#pragma once
#include <string>
#include <set>
#include <memory>
#include <re2/re2.h>

namespace score { namespace re {

class MultiMatcher {
 public:
  using string_t = std::string;
  using string_set_t = std::set<string_t>;
 protected:
  re2::RE2 re_;
  MultiMatcher(const MultiMatcher&) = delete;
  MultiMatcher& operator=(const MultiMatcher&) = delete;
 public:
  MultiMatcher(const string_t &reText);
  static std::shared_ptr<MultiMatcher> fromWordSet(const string_set_t& words);
  bool matches(const string_t& word);
};

}} // score::re
