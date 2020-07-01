#include <vector>
#include <string>
#include <iostream>

void exec(std::vector<std::string> args);

int main(int argc,char** argv) {
    exec({argv+1, argv+argc});
    return EXIT_SUCCESS;
}