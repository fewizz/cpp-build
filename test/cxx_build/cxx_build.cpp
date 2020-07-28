#include <string>
#include <vector>

#include "../../include/cxx_exec/ext/build/cxx_build.cpp"

void configure(builder& b);

auto& test_builder = add_buidler("test", configure);

void configure(builder& b) {
    build_configuration = release;
    sources.include_directory_recursive("src");
}