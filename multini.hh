﻿#pragma once
#include <algorithm>
#include <map>
#include <ostream>
#include <ranges>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace multini {

#ifdef MULTINI_TESTING
#define private public
#define protected public
#endif

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
    static auto countPrefix(const std::string_view& sv, char ch)
    {
        return std::ranges::distance(sv
            | std::views::take_while([ch](char c) { return c == ch; }));
    }

    static auto countSuffix(const std::string_view& sv, char ch)
    {
        return std::ranges::distance(sv
            | std::views::reverse
            | std::views::take_while([ch](char c) { return c == ch; }));
    }

    class Line {
    public:
        Line(const std::string_view& line, int lineNum = 0, ErrorBag* errors = nullptr)
            : mErrors(errors)
            , mIsValid(true)
            , mIsHeader(line.starts_with("["))
        {
            if (mIsHeader) {
                const auto startCount = countPrefix(line, '[');
                const auto endCount = countSuffix(line, ']');

                if (startCount != endCount) {
                    addError("Line ", lineNum, ": Section header has unbalanced brackets. (", startCount, " vs ", endCount, ")");
                    // non-critical error, continue parsing
                }

                const auto startIndex = line.find_first_of('[') + startCount;
                const auto lastIndex = line.find_last_of(']') - endCount + 1;

                mHeaderOrPair.first = line.substr(startIndex, lastIndex - startIndex);
            } else {
                std::string_view key, value;

                const auto startIndex = line.find_first_not_of(' ');
                const auto equalsIndex = line.find_first_of('=');

                key = line.substr(startIndex, equalsIndex - startIndex);
                value = line.substr(equalsIndex + 1);

                mHeaderOrPair.first = stripLine(key);
                mHeaderOrPair.second = stripLine(value);
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

        [[nodiscard]] auto getKey() const noexcept
        {
            assert(!mIsHeader);
            return mHeaderOrPair.first;
        }

        [[nodiscard]] auto getValue() const noexcept
        {
            assert(!mIsHeader);
            return mHeaderOrPair.second;
        }

        [[nodiscard]] auto isValid() const noexcept
        {
            return mIsValid;
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

    static auto stripLine(const std::string_view& sv)
    {
        const auto IsSpace = [](char c) {
            return std::isspace(static_cast<unsigned char>(c));
        };

        const auto startIndex = std::ranges::distance(sv
            | std::views::take_while(IsSpace));
        const auto endIndex = std::ranges::distance(sv
            | std::views::reverse | std::views::take_while(IsSpace));

        return sv.substr(startIndex, sv.size() - startIndex - endIndex);
    }

    static auto splitLines(const std::string_view& sv)
    {
        return sv
            | std::views::split('\n')
            | std::views::transform([](const auto&& line) {
                  return stripLine(std::string_view(line.begin(), line.end()));
              });
    }

    static auto filterLines(const std::string_view& sv)
    {
        return splitLines(sv)
            | std::views::filter([](const auto& line) {
                  return !line.empty();
              })
            | std::views::filter([](const auto& line) {
                  return !line.starts_with(";");
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

#ifdef MULTINI_TESTING
#undef private
#undef protected
#endif
}
