#include <gtest/gtest.h>
#include <iostream>
#include <string_view>

#define private public
#include "multini.hh"

static constexpr std::string_view trimmed(std::string_view&& str)
{
    while (!str.empty() && std::isspace(str.front()))
        str.remove_prefix(1);

    while (!str.empty() && std::isspace(str.back()))
        str.remove_suffix(1);

    return str;
}

TEST(multini, SplitLinesOfConfig)
{
    const std::string_view config = trimmed(R"(
[section1]
key1=value1
key2=value2

[section2]
key3=value3
)");

    auto view = multini::INIReader::splitLines(config);
    auto lines = std::vector<std::string_view>(view.begin(), view.end());

    ASSERT_EQ(lines.size(), 6);
    ASSERT_EQ(lines[0], "[section1]");
    ASSERT_EQ(lines[1], "key1=value1");
    ASSERT_EQ(lines[2], "key2=value2");
    ASSERT_EQ(lines[3], "");
    ASSERT_EQ(lines[4], "[section2]");
    ASSERT_EQ(lines[5], "key3=value3");
}

TEST(multini, LinesWithCommentsOrEmptyShouldBeExcluded)
{
    const std::string_view config = trimmed(R"(
; whatever
; blabla
; some text
;
;; test123
[section1]
key1=value1
key2=value2

;; more rambling

;; test
)");

    auto view = multini::INIReader::filterLines(config);
    auto lines = std::vector<std::string_view>(view.begin(), view.end());

    ASSERT_EQ(lines.size(), 3);
    ASSERT_EQ(lines[0], "[section1]");
    ASSERT_EQ(lines[1], "key1=value1");
    ASSERT_EQ(lines[2], "key2=value2");
}

TEST(multini, ConfigShouldHaveSection)
{
    const std::string_view config = trimmed(R"(
; whatever
[section1]
key1=value1
)");

    multini::INIReader reader(config);
}
