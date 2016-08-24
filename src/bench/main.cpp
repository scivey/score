#include <vector>
#include <list>
#include <array>
#include <memory>
#include <random>

#include <benchmark/benchmark.h>

class SingletonIntDistribution {
 protected:
  std::random_device rd_;
  std::mt19937 engine_;
  std::uniform_int_distribution<int> distribution_ {0, 1000};
  SingletonIntDistribution() {
    engine_.seed(rd_());
  }
 public:
  static SingletonIntDistribution* instance() {
    static SingletonIntDistribution pInstance;
    return &pInstance;
  }
  int getInt() {
    return distribution_(engine_);
  }
  static int get() {
    return instance()->getInt();
  }
};

using Distrib = SingletonIntDistribution;
const size_t kNIter = 50;


template<typename T, size_t N>
class FixedBuffer {
 protected:
  std::array<T, N> data_;
  size_t offset_ {0};
 public:
  void clear() {
    offset_ = 0;
  }
  size_t capacity() const {
    return N;
  }
  size_t size() const {
    return offset_;
  }
  size_t nAvailable() const {
    return capacity() - size();
  }
  void push_back(const T &item) {
    data_[offset_] = item;
    offset_++;
  }
};



static void BM_NoReservation(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<float> nums;
    int j = 0;
    for (size_t i = 0; i < kNIter; i++) {
      instance.push_back(j);
      j++;
    }
  }
}

BENCHMARK(BM_NoReservation);


static void BM_Reservation(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::vector<int> instance;
    instance.reserve(kNIter);
    int j = 0;
    for (size_t i = 0; i < kNIter; i++) {
      instance.push_back(j);
      j++;
    }
  }
}

BENCHMARK(BM_Reservation);



static void BM_FixedBuffer(benchmark::State& state) {
  while (state.KeepRunning()) {
    FixedBuffer<int, kNIter> instance;
    int j = 0;
    for (size_t i = 0; i < kNIter; i++) {
      instance.push_back(j);
      j++;
    }
  }
}

BENCHMARK(BM_FixedBuffer);


static void BM_LinkedList(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::list<int> instance;
    int j = 0;
    for (size_t i = 0; i < kNIter; i++) {
      instance.push_back(j);
      j++;
    }
  }
}

BENCHMARK(BM_LinkedList);

float Q_rsqrt( float number )
{
  long i;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  y  = number;
  i  = * ( long * ) &y;                       // evil floating point bit level hacking
  i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
  y  = * ( float * ) &i;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//  y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}

static void BM_QuickSqrt(benchmark::State &state) {
  while (state.KeepRunning()) {
    state.
  }
}

BENCHMARK_MAIN();
