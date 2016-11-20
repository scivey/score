#include "score/io/string_utils.h"
#include <re2/re2.h>
#include <set>

using namespace std;

namespace score { namespace io {

static const RE2 TRIM_WHITESPACE_REGEX {
  "^\\s*(\\S.*\\S)?\\s*$"
};

static const set<char> punctuation {
  '.', '"', '!', '/', '\\', ':', '?', '@', '[', ']', '(', ')', '{', '}', '#', '$', '%', '^', '&',
  ';', ','
};

std::string trimWhitespace(const string& text) {
  re2::StringPiece target {text.c_str(), (int) text.size()};
  std::string result;
  if (RE2::PartialMatch(text, TRIM_WHITESPACE_REGEX, &result)) {
    return result;
  }
  return text;
}

bool isAsciiPunctuation(char c) {
  return punctuation.count(c) > 0;
}

}} // score::io
