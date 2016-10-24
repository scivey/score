#include "score/html/HTMLDoc.h"
#include <string>

using namespace std;

namespace score { namespace html {

using detail::GumboOutputWrapper;

HTMLDoc::HTMLDoc(const string &text, shared_ptr<GumboOutputWrapper> output)
  : text_(text), gumboOutput_(output) {}

Node HTMLDoc::root() const {
  return Node(gumboOutput_->get()->root);
}

HTMLDoc HTMLDoc::create(const string &text) {
  return HTMLDoc(text, GumboOutputWrapper::createShared(text));
}

}} // score::html