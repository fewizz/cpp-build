#include "cxx_exec/ext/test"
#include "cxx_exec/ext/log"

#include "cxx_exec/environment.hpp"
#include "cxx_exec/cxx_exec.hpp"

using namespace environment;

on_test unix_pipe() {
    info("executing ls");

    change_dir("unix", []() {
        execute(
            cxx_exec::execution_of("pipe.cpp").operands("ls")
        );
    });
}