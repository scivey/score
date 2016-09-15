#pragma once
#include <map>

namespace score { namespace iter {

template<typename TIter>
class TrailingIterator;

template<typename TCollection>
class IterTrailer {
 public:
  using iterator = TrailingIterator<typename TCollection::iterator>;
 protected:
  TCollection &ref_;
 public:
  IterTrailer(TCollection &ref): ref_(ref) {}
  iterator begin();
  iterator end();
};

template<typename TIter>
class TrailingIterator {
 public:
  using iter_type = TIter;
  using sub_value_type = typename TIter::value_type;
  using value_type = std::pair<
    typename TIter::value_type, typename TIter::value_type
  >;
 protected:
  value_type value_ {sub_value_type(), sub_value_type()};
  iter_type subIter_;

  TrailingIterator(iter_type &&subIter)
    : subIter_(std::forward<iter_type>(subIter)) {}

 public:

  TrailingIterator(value_type &&vals, iter_type &&subIter)
    : value_(std::forward<value_type>(vals)), subIter_(std::forward<iter_type>(subIter)){}
  TrailingIterator(sub_value_type val1, sub_value_type val2, iter_type &&subIter)
    : value_(std::make_pair(val1, val2)), subIter_(std::forward<iter_type>(subIter)) {}

  value_type operator*() {
    return value_;
  }
  value_type* operator->() {
    return &value_;
  }

  TrailingIterator& operator++() {
    ++subIter_;
    value_ = std::make_pair(value_.second, *subIter_);
    return *this;
  }
  TrailingIterator& operator++(int) {
    TrailingIterator result = *this;
    ++*this;
    return result;
  }
  bool operator!=(const TrailingIterator &other) const {
    return subIter_ != other.subIter_;
  }

  template<typename TCollection>
  static TrailingIterator create(TCollection &collection) {
    auto fst = collection.begin();
    auto last = collection.end();
    if (! (fst != last)) {
      return TrailingIterator(std::move(fst));
    }
    auto firstVal = *fst;
    auto snd = ++fst;
    return TrailingIterator(firstVal, *snd, std::move(snd));
  }

  template<typename TCollection>
  static TrailingIterator createEnd(TCollection &collection) {
    return TrailingIterator(collection.end());
  }
};


template<typename TCollection>
typename IterTrailer<TCollection>::iterator IterTrailer<TCollection>::begin() {
  return iterator::create(ref_);
}

template<typename TCollection>
typename IterTrailer<TCollection>::iterator IterTrailer<TCollection>::end() {
  return iterator::createEnd(ref_);
}



template<typename TCollection>
IterTrailer<TCollection> iterTrailing(TCollection &collection) {
  return IterTrailer<TCollection>(collection);
}

}} // score::iter

