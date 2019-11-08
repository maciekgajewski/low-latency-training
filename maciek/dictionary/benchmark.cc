#include <benchmark/benchmark.h>

#include "../benchmarks1/mtrace.h"
#include "../benchmarks2/papipp.h"

#include "dict.hh"
#include "utils.h"

#include <fstream>
#include <iostream>

static std::vector<std::string> load() {
  std::ifstream file("/etc/dictionaries-common/words");
  std::vector<std::string> words;

  std::string word;
  for (std::string line; std::getline(file, line);) {
    words.push_back(line);
  }

  std::cout << "Loaded " << words.size() << " words" << std::endl;
  return words;
}

static std::vector<std::string> loadMangled() {
  std::vector<std::string> words = load();
  for (std::string &word : words) {
    word[word.length() / 2] = '_';
  }
  return words;
}

std::vector<std::string> wordsIn;    // = load();
std::vector<std::string> wordsNotIn; // = loadMangled();

template <typename Dictionary>
static void InDictionary(benchmark::State &state) {
  Dictionary dict(wordsIn);

  bool allIn = true;
  auto it = wordsIn.begin();
  for (auto _ : state) {
    allIn = allIn && dict.isInDictionary(*it);
    ++it;
    if (it == wordsIn.end())
      it = wordsIn.begin();
  }

  if (!allIn)
    throw std::runtime_error("Expected all words to be in");
}

template <typename Dictionary>
static void NotInDictionary(benchmark::State &state) {
  Dictionary dict(wordsIn);

  bool someIn = false;
  auto it = wordsNotIn.begin();
  for (auto _ : state) {
    someIn = someIn || dict.isInDictionary(*it);
    ++it;
    if (it == wordsNotIn.end())
      it = wordsNotIn.begin();
  }

  if (someIn)
    throw std::runtime_error("Expected no words to be in");
}

BENCHMARK_TEMPLATE(InDictionary, SetDict);
BENCHMARK_TEMPLATE(NotInDictionary, SetDict);

BENCHMARK_TEMPLATE(InDictionary, UnorderedSetDict);
BENCHMARK_TEMPLATE(NotInDictionary, UnorderedSetDict);

int main(int argc, char **argv) {
  static const int DICT_SIZE = 100'000;
  std::vector<std::string> words =
      createVectorOfUniqueRandomStrings(DICT_SIZE * 2);
  wordsIn.assign(words.begin(), words.begin() + DICT_SIZE);
  wordsNotIn.assign(words.begin() + DICT_SIZE, words.end());

  benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv))
    return 1;
  benchmark::RunSpecifiedBenchmarks();
}
