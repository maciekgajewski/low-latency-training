#include <benchmark/benchmark.h>

#include <numeric>

static void hello(benchmark::State& state) {
  for(auto _ : state) {
    // hello :)
  }
}

BENCHMARK(hello);

BENCHMARK_MAIN();
