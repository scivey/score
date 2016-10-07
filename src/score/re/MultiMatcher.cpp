#include "score/re/MultiMatcher.h"
#include <string>
#include <set>
#include <re2/re2.h>
#include <glog/logging.h>

namespace score { namespace re {

using string_t = MultiMatcher::string_t;
using string_set_t = MultiMatcher::string_set_t;

MultiMatcher::MultiMatcher(const string_t &reText): re_(reText)  {}

std::shared_ptr<MultiMatcher> MultiMatcher::fromWordSet(const string_set_t& words) {
  CHECK(!words.empty());
  std::ostringstream reBuff;
  size_t lastIdx = words.size() - 1;
  size_t i = 0;
  for (const auto& word: words) {
    reBuff << RE2::QuoteMeta(word);
    if (i < lastIdx) {
      reBuff << "|";
    }
    i++;
  }
  auto result = std::make_shared<MultiMatcher>(reBuff.str());
  CHECK(result->re_.ok());
  return result;
}

bool MultiMatcher::matches(const string_t& word) {
  return RE2::FullMatch(word, re_);
}

}} // score::re
