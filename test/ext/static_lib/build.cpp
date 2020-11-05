#include "ext/static_lib.hpp"

const char* name() { return "code"; }
vector<path> sources() { return { "code.cpp" }; }

void configure(gnu::clap& clap, command_builder& cc) {
    cc.include("include");
}