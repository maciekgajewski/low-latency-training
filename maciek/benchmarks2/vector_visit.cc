#include <benchmark/benchmark.h>

#include <algorithm>
#include <random>
#include <vector>

std::vector<int> generateRandomVector(int size) {
  static std::default_random_engine rd;
  static std::uniform_int_distribution<int> dist(0, size - 1);

  std::vector<int> v;
  v.resize(size);
  for (int &i : v)
    i = dist(rd);
  return v;
}

struct InOrder {
  static void init(std::vector<int> &v2) {
    for (int i = 0; i < v2.size(); ++i)
      v2[i] = i;
  }
};

struct Shuffled {
  static void init(std::vector<int> &v2) {
    for (int i = 0; i < v2.size(); ++i)
      v2[i] = i;
    std::random_shuffle(v2.begin(), v2.end());
  }
};

template <typename Initializer> static void visit(benchmark::State &state) {
  int size = state.range(0);
  std::vector<int> v1 = generateRandomVector(size);
  std::vector<int> v2;
  v2.resize(size);
  Initializer::init(v2);

  for (auto _ : state) {
    int sum = 0;
    for (int i : v2)
      sum += v1[i];
    benchmark::DoNotOptimize(&sum);
  }

  state.SetBytesProcessed(size * sizeof(int) * state.iterations());
}

BENCHMARK_TEMPLATE(visit, InOrder)
    ->RangeMultiplier(2)
    ->Range(1024 * 2, 1024 * 1024 * 4);

BENCHMARK_TEMPLATE(visit, Shuffled)
    ->RangeMultiplier(2)
    ->Range(1024 * 2, 1024 * 1024 * 4);

BENCHMARK_MAIN();
