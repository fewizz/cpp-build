#include "cxx_exec/ext/static_lib"
#include <string_view>

string_view name() { return "dependency"; }
vector<path> sources() { return { "hello.cpp" }; }