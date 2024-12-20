#pragma once
#include <map>
#include <ranges>
#include <string>
#include <string_view>

namespace multini {

/**
 * @brief INI reader that supports non-unique section names.
 * Uses std::multimap under the hood.
 */
class INIReader {
public:
    INIReader(const std::string_view& sv)
    {
    }

private:
    static auto splitLines(const std::string_view& sv)
    {
        return sv
            | std::views::split('\n')
            | std::views::transform([](const auto& line) {
                  auto trimmedLine = std::string_view(line.begin(), line.end());
                  if (!trimmedLine.empty() && trimmedLine.back() == '\r') {
                      trimmedLine.remove_suffix(1);
                  }
                  return trimmedLine;
              });
    }

    static auto filterLines(const std::string_view& sv)
    {
        return splitLines(sv)
            | std::views::filter([](const auto& line) {
                  return !line.empty();
              })
            | std::views::filter([](const auto& line) {
                  return line.starts_with(";");
              });
    }

    std::multimap<std::string, std::map<std::string, std::string>> mSections;
};
}
