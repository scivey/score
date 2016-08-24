#pragma once
#include <functional>
#include <array>

namespace aliens { namespace async {

template<size_t NCallbacks>
class CallbackArray {
 public:
  using void_func = std::function<void()>;
  static const size_t N = NCallbacks;
 protected:
  std::array<void_func, N> callbacks_;
};

}}