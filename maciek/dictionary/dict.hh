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

struct TransparentSetDict {
  TransparentSetDict(const std::vector<std::string> &words)
      : set_(words.begin(), words.end()) {}

  bool isInDictionary(std::string_view word) const {
    return set_.find(word) != set_.end();
  }

private:
  std::set<std::string, std::less<>> set_;
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

struct NonAllocatingUnorderedSetDict {
  NonAllocatingUnorderedSetDict(const std::vector<std::string> &words)
      : data_(words), set_(data_.begin(), data_.end()) {}

  bool isInDictionary(std::string_view word) const {
    return set_.find(word) != set_.end();
  }

private:
  std::vector<std::string> data_;
  std::unordered_set<std::string_view> set_;
};
