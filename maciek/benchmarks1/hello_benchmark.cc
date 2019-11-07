#include <benchmark/benchmark.h>

#include <deque>
#include <list>
#include <numeric>
#include <vector>

static void push_back_reserve(benchmark::State &state) {

  int size = state.range(0);

  for (auto _ : state) {
    std::vector<int> v;
    v.reserve(size);
    for (int i = 0; i < size; ++i) {
      v.push_back(i);
      benchmark::DoNotOptimize(v.data());
    }
  }
}

template <typename Container> static void push_back(benchmark::State &state) {
  int size = state.range(0);
  for (auto _ : state) {
    Container v;
    for (int i = 0; i < size; ++i) {
      v.push_back(i);
      // benchmark::DoNotOptimize(v.front());
    }
  }
}

BENCHMARK_TEMPLATE(push_back, std::vector<int>)
    ->RangeMultiplier(2)
    ->Range(1024, 128 * 1024);
BENCHMARK_TEMPLATE(push_back, std::list<int>)->Range(1024, 128 * 1024);
BENCHMARK_TEMPLATE(push_back, std::deque<int>)->Range(1024, 128 * 1024);
BENCHMARK(push_back_reserve)->Range(1024, 128 * 1024);

BENCHMARK_MAIN();
