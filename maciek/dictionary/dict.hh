#pragma once

#include <iostream>
#include <set>
#include <string>
#include <unordered_set>
#include <vector>

struct SetDict {
  SetDict(const std::vector<std::string> &words)
      : set_(words.begin(), words.end()) {}

  bool isInDictionary(std::string_view word) const {
    return set_.find(std::string(word)) != set_.end();
  }

private:
  std::set<std::string> set_;
};

struct UnorderedSetDict {
  UnorderedSetDict(const std::vector<std::string> &words)
      : set_(words.begin(), words.end()) {}

  bool isInDictionary(std::string_view word) const {
    return set_.find(std::string(word)) != set_.end();
  }

private:
  std::unordered_set<std::string> set_;
};
