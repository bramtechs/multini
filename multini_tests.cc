#include <gtest/gtest.h>
#include <iostream>
#include <string_view>

#define private public
#include "multini.hh"

using namespace multini;

static constexpr std::string_view trimmed(std::string_view&& str)
{
    while (!str.empty() && std::isspace(str.front()))
        str.remove_prefix(1);

    while (!str.empty() && std::isspace(str.back()))
        str.remove_suffix(1);

    return str;
}

TEST(multini, ParseHeaderLine)
{
    const std::string_view input = "[Section1]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "Section1");
}

TEST(multini, ParseHeaderLineMoreBraces)
{
    const std::string_view input = "[[[Section1]]]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "Section1");
}

TEST(multini, ParseHeaderLineWithUnevenBracesShouldError)
{
    const std::string_view input = "[[[Section1]]]]]";

    auto bag = INIReader::ErrorBag();
    auto line = INIReader::Line(input, 0, &bag);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(bag.str().empty(), false);
}

TEST(multini, ParseHeaderLineWithUnevenBracesShouldStillParse)
{
    const std::string_view input = "[Section1]]]]]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "Section1");
}

TEST(multini, ParseHeaderLineWithInternalBraces)
{
    const std::string_view input = "[[#]]Section[[#]]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "Section");
}

TEST(multini, ParseHeaderLineWithSpaces)
{
    const std::string_view input = "[Section Title 1]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "Section Title 1");
}

// TODO: test stray bracket should be an error
// TODO: uncommented section should not parse

TEST(multini, SplitLinesOfConfig)
{
    const std::string_view config = trimmed(R"(
[section1]
key1=value1
key2=value2

[section2]
key3=value3
)");

    auto view = INIReader::splitLines(config);
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

    auto view = INIReader::filterLines(config);
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

    INIReader reader(config);
}
