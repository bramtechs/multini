/*
 * Copyright (c) 2025. Doomhowl Interactive - bramtechs/brambasiel
 * MIT License. Absolutely no warranty.
 *
 * File created on: 04-01-2025
 */

#include <fstream>
#include <iostream>
#include <multini.hh>
#include <sstream>

using namespace multini;

int main(int argc, char** argv)
{
    std::ifstream file("config.ini", std::ios::in);
    if (!file.is_open()) {
        std::cerr << "Failed to open file\n";
        return EXIT_FAILURE;
    }

    std::ostringstream ss;
    ss << file.rdbuf();
    std::string fileContent = ss.str();

    multini::INIReader reader(fileContent);
    if (reader.hasErrors()) {
        std::cerr << reader.errors() << '\n';
        return EXIT_FAILURE;
    }

    // INIReader uses an underlying std::multimap
    // https://en.cppreference.com/w/cpp/container/multimap

    for (const auto& [header, content] : reader) {
        std::cout << "found header: " << header << '\n';
        for (const auto& [key, value] : content) {
            std::cout << "  with entry: " << key << "=" << value << '\n';
        }
    }

    return EXIT_SUCCESS;
}
