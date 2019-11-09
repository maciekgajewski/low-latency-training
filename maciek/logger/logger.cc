#include <iostream>
#include <string>

struct Trade {
  double price;
  int volume;
  std::string symbol;
};

namespace {

inline void doLog(std::string_view remainder) {
  std::cout << remainder << std::endl;
}

template <typename T, typename... Ts>
void doLog(std::string_view format, const T &p, const Ts &... tail) {
  auto place_holder_position = format.find("{}");
  if (place_holder_position == std::string_view::npos)
    throw std::runtime_error("Missing placeholder");

  auto beforePlaceholder = format.substr(0, place_holder_position);
  auto remainder = format.substr(place_holder_position + 2, format.length());

  std::cout << beforePlaceholder;
  std::cout << p;

  doLog(remainder, tail...);
}

} // namespace

template <typename... Ts> void log(const char *formatString, const Ts &... p) {

  doLog(formatString, p...);
}

int main() {

  Trade t{666.66, 42, "FFESX"};
  log("Tradetick on {}, {}@{}", t.symbol, t.volume, t.price);
}
