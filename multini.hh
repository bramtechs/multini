#pragma once
#include <map>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace multini {
/**
 * @brief INI reader that supports non-unique section names.
 * Uses std::multimap under the hood.
 */
class INIReader {
public:
    using ErrorBag = std::stringstream;

    INIReader(const std::string_view& sv)
    {
    }

    [[nodiscard]] auto hasErrors() const noexcept
    {
        return !mErrors.str().empty();
    }

    [[nodiscard]] auto errors() const noexcept
    {
        return mErrors.str();
    }

private:
    class Line {
    public:
        Line(const std::string_view& line, int lineNum = 0, ErrorBag* errors = nullptr)
            : mErrors(errors)
            , mIsValid(true)
            , mIsHeader(line.starts_with("["))
        {
            if (mIsHeader) {
                auto startCount = std::ranges::distance(line
                    | std::views::take_while([](char c) { return c == '['; }));

                auto endCount = std::ranges::distance(line
                    | std::views::take_while([](char c) { return c == ']'; }));

                if (startCount != endCount) {
                    addError("Line ", lineNum, ": Section header has unbalanced brackets. (", startCount, " vs ", endCount, ")");
                    // non-critical error, continue parsing
                }
            }
        }

        [[nodiscard]] auto isHeader() const noexcept
        {
            return mIsHeader;
        }

        [[nodiscard]] auto getHeader() const noexcept
        {
            assert(mIsHeader);
            return mHeaderOrPair.first;
        }

        [[nodiscard]] auto getKeyValuePair() const noexcept
        {
            assert(mIsHeader);
            return mHeaderOrPair;
        }

    private:
        template<typename... Args>
        void addError(Args&&... args)
        {
            mIsValid = false;
            if (mErrors) {
                ((*mErrors) << ... << args);
            }
        }

        ErrorBag* mErrors;
        bool mIsValid;
        bool mIsHeader;
        std::pair<std::string, std::string> mHeaderOrPair;
    };

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

    static auto parseLines(const std::string_view& sv, ErrorBag* errors = nullptr)
    {
        return std::views::zip(filterLines(sv), std::views::iota(1))
            | std::views::transform([errors](const auto&& it) {
                  return Line(std::get<0>(it), std::get<1>(it), errors);
              });
    }

    ErrorBag mErrors {};
    std::multimap<std::string, std::map<std::string, std::string>> mSections {};
};
}
