#include <benchmark/benchmark.h>

#include "papipp.h"

#include <algorithm>
#include <iostream>
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

  papi::event_set<PAPI_TOT_INS, PAPI_TOT_CYC, PAPI_L1_DCM, PAPI_L2_DCM,
                  PAPI_L3_LDM>
      events;
  events.start_counters();

  for (auto _ : state) {
    int sum = 0;
    for (int i : v2)
      sum += v1[i];
    benchmark::DoNotOptimize(&sum);
  }

  events.stop_counters();

  // double ipc = double(events.get<PAPI_TOT_INS>().counter()) /
  events.get<PAPI_TOT_CYC>().counter();
  // state.counters["ipc"] = ipc;
  state.counters["L1M"] =
      double(events.get<PAPI_L1_DCM>().counter()) / state.iterations();
  state.counters["L2M"] =
      double(events.get<PAPI_L2_DCM>().counter()) / state.iterations();
  state.counters["L3M"] =
      double(events.get<PAPI_L3_LDM>().counter()) / state.iterations();
  state.SetBytesProcessed(2 * size * sizeof(int) * state.iterations());
  state.counters["data"] = 2 * size * sizeof(int);
}

BENCHMARK_TEMPLATE(visit, InOrder)
    ->RangeMultiplier(2)
    ->Range(1024, 1024 * 1024 * 4);

BENCHMARK_TEMPLATE(visit, Shuffled)
    ->RangeMultiplier(2)
    ->Range(1024, 1024 * 1024 * 4);

BENCHMARK_MAIN();
