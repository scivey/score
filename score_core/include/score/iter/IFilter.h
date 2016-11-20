#pragma once
#include <functional>
#include "score/func/Function.h"

namespace score { namespace iter {

template<typename TIter,
  typename = typename TIter::value_type>
class IFilterer {
 public:
  using sub_iter_t = TIter;
  using value_type = typename TIter::value_type;
  using filter_fn_t = score::func::Function<bool, value_type>;

 protected:
  filter_fn_t filterFunc_;
  sub_iter_t startIt_;
  sub_iter_t endIt_;
 public:
  IFilterer(filter_fn_t func, sub_iter_t&& startIt, sub_iter_t&& endIt)
    : filterFunc_(func), startIt_(startIt), endIt_(endIt) {}

  class Iterator {
   protected:
    filter_fn_t func_;
    sub_iter_t currentIter_;
    sub_iter_t endIter_;
    void advanceUntilPassing() {
      while (currentIter_ != endIter_) {
        if (func_(*currentIter_)) {
          break;
        }
        currentIter_++;
      }
    }
   public:
    Iterator(filter_fn_t filterFunc, sub_iter_t current, sub_iter_t endIt)
      : func_(filterFunc),
        currentIter_(current),
        endIter_(endIt) {
      advanceUntilPassing();
    }

    Iterator& operator++() {
      currentIter_++;
      advanceUntilPassing();
      return *this;
    }

    Iterator operator++(int) {
      Iterator other = *this;
      ++*this;
      return other;
    }

    value_type operator*() {
      return *currentIter_;
    }

    bool operator!=(const Iterator& other) const {
      return currentIter_ != other.currentIter_;
    }
  };

  using iterator = Iterator;

  iterator begin() const {
    return Iterator {filterFunc_, startIt_, endIt_};
  }

  iterator end() const {
    return Iterator {filterFunc_, endIt_, endIt_};
  }
};



namespace detail {

template<typename TCollection>
struct IFiltererCollectionFactory {
  using iter_t = decltype(std::declval<TCollection>().begin());
  using value_type = typename TCollection::value_type;
  using func_t = func::Function<bool, value_type>;
  static IFilterer<iter_t> build(func_t filterFunc, const TCollection& target) {
    return IFilterer<iter_t> {filterFunc, target.begin(), target.end()};
  }
};

} // detail


template<typename TCollection,
  typename = decltype(std::declval<TCollection>().begin())>
IFilterer<typename TCollection::iterator> ifilter(
    func::Function<bool, typename TCollection::value_type> filterFunc,
    const TCollection& collection) {
  return detail::IFiltererCollectionFactory<TCollection>::build(
    filterFunc, collection
  );
}

template<typename TIter>
IFilterer<TIter> ifilter(
    func::Function<bool, typename TIter::value_type> filterFunc,
    TIter&& start, TIter&& end) {
  return IFilterer<TIter> {
    filterFunc,
    std::forward<TIter>(start),
    std::forward<TIter>(end)
  };
}

}} // score::iter

