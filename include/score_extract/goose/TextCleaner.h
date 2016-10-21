#pragma once

#include <regex>
#include <string>
#include "score_html/Node.h"

namespace score { namespace extract { namespace goose {

class TextCleaner {
 protected:
  std::regex badClasses_;
  bool isBadTextNode(const html::Node &node);
 public:
  TextCleaner();
  std::string getText(const html::Node &node);
  size_t getText(const html::Node &node, std::ostringstream&);
};

}}} // score::extract::goose
