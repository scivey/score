#pragma once
#include <string>
#include <memory>
#include "score_html/detail/GumboOutputWrapper.h"
#include "score_html/Node.h"

namespace score { namespace html {

class HTMLDoc {
 protected:
  const std::string &text_;
  std::shared_ptr<detail::GumboOutputWrapper> gumboOutput_ {nullptr};
 public:
  HTMLDoc(const std::string&, std::shared_ptr<detail::GumboOutputWrapper>);
  static HTMLDoc create(const std::string&);
  Node root() const;
};

}} // score::html
