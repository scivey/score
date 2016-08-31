#include <vector>
#include <list>
#include <array>
#include <memory>
#include <random>
#include <benchmark/benchmark.h>
#include "score/mem/standard.h"

using score::mem::StandardAllocator;

const size_t kSmallBatchSize = 10000;
const size_t kSmallBatchBlockSize = 2e16;

static void BM_Standard1(benchmark::State& state) {
  StandardAllocator<int64_t> allocator;
  std::array<int64_t*, kSmallBatchSize> allocatedBuffers;
  while (state.KeepRunning()) {
    for (size_t i = 0; i < kSmallBatchSize; i++) {
      allocatedBuffers[i] = allocator.malloc(kSmallBatchBlockSize);
    }
    for (size_t i = 0; i < kSmallBatchSize; i++) {
      allocator.free(allocatedBuffers[i]);
      allocatedBuffers[i] = nullptr;
    }
  }
}
BENCHMARK(BM_Standard1);

static void BM_Better1(benchmark::State& state) {
  StandardAllocator<int64_t> allocator;
  const size_t kBlockSize = 2e20;
  std::array<int64_t*, kSmallBatchSize> allocatedBuffers;
  while (state.KeepRunning()) {
    for (size_t i = 0; i < kSmallBatchSize; i++) {
      allocatedBuffers[i] = allocator.malloc(kBlockSize);
    }
    for (size_t i = 0; i < kSmallBatchSize; i++) {
      allocator.free(allocatedBuffers[i]);
      allocatedBuffers[i] = nullptr;
    }
  }
}
BENCHMARK(BM_Better1);

BENCHMARK_MAIN();
