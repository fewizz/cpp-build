#include <iostream>
#include <algorithm>
#include "cpp_build/options.hpp"

void build(std::vector<std::string> args) {
    gnu_options_parser parser;

    parser.option({{}}, {{"echo"}}, [](std::string arg){
        std::cout << arg << "\n";
    });

    parser.parse(args);
}