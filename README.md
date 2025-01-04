# multini

Single-header INI config file reader that supports duplicate section names.

Written in bleeding-edge C++23.

## Example

config.ini
```ini
[section1]
key1=value1
key2=value2

[section1]
key3=value3

[section2]
hello=world
```

demo.cc
```c++
#include <multini.hh>

#include <fstream>
#include <iostream>
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
```

output
```
found header: section1
  with entry: key1=value1
  with entry: key2=value2
found header: section1
  with entry: key3=value3
found header: section2
  with entry: hello=world
```
