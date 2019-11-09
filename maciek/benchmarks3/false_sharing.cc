#include "util.hh"

#include <benchmark/benchmark.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdlib>
#include <numeric>
#include <thread>

struct alignas(64) SharedData {
  std::array<int, 8> a;
  std::array<int, 8> b;
};
static_assert(sizeof(SharedData) == 64);
static_assert(offsetof(SharedData, b) == 32);

struct alignas(64) SharedDataWithGap {
  std::array<int, 8> a;
  std::array<int, 8> gap;
  std::array<int, 8> b;
};
static_assert(sizeof(SharedDataWithGap) > 64);
static_assert(offsetof(SharedDataWithGap, b) == 64);

template <typename Shared> static void sharing(benchmark::State &state) {
  Shared shared;
  std::atomic<bool> go = true;
  std::generate(shared.a.begin(), shared.a.end(), std::rand);

  std::thread background([&] {
    while (go)
      for (int &v : shared.b) {
        v = std::rand();
        benchmark::DoNotOptimize(shared.b.data());
      }
  });
  for (auto _ : state) {
    int sum = std::accumulate(shared.a.begin(), shared.a.end(), 0);
    benchmark::DoNotOptimize(sum);
  }
  go = false;
  background.join();
}

static void justSum(benchmark::State &state) {

  SharedData shared;
  std::generate(shared.a.begin(), shared.a.end(), std::rand);

  for (auto _ : state) {
    int sum = std::accumulate(shared.a.begin(), shared.a.end(), 0);
    benchmark::DoNotOptimize(sum);
  }
}

template <typename Shared> static void sharedRead(benchmark::State &state) {

  Shared shared;
  std::atomic<bool> go = true;
  std::generate(shared.a.begin(), shared.a.end(), std::rand);

  std::thread background([&] {
    while (go) {
      int sum = std::accumulate(shared.b.begin(), shared.b.end(), 0);
      benchmark::DoNotOptimize(&sum);
    }
  });
  for (auto _ : state) {
    int sum = std::accumulate(shared.a.begin(), shared.a.end(), 0);
    benchmark::DoNotOptimize(sum);
  }
  go = false;
  background.join();
}

BENCHMARK(justSum);
BENCHMARK_TEMPLATE(sharing, SharedData);
BENCHMARK_TEMPLATE(sharing, SharedDataWithGap);
BENCHMARK_TEMPLATE(sharedRead, SharedData);
BENCHMARK_TEMPLATE(sharedRead, SharedDataWithGap);

BENCHMARK_MAIN();
