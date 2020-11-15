#include "cxx_exec/ext/executable"
#include "cxx_exec/ext/build_accessor.hpp"
#include "cxx_exec/cxx_exec.hpp"

string_view name() { return "main"; }
vector<path> sources() { return { "main.cpp" }; }

using namespace environment;

on_startup configure() {
    before_build = [&]() {

        change_dir("dependency", [&]() {
            execute(
                cxx_exec::compilation_of("build.cpp").output("dep")
            );

            build_accessor {
                load_shared_library("dep")
            }.build();
        });

        input_files.push_back("dependency/build/dependency.a");
    };
}