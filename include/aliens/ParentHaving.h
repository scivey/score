#pragma once

namespace aliens {

template<typename T>
class ParentHaving {
 public:
  using parent_type = T;
 protected:
  parent_type *parent_ {nullptr};
 public:
  void setParent(T *parent) {
    parent_ = parent;
  }
  T* getParent() const {
    return parent_;
  }
  bool hasParent() const {
    return !!parent_;
  }
};

} // aliens
