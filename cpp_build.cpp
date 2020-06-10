#include "cpp_build/clang_driver.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace filesystem;

int main(int argc,char** argv) {
    path usr = path(argv[0]).parent_path().parent_path();

    string exec_name = current_path().string();
    replace_if(
        exec_name.begin(),
        exec_name.end(),
        [](char ch){ return ch == '/' || ch == ':' || ch == '\\'; },
        '_'
    );
    path exec = absolute(temp_directory_path()) / exec_name;

    clang::default_cxx20_driver_executor comp;
    comp.include_path(usr/"include");
    comp.include_path("./");
    comp.input_file(usr/"share/cpp_build/build_entry.cpp");
    comp.output = exec;
    comp();

    try {
        program_executor{exec, {argv+1, argv+argc}}();
    } catch(exception& e) {} //We're not interested in this.
    
    return EXIT_SUCCESS;
}