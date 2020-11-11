#include "cxx_exec/ext/static_lib"

const char* name() { return "dependency"; }
vector<path> sources() { return { "hello.cpp" }; }

void configure() {
    
}