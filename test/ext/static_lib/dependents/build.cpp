#include "cxx_exec/ext/static_lib"
#include "cxx_exec/environment.hpp"
#include "cxx_exec/ext/static_lib_accessor.hpp"
#include "cxx_exec/cxx_exec.hpp"
#include <stdexcept>

const char* name() { return "main"; }
vector<path> sources() { return { "main.cpp" }; }

void configure() {
    before_build = ([&](gcc_like_driver::command_builder& cc) {

        environment::change_dir("dependency", [&](){
            environment::execute(
                cxx_exec::command_builder("build.cpp")
                .output("dep")
            );

            static_lib_accessor {
                environment::load_shared_library("dep.dll")
            }.build("build", cc);
        });

    });
}