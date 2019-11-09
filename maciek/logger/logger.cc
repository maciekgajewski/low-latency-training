#include <cstring>
#include <iostream>
#include <string>

#include "mock_queue.hh"

struct Trade {
  double price;
  int volume;
  std::string symbol;
};

namespace {

inline void doLogSlow(std::string_view remainder) {
  std::cout << remainder << std::endl;
}

template <typename T, typename... Ts>
void doLogSlow(std::string_view format, const T &p, const Ts &... tail) {
  auto place_holder_position = format.find("{}");
  if (place_holder_position == std::string_view::npos)
    throw std::runtime_error("Missing placeholder");

  auto beforePlaceholder = format.substr(0, place_holder_position);
  auto remainder = format.substr(place_holder_position + 2, format.length());

  std::cout << beforePlaceholder;
  std::cout << p;

  doLogSlow(remainder, tail...);
}

MockQueue queue;

template <typename T> std::size_t size(const T &) { return sizeof(T); }

std::size_t size(const std::string &s) { return s.length() + 1; }

template <typename T> void copy(char *&buffer, const T &v) {
  std::memcpy(buffer, &v, sizeof(T));
  buffer += sizeof(T);
}

void copy(char *&buffer, const std::string &s) {
  std::strcpy(buffer, s.c_str());
  buffer += s.length() + 1;
}

using printing_fun_t = void (*)(std::string_view, const char *);

struct Header {
  const char *formatString;
  printing_fun_t prntingFun;
  std::size_t paramsBufferSize;
};

template <typename T> void do_print(const char *&buf) {
  const T *p = reinterpret_cast<const T *>(buf);
  std::cout << *p;
  buf += sizeof(T);
}

template <> void do_print<std::string>(const char *&buf) {
  std::cout << buf;
  buf += std::strlen(buf) + 1;
}

template <typename T, typename... Ts>
void do_print_from_buffer(std::string_view formatString, const char *buffer) {
  auto pos = formatString.find("{}");
  if (pos == std::string_view::npos)
    throw std::runtime_error("No placeholder in format string");
  std::string_view before = formatString.substr(0, pos);
  std::string_view after = formatString.substr(pos + 2, formatString.length());

  std::cout << before;
  do_print<T>(buffer);

  if constexpr (sizeof...(Ts) == 0) {
    std::cout << after << std::endl;
  } else {
    do_print_from_buffer<Ts...>(after, buffer);
  }
}

template <typename... Ts>
void doLogFast(std::string_view format, const Ts &... pp) {

  std::size_t sizeNeeded = (size(pp) + ...);

  char *buffer = queue.beginWriting(sizeNeeded + sizeof(Header));
  Header *header = reinterpret_cast<Header *>(buffer);
  header->formatString = format.data();
  header->paramsBufferSize = sizeNeeded;
  header->prntingFun = do_print_from_buffer<Ts...>;

  buffer += sizeof(Header);

  (copy(buffer, pp), ...);

  queue.endWriting();
}

} // namespace

template <typename... Ts>
void log_slow(const char *formatString, const Ts &... p) {

  doLogSlow(formatString, p...);
}

template <typename... Ts>
void log_fast(const char *formatString, const Ts &... p) {
  doLogFast(formatString, p...);
}

void log_flush() {

  while (!queue.empty()) {
    Header *headerPtr =
        reinterpret_cast<Header *>(queue.beginReading(sizeof(Header)));
    Header header = *headerPtr;
    queue.endReading();

    const char *buffer = queue.beginReading(header.paramsBufferSize);
    header.prntingFun(header.formatString, buffer);
    queue.endReading();
  }
}

using namespace std::literals;

int main() {

  Trade t{666.66, 42, "FFESX123"};
  log_slow("Tradetick on {}, {}@{}", t.symbol, t.volume, t.price);

  log_fast("Tradetick on {}, {}@{}", t.symbol, t.volume, t.price);
  log_fast("Hello, my name is {}, I'm {} years old", "Max"s, 3);

  std::cout << "Flushing..." << std::endl;

  log_flush();
}
