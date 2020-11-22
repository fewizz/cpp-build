#include "cxx_exec/ext/static_lib"

string_view name() { return "code"; }
vector<path> sources() { return { "code.cpp" }; }

on_startup configure() {
    cc.include("include");
}