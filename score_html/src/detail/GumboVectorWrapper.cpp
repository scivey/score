#include "score/html/detail/GumboVectorWrapper.h"
#include "score/html/vendored/gumbo-parser/gumbo.h"

namespace score { namespace html { namespace detail {

using VecIter = typename GumboVectorWrapper::Iterator;

GumboVectorWrapper::GumboVectorWrapper(const GumboVector *gumboVec)
  : gumboVec_(gumboVec){}

GumboVectorWrapper::GumboVectorWrapper()
  : gumboVec_(nullptr){}

size_t GumboVectorWrapper::size() const {
  if (!gumboVec_) {
    return 0;
  }
  return gumboVec_->length;
}

const GumboNode* GumboVectorWrapper::at(size_t idx) const {
  if (idx >= size()) {
    return nullptr;
  }
  return static_cast<const GumboNode*>(gumboVec_->data[idx]);
}

const GumboNode* GumboVectorWrapper::operator[](size_t idx) const {
  return at(idx);
}

VecIter GumboVectorWrapper::begin() const {
  return VecIter(this, 0);
}

VecIter GumboVectorWrapper::end() const {
  return VecIter(this, size());
}

VecIter::Iterator(const GumboVectorWrapper *parent, size_t idx)
  : parent_(parent), idx_(idx){}

VecIter VecIter::operator++(int) {
  VecIter result(parent_, idx_);
  ++result;
  return result;
}

VecIter& VecIter::operator++() {
  idx_++;
  return *this;
}

const GumboNode* VecIter::operator*() {
  return parent_->at(idx_);
}

const GumboNode* VecIter::operator->() {
  return parent_->at(idx_);
}

bool VecIter::operator!=(const VecIter &other) const {
  return idx_ != other.idx_;
}


}}} // score::html::detail
