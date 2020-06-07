#include <iostream>
#include <algorithm>

void build(std::vector<std::string> args) {
    std::cout << "arguments: \n";
    
    std::for_each(args.begin(), args.end(), [&](std::string arg){
        std::cout << arg << "\n";
    });
}