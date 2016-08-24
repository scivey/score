#include "asioz/ScopeGuard.h"

namespace asioz {

ScopeGuard::ScopeGuard(typename ScopeGuard::void_func &&func)
  : func_(func) {}

void ScopeGuard::dismiss() {
  dismissed_.store(true);
}

ScopeGuard::~ScopeGuard() {
  if (!dismissed_.load()) {
    func_();
  }
}

} // asioz
