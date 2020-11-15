#include "cxx_exec/ext/executable"
#include "cxx_exec/ext/static_lib_accessor.hpp"
#include "cxx_exec/cxx_exec.hpp"

string_view name() { return "main"; }
vector<path> sources() { return { "main.cpp" }; }

using namespace environment;

on_startup configure() {
    before_build = ([&]() {

        change_dir("dependency", [&]() {
            
            execute(
                cxx_exec::command_builder{"build.cpp"}.output("dep").compile_only(true)
            );

            static_lib_accessor {
                load_shared_library("dep")
            }.build();
        });

        input_files.push_back("dependency/build/dependency.a");
    });
}