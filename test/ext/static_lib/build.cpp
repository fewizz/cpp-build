#include "build/build.hpp"
#include "ext/static_lib.hpp"

string name() { return "code"; }
vector<path> sources() { return { "code.cpp" }; }

void configure(clap& clap, command_builder& cc) {
    cc.include("include");
}