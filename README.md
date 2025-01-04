# multini

Single-header INI config file reader that supports duplicate section names.

Written in bleeding-edge C++23.

## Example

grocery_list.ini
```ini
[apple]
price=0.50 euro
color=red

[apple]
price=0.70 euro
color=green

[milk]
price=3.00 euro
volume=0.5 liters
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
    std::ifstream file("grocery_list.ini", std::ios::in);
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

    // INIReader inherits std::multimap
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
found header: apple
  with entry: color=red
  with entry: price=0.50 euro
found header: apple
  with entry: color=green
  with entry: price=0.70 euro
found header: milk
  with entry: price=3.00 euro
  with entry: volume=0.5 liters
```
