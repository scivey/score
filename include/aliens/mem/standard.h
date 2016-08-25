#pragma once
#include <cstdint>
#include <cstddef>

namespace aliens { namespace mem {

namespace detail {

void* stdMallocImpl(size_t);
void* stdCallocImpl(size_t, size_t);
int stdPosixMemalignImpl(void**, size_t, size_t);
void* stdAlignedAllocImpl(size_t, size_t);
void* stdReallocImpl(void*, size_t);
void stdFreeImpl(void*);
} // detail

template<typename T>
class StandardAllocator {
 public:
  T* malloc(size_t n) const {
    return (T*) detail::stdMallocImpl(n);
  }
  T* calloc(size_t num, size_t size) const {
    return (T*) detail::stdCallocImpl(num, size);
  }
  int posixMemalign(T** ptr, size_t alignment, size_t size) const {
    return detail::stdPosixMemalignImpl((void**) ptr, alignment, size);
  }
  T* alignedAlloc(size_t alignment, size_t size) const {
    return (T*) detail::stdAlignedAllocImpl(alignment, size);
  }
  T* realloc(T* ptr, size_t size) const {
    return (T*) detail::stdReallocImpl(ptr, size);
  }
  void free(T *ptr) const {
    detail::stdFreeImpl((void*) ptr);
  }
};

}} // aliens::mem