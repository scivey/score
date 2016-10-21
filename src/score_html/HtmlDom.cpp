#include "score_html/HtmlDom.h"
#include <string>

using namespace std;

namespace score { namespace html {

HtmlDom::HtmlDom(const string &text, shared_ptr<GumboOutputWrapper> output)
  : text_(text), gumboOutput_(output) {}

Node HtmlDom::root() const {
  return Node(gumboOutput_->get()->root);
}

HtmlDom HtmlDom::create(const string &text) {
  return HtmlDom(text, GumboOutputWrapper::createShared(text));
}

}} // score::html