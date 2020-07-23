#include <string>
#include <vector>

#include "../../include/cxx_exec/ext/build/cxx_build.cpp"

using namespace std;

void configure() {
    name = "test";
    output_type = executable;
    build_configuration = release;
    sources.include_directory_recursive("src");
}