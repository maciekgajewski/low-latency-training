#include <benchmark/benchmark.h>

#include <cstdlib>
#include <list>
#include <numeric>
#include <vector>

void fill(std::list<int> &container, std::size_t elements) {
  for (int i = 0; i < elements; ++i)
    container.push_back(::rand());
}

void fill(std::vector<int> &container, std::size_t elements) {
  container.reserve(elements);
  for (int i = 0; i < elements; ++i)
    container.push_back(::rand());
}

template <typename Container> static void accumulate(benchmark::State &state) {
  Container container;
  auto elements = state.range(0);

  fill(container, elements);

  for (auto _ : state) {
    int sum = std::accumulate(container.begin(), container.end(), 0);
    benchmark::DoNotOptimize(&sum);
  }
  state.SetBytesProcessed(elements * sizeof(int) * state.iterations());
}

BENCHMARK_TEMPLATE(accumulate, std::vector<int>)
    ->RangeMultiplier(1024)
    ->Range(1ull << 10, 2ull << 10 << 10);
BENCHMARK_TEMPLATE(accumulate, std::list<int>)
    ->RangeMultiplier(1024)
    ->Range(1ull << 10, 2ull << 10 << 10);

BENCHMARK_MAIN();
