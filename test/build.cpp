#include <iostream>
#include <algorithm>

int build(std::vector<std::string> args) {
    std::cout << "arguments: \n";
    
    std::for_each(args.begin(), args.end(), [&](std::string arg){
        std::cout << arg << "\n";
    });

    return EXIT_SUCCESS;
}