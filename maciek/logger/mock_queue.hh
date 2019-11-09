#pragma once

#include <vector>

class MockQueue {
public:
  char *beginWriting(std::size_t size);
  void endWriting();

  char *beginReading(std::size_t size);
  void endReading();

  bool empty() const;

private:
  std::vector<char> buffer;
  std::size_t bytesRead = 0;
};
