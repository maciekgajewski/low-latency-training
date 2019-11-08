#pragma once

#include <random>
#include <string>
#include <unordered_set>
#include <vector>

class Dictionary {
public:
  Dictionary(const std::vector<std::string> &words);

  bool isInDictionary(std::string_view word) const;
};

static std::random_device rd;
static std::uniform_int_distribution<int> charDist(48, 127);
static std::lognormal_distribution<float> lengthDist(2.19, 0.25);

static std::string createRandomString() {
  std::size_t len = (int)(std::floor(lengthDist(rd))) + 1;
  std::string out;
  out.reserve(len);
  for (std::size_t i = 0; i < len; i++)
    out.push_back(charDist(rd));

  return out;
}

static std::vector<std::string>
createVectorOfUniqueRandomStrings(std::size_t len) {
  std::unordered_set<std::string> strings;
  strings.reserve(len);

  while (strings.size() < len)
    strings.insert(createRandomString());

  return std::vector<std::string>(strings.begin(), strings.end());
}
