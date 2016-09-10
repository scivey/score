#include "score/html/GumboOutputWrapper.h"
#include <string>

using namespace std;

namespace score { namespace html {

GumboOutputWrapper::GumboOutputWrapper(GumboOutput *output)
  : output_(output){}

GumboOutputWrapper::~GumboOutputWrapper() {
  if (output_) {
    gumbo_destroy_output(&kGumboDefaultOptions, output_);
    output_ = nullptr;
  }
}

GumboOutputWrapper::GumboOutputWrapper(GumboOutputWrapper &&other) {
  output_ = other.output_;
  other.output_ = nullptr;
}

GumboOutput* GumboOutputWrapper::get() const {
  return output_;
}

GumboOutput* GumboOutputWrapper::operator->() const {
  return output_;
}

GumboOutputWrapper GumboOutputWrapper::create(
    const string &htmlStr) {
  auto output = gumbo_parse(htmlStr.c_str());
  return GumboOutputWrapper(output);
}

std::shared_ptr<GumboOutputWrapper> GumboOutputWrapper::createShared(
    const string &htmlStr) {
  auto output = gumbo_parse(htmlStr.c_str());
  return std::make_shared<GumboOutputWrapper>(output);
}


}} // score::html