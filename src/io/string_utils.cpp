#include "score/io/string_utils.h"
#include <set>

using namespace std;

namespace score { namespace io {

static const set<char> punctuation {
  '.', '"', '!', '/', '\\', ':', '?', '@', '[', ']', '(', ')', '{', '}', '#', '$', '%', '^', '&',
  ';', ','
};

bool isAsciiPunctuation(char c) {
  return punctuation.count(c) > 0;
}

}} // score::io
