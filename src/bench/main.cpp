#include <vector>
#include <list>
#include <array>
#include <memory>
#include <random>
#include <benchmark/benchmark.h>

static void BM_nothing(benchmark::State& state) {
  while (state.KeepRunning()) {
    this_thread::sleep_for(chrono::milliseconds(10));
  }
}
BENCHMARK(BM_nothing);

BENCHMARK_MAIN();
