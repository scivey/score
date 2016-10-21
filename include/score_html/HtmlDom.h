#pragma once
#include <string>
#include <memory>
#include "score_html/GumboOutputWrapper.h"
#include "score_html/Node.h"

namespace score { namespace html {

class HtmlDom {
 protected:
  const std::string &text_;
  std::shared_ptr<GumboOutputWrapper> gumboOutput_ {nullptr};
 public:
  HtmlDom(const std::string&, std::shared_ptr<GumboOutputWrapper>);
  static HtmlDom create(const std::string&);
  Node root() const;
};

}} // score::html
