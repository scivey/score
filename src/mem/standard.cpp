#include "score/mem/standard.h"
#include <cstdlib>
namespace score { namespace mem {

namespace detail {

void* stdMallocImpl(size_t n) {
  return malloc(n);
}
void* stdCallocImpl(size_t num, size_t size) {
  return calloc(num, size);
}
int stdPosixMemalignImpl(void** ptr, size_t alignment, size_t size) {
  return posix_memalign(ptr, alignment, size);
}
void* stdAlignedAllocImpl(size_t alignment, size_t size) {
  return aligned_alloc(alignment, size);
}
void* stdReallocImpl(void* ptr, size_t size) {
  return realloc(ptr, size);
}
void stdFreeImpl(void *ptr) {
  free(ptr);
}

} // detail

}} // score::mem