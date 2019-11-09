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

static void singleThread(benchmark::State &state) {

  SharedData shared;
  std::generate(shared.a.begin(), shared.a.end(), std::rand);

  for (auto _ : state) {
    int sum = std::accumulate(shared.a.begin(), shared.a.end(), 0);
    benchmark::DoNotOptimize(sum);
  }
}

BENCHMARK(singleThread);

BENCHMARK_MAIN();
