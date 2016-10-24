#pragma once
#include "score/html/vendored/gumbo-parser/gumbo.h"
#include <string>
#include <memory>

namespace score { namespace html { namespace detail {

class GumboOutputWrapper {
 protected:
  GumboOutput *output_;
  GumboOutputWrapper(GumboOutput other) = delete;
  GumboOutputWrapper(const GumboOutput &other) = delete;
  void operator=(GumboOutput other) = delete;
  void operator=(const GumboOutput &other) = delete;
 public:

  static GumboOutputWrapper create(const std::string&);
  static std::shared_ptr<GumboOutputWrapper> createShared(const std::string&);
  GumboOutputWrapper(GumboOutputWrapper &&other);
  GumboOutputWrapper(GumboOutput *output);
  GumboOutput* get() const;
  GumboOutput* operator->() const;
  ~GumboOutputWrapper();
};

}}} // score::html::detail