#pragma once

#include <functional>
#include <iostream>
#include <string>

class HashDict {
public:
  HashDict(const std::vector<std::string> &words) : storage_(words) {

    buckets_.resize(findGoodSize(words.size()));

    for (const std::string &word : storage_) {
      auto hash = std::hash<std::string_view>()(word);
      auto index = indexFromHash(hash);
      while (true) {
        Bucket &bucket = buckets_[index];
        if (bucket.empty()) {
          bucket.assign(word, hash);
          break;
        }
        index = next(index);
      }
    }
  }

  bool isInDictionary(std::string_view word) const {
    auto hash = std::hash<std::string_view>()(word);
    auto index = indexFromHash(hash);

    for (std::size_t i = 0; i < buckets_.size(); ++i) {
      const Bucket &bucket = buckets_[index];
      if (bucket.empty())
        return false;
      if (bucket.equal(word, hash))
        return true;
      index = next(index);
    }
    return false;
  }

private:
  static std::size_t findGoodSize(std::size_t words) {

    std::size_t size = 1;
    while (size < words)
      size <<= 1;
    size <<= 1;
    return size;
  }

  std::size_t indexFromHash(std::size_t hash) const {
    return hash & (buckets_.size() - 1);
  }

  std::size_t next(std::size_t index) const {
    return (index + 1) & (buckets_.size() - 1);
  }

  struct Bucket {

    Bucket() {} // init empty bucket

    bool empty() const { return word_ == nullptr; }
    bool equal(std::string_view word, std::size_t hash) const {
      if (hash_ == hash)
        return *word_ == word;
      else
        return false;
    }
    void assign(const std::string &word, std::size_t hash) {
      word_ = &word;
      hash_ = hash;
    }

  private:
    const std::string *word_ = nullptr;
    std::size_t hash_;
  };

  std::vector<std::string> storage_;
  std::vector<Bucket> buckets_;
};

class HashDictWithModulo {
public:
  HashDictWithModulo(const std::vector<std::string> &words) : storage_(words) {

    buckets_.resize(findGoodSize(words.size()));

    for (const std::string &word : storage_) {
      auto hash = std::hash<std::string_view>()(word);
      auto index = indexFromHash(hash);
      while (true) {
        Bucket &bucket = buckets_[index];
        if (bucket.empty()) {
          bucket.assign(word, hash);
          break;
        }
        index = next(index);
      }
    }
  }

  bool isInDictionary(std::string_view word) const {
    auto hash = std::hash<std::string_view>()(word);
    auto index = indexFromHash(hash);

    for (std::size_t i = 0; i < buckets_.size(); ++i) {
      const Bucket &bucket = buckets_[index];
      if (bucket.empty())
        return false;
      if (bucket.equal(word, hash))
        return true;
      index = next(index);
    }
    return false;
  }

private:
  static std::size_t findGoodSize(std::size_t words) { return words * 2; }

  std::size_t indexFromHash(std::size_t hash) const {
    return hash % buckets_.size();
  }

  std::size_t next(std::size_t index) const {
    return (index + 1) % buckets_.size();
  }

  struct Bucket {

    Bucket() {} // init empty bucket

    bool empty() const { return word_ == nullptr; }
    bool equal(std::string_view word, std::size_t hash) const {
      if (hash_ == hash)
        return *word_ == word;
      else
        return false;
    }
    void assign(const std::string &word, std::size_t hash) {
      word_ = &word;
      hash_ = hash;
    }

  private:
    const std::string *word_ = nullptr;
    std::size_t hash_;
  };

  std::vector<std::string> storage_;
  std::vector<Bucket> buckets_;
};

class HashDict2 {
public:
  HashDict2(const std::vector<std::string> &words) : storage_(words) {

    buckets_.resize(findGoodSize(words.size()));

    for (const std::string &word : storage_) {
      auto hash = std::hash<std::string_view>()(word);
      auto index = indexFromHash(hash);
      while (true) {
        Bucket &bucket = buckets_[index];
        if (bucket.empty()) {
          bucket.assign(word);
          break;
        }
        index = next(index);
      }
    }
  }

  bool isInDictionary(std::string_view word) const {
    auto hash = std::hash<std::string_view>()(word);
    auto index = indexFromHash(hash);

    for (std::size_t i = 0; i < buckets_.size(); ++i) {
      const Bucket &bucket = buckets_[index];
      if (bucket.empty())
        return false;
      if (bucket.equal(word))
        return true;
      index = next(index);
    }
    return false;
  }

private:
  static std::size_t findGoodSize(std::size_t words) { return words * 2; }

  std::size_t indexFromHash(std::size_t hash) const {
    return hash % buckets_.size();
  }

  std::size_t next(std::size_t index) const {
    return (index + 1) % buckets_.size();
  }

  struct Bucket {

    Bucket() {} // init empty bucket

    bool empty() const { return word_ == nullptr; }
    bool equal(std::string_view word) const { return *word_ == word; }
    void assign(const std::string &word) { word_ = &word; }

  private:
    const std::string *word_ = nullptr;
  };

  std::vector<std::string> storage_;
  std::vector<Bucket> buckets_;
};

class HashDict1 {
public:
  HashDict1(const std::vector<std::string> &words) {

    buckets_.resize(findGoodSize(words.size()));

    for (const std::string &word : words) {
      auto hash = std::hash<std::string_view>()(word);
      auto index = indexFromHash(hash);
      while (true) {
        Bucket &bucket = buckets_[index];
        if (bucket.empty()) {
          bucket.assign(word);
          break;
        }
        index = next(index);
      }
    }
  }

  bool isInDictionary(std::string_view word) const {
    auto hash = std::hash<std::string_view>()(word);
    auto index = indexFromHash(hash);

    for (std::size_t i = 0; i < buckets_.size(); ++i) {
      const Bucket &bucket = buckets_[index];
      if (bucket.empty())
        return false;
      if (bucket.equal(word))
        return true;
      index = next(index);
    }
    return false;
  }

private:
  static std::size_t findGoodSize(std::size_t words) { return words * 2; }

  std::size_t indexFromHash(std::size_t hash) const {
    return hash % buckets_.size();
  }

  std::size_t next(std::size_t index) const {
    return (index + 1) % buckets_.size();
  }

  struct Bucket {

    Bucket() {} // TODO init empty bucket

    bool empty() const { return word_.empty(); }                      // TODO
    bool equal(std::string_view word) const { return word_ == word; } // TODO
    void assign(const std::string &word) { word_ = word; }            // TODO

  private:
    std::string word_;
  };

  std::vector<Bucket> buckets_;
};
