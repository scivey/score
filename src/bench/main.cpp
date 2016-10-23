#include <vector>
#include <list>
#include <array>
#include <memory>
#include <random>
#include <thread>
#include <chrono>
#include <benchmark/benchmark.h>

using namespace std;

static void BM_nothing(benchmark::State& state) {
  while (state.KeepRunning()) {
    ;
  }
}
BENCHMARK(BM_nothing);

BENCHMARK_MAIN();
