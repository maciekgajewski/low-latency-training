#include <vector>
#include <string>

class Dictionary {
public:
  Dictionary(const std::vector<std::string>& words);

  bool isInDictionary(std::string_view word) const;
};
