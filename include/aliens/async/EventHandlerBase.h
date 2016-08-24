#pragma once
#include <thread>
#include <string>
#include <sstream>
#include <memory>
#include <glog/logging.h>
#include <boost/asio.hpp>
#include "aliens/FixedBuffer.h"

namespace aliens { namespace async {

template<typename TParent>
class EventHandlerBase {
 public:
  using parent_type = TParent;

  friend parent_type;

 protected:
  TParent *parent_ {nullptr};

  void setParent(TParent *parent) {
    parent_ = parent;
  }

  TParent* getParent() const {
    return parent_;
  }
 public:
  EventHandlerBase(){}

  bool hasParent() const {
    return !!parent_;
  }
};



}} // aliens::async
