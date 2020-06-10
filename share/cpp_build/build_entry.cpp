#include <vector>
#include <string>
#include "build.cpp"

int main(int argc,char** argv) {
    build({argv+1, argv+argc});
    return EXIT_SUCCESS;
}