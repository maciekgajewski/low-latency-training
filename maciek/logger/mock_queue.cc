#include "mock_queue.hh"

#include <cassert>
#include <stdexcept>

char *MockQueue::beginWriting(std::size_t size) {

  std::size_t oldSize = buffer.size();
  buffer.resize(buffer.size() + size);
  return buffer.data() + oldSize;
}

void MockQueue::endWriting() {}

char *MockQueue::beginReading(std::size_t size) {
  if (size > buffer.size())
    return nullptr;

  bytesRead = size;
  return buffer.data();
}

void MockQueue::endReading() {

  assert(bytesRead <= buffer.size());

  buffer.erase(buffer.begin(), buffer.begin() + bytesRead);
  bytesRead = 0;
}

bool MockQueue::empty() const { return buffer.empty(); }
