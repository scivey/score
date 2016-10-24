#pragma once

namespace score { namespace iter {

template<typename TIterable, typename TTarget>
void drainIterable(const TIterable &iterable, TTarget &target) {
  for (auto elem: iterable) {
    target.push_back(elem);
  }
}

}}