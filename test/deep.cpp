#include "cxx_exec/ext/test"
#include <vector>
#include "cxx_exec/unix/ipstream.hpp"
#include <iostream>
#include <filesystem>

#include "cxx_exec/update_need_checker.hpp"

using namespace std;

on_test __deep() {
    auto cc = environment::cxx_compile_command_builder();
    deep(
        cc,
        "test.cpp",
        "out",
        "output"
    );
}