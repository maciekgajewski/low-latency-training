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
  std::atomic<bool> go = false;
  std::generate(shared.a.begin(), shared.a.end(), std::rand);

  std::thread background([&] {
    while (go)
      for (int &v : shared.b)
        v = std::rand();
  });

  for (auto _ : state) {
    int sum = 0;
    for (int i = 0; i < 1000; ++i) {
      sum += std::accumulate(shared.a.begin(), shared.a.end(), 0);
      benchmark::DoNotOptimize(sum);
    }
  }

  go = false;
  background.join();
}

BENCHMARK_TEMPLATE(sharing, SharedData);
BENCHMARK_TEMPLATE(sharing, SharedDataWithGap);

BENCHMARK_MAIN();
