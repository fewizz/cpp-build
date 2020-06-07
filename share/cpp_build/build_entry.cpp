#include <vector>
#include <string>
#include "build.cpp"

int main(int argc,char** argv) {
    std::vector<std::string> args;

    for(int i = 1; i < argc; i++)
        args.emplace_back(argv[i]);

    build(args);
    return EXIT_SUCCESS;
}