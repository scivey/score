#pragma once
#include <vector>
#include "score/mem/st_shared_ptr.h"
#include "score_async/futures/st/STPromise.h"
#include "score_async/futures/st/STFuture.h"

namespace score { namespace async { namespace futures { namespace st {

template<typename T>
STFuture<T> makeReadySTFuture(T value) {
  auto prom = score::mem::make_st_shared<STPromise<T>>(STPromise<T>::create());
  prom->setValue(value);
  return prom->getFuture();
}


namespace detail {

template<typename T>
class STCollectAllContext: public std::enable_shared_from_this<STCollectAllContext<T>> {
 protected:
  score::mem::st_shared_ptr<STPromise<std::vector<T>>> doneSTPromise;
  std::vector<T> results;
  std::vector<STFuture<T>> inputs;
  size_t total {0};
 public:
  static score::mem::st_shared_ptr<STCollectAllContext> createShared(
      std::vector<STFuture<T>>&& inputVec) {
    auto newCtx = score::mem::make_st_shared<STCollectAllContext>();
    newCtx->inputs = std::move(inputVec);
    newCtx->doneSTPromise = score::mem::make_st_shared<STPromise<std::vector<T>>>();
    newCtx->total = newCtx->inputs.size();
    return newCtx;
  }
  void run() {
    for (auto& future: inputs) {
      auto self = this->shared_from_this();
      future.then([this, self](T result) {
        this->results.push_back(result);
        if (this->results.size() == this->total) {
          this->doneSTPromise->setValue(std::move(results));
        }
      });
    }
  }
  STFuture<std::vector<T>> getFuture() {
    return doneSTPromise->getFuture();
  }
};

} // detail


template<typename T>
STFuture<std::vector<T>> collectAll(std::vector<STFuture<T>> &&futures) {
  auto ctx = detail::STCollectAllContext<T>::createShared(std::move(futures));
  ctx->run();
  return ctx->getFuture();
}


}}}} // score::async::futures::st
