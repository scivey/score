#pragma once
#include <string>
#include <memory>

struct GumboInternalOutput;

namespace score { namespace html { namespace detail {

class GumboOutputWrapper {
 protected:
  GumboInternalOutput *output_;
  GumboOutputWrapper(const GumboOutputWrapper&) = delete;
  void operator=(const GumboOutputWrapper&) = delete;
 public:
  static GumboOutputWrapper create(const std::string&);
  static std::shared_ptr<GumboOutputWrapper> createShared(const std::string&);
  GumboOutputWrapper(GumboOutputWrapper &&other);
  GumboOutputWrapper(GumboInternalOutput *output);
  GumboInternalOutput* get() const;
  GumboInternalOutput* operator->() const;
  ~GumboOutputWrapper();
};

}}} // score::html::detail