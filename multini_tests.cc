/*
 * Copyright (c) 2025. Doomhowl Interactive - bramtechs/brambasiel
 * MIT License. Absolutely no warranty.
 *
 * File created on: 20-12-2024
 */

#include <algorithm>
#include <gtest/gtest.h>
#include <iostream>
#include <string_view>

#define MULTINI_TESTING
#include <multini.hh>

using namespace multini;

static constexpr std::string_view trimmed(std::string_view&& str)
{
    while (!str.empty() && std::isspace(str.front()))
        str.remove_prefix(1);

    while (!str.empty() && std::isspace(str.back()))
        str.remove_suffix(1);

    return str;
}

TEST(multini, TestStripLine)
{
    const std::string_view input = "  [  Section1  ]  ";
    const std::string_view expected = "[  Section1  ]";

    ASSERT_EQ(INIReader::stripLine(input), expected);
}

TEST(multini, TestStripLine2)
{
    const std::string_view input = "\t\t\t [Section 1]  ";
    const std::string_view expected = "[Section 1]";

    ASSERT_EQ(INIReader::stripLine(input), expected);
}

TEST(multini, ParseHeaderLine)
{
    const std::string_view input = "[Section1]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getHeader(), "Section1");
}

TEST(multini, ParseHeaderLineMoreBrackets)
{
    const std::string_view input = "[[[Section1]]]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getHeader(), "Section1");
}

TEST(multini, ParseHeaderLineWithUnevenBracketsShouldError)
{
    const std::string_view input = "[[[Section1]]]]]";

    auto bag = INIReader::ErrorBag();
    auto line = INIReader::Line(input, 0, &bag);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.isValid(), false);
    ASSERT_EQ(bag.str().empty(), false);
}

TEST(multini, ParseHeaderLineWithUnevenBracketsShouldStillParse)
{
    const std::string_view input = "[Section1]]]]]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.isValid(), false);
    ASSERT_EQ(line.getHeader(), "Section1");
}

TEST(multini, ParseKeyValuePairLine)
{
    const std::string_view input = "key1=value1";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getKey(), "key1");
    ASSERT_EQ(line.getValue(), "value1");
}

TEST(multini, ParseKeyValuePairLineWithSpaces)
{
    const std::string_view input = "key1 = value";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.getKey(), "key1");
    ASSERT_EQ(line.getValue(), "value");
}

TEST(multini, ParseKeyValuePairLineWithSpacesInValue)
{
    const std::string_view input = "key1 =        value with spaces   ";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getKey(), "key1");
    ASSERT_EQ(line.getValue(), "value with spaces");
}

TEST(multini, ParseKeyValuePairWithSpacesInKey)
{
    const std::string_view input = "key 1 = value";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getKey(), "key 1");
    ASSERT_EQ(line.getValue(), "value");
}

TEST(multini, ParseKeyValuePairWithEqualsInValue)
{
    const std::string_view input = "key1 = value = 1";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getKey(), "key1");
    ASSERT_EQ(line.getValue(), "value = 1");
}

TEST(multini, ParseKeyValuePairWithBracketsInValue)
{
    const std::string_view input = "key1 = value [1]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getKey(), "key1");
    ASSERT_EQ(line.getValue(), "value [1]");
}

TEST(multini, ParseKeyValuePairWithBracketsInKey)
{
    const std::string_view input = "key[1] = value";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), false);
    ASSERT_EQ(line.isValid(), true);
    ASSERT_EQ(line.getKey(), "key[1]");
    ASSERT_EQ(line.getValue(), "value");
}

TEST(multini, CountLeadingBrackets)
{
    const std::string_view input = "[[[section1";
    ASSERT_EQ(INIReader::countPrefix(input, '['), 3);
}

TEST(multini, CountLeadingBrackets2)
{
    const std::string_view input = "[[[ [[]section1]][";
    ASSERT_EQ(INIReader::countPrefix(input, '['), 3);
}

TEST(multini, CountEndingBrackets)
{
    const std::string_view input = "[[[section1]]]";
    ASSERT_EQ(INIReader::countSuffix(input, ']'), 3);
}

TEST(multini, CountEndingBrackets2)
{
    const std::string_view input = "[[[section]]]1]]]";
    ASSERT_EQ(INIReader::countSuffix(input, ']'), 3);
}

TEST(multini, ParseHeaderLineWithInternalBrackets)
{
    const std::string_view input = "[[#]]Section[[#]]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "#]]Section[[#");
}

TEST(multini, ParseHeaderLineWithSpaces)
{
    const std::string_view input = "[Section Title 1]";

    auto line = INIReader::Line(input);
    ASSERT_EQ(line.isHeader(), true);
    ASSERT_EQ(line.getHeader(), "Section Title 1");
}

TEST(multini, ParseLines)
{
    const std::string_view input = R"(
[section1]
key1=value1
)";

    auto lines = std::vector<INIReader::Line>();
    std::ranges::for_each(INIReader::parseLines(input), [&lines](auto&& it) {
        lines.emplace_back(it);
    });

    ASSERT_EQ(lines.size(), 2);
    ASSERT_EQ(lines[0].isValid(), true);
    ASSERT_EQ(lines[0].isHeader(), true);
    ASSERT_EQ(lines[0].getHeader(), "section1");

    ASSERT_EQ(lines[1].isValid(), true);
    ASSERT_EQ(lines[1].isHeader(), false);
    ASSERT_EQ(lines[1].getKey(), "key1");
    ASSERT_EQ(lines[1].getValue(), "value1");
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

    INIReader::ErrorBag bag {};
    IniReaderUsedContainer map;
    INIReader::fillMultimap(config, map, bag);

    ASSERT_TRUE(map.contains("section1"));
    ASSERT_EQ(map.find("section1")->second.at("key1"), "value1");
    ASSERT_TRUE(bag.str().empty());
}

TEST(multini, IntegrationTestWith2UniqueSections)
{
    const std::string_view config = trimmed(R"(
[section1]
key1=value1
key2=value2

[section2]
key3=value3
    )");

    multini::INIReader reader(config);
    if (reader.hasErrors()) {
        std::cerr << reader.errors() << '\n';
        FAIL();
    }
    ASSERT_EQ(reader.size(), 2);
    ASSERT_TRUE(reader.contains("section1"));
    ASSERT_TRUE(reader.contains("section2"));

    ASSERT_TRUE(reader.contains("section1"));
    ASSERT_TRUE(reader.find("section1")->second.contains("key1"));
    ASSERT_EQ(reader.find("section1")->second.find("key1")->second, "value1");

    ASSERT_TRUE(reader.find("section1")->second.contains("key2"));
    ASSERT_EQ(reader.find("section1")->second.find("key2")->second, "value2");

    ASSERT_TRUE(reader.contains("section2"));
    ASSERT_TRUE(reader.find("section2")->second.contains("key3"));
    ASSERT_EQ(reader.find("section2")->second.find("key3")->second, "value3");
}

TEST(multini, IntegrationTestWith2EquallyNamedSections)
{
    const std::string_view config = trimmed(R"(
[section1]
key1=value1
key2=value2

[section1]
key3=value3
    )");

    multini::INIReader reader(config);
    if (reader.hasErrors()) {
        std::cerr << reader.errors() << '\n';
        FAIL();
    }
    ASSERT_EQ(reader.size(), 2);
    ASSERT_TRUE(reader.contains("section1"));
    ASSERT_EQ(reader.count("section1"), 2);

    ASSERT_EQ(reader.lower_bound("section1")->second.find("key1")->second, "value1");
    ASSERT_EQ(reader.lower_bound("section1")->second.find("key2")->second, "value2");

    auto it = reader.lower_bound("section1");
    std::advance(it, 1);
    ASSERT_EQ(it->second.find("key3")->second, "value3");
}
