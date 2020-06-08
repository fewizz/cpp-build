#include "cpp_build/clang_driver.hpp"
#include <filesystem>
#include <algorithm>

using namespace std;
using namespace filesystem;

int main(int argc,char** argv) {
    path bin = path(argv[0]).parent_path();
    path share = bin.parent_path()/"share";
    path include = bin.parent_path()/"include";
    string exec_name = current_path().string();
    replace_if(
        exec_name.begin(),
        exec_name.end(),
        [](char ch){ return ch == '/' || ch == ':' || ch == '\\'; },
        '_'
    );
    string exec = (absolute(temp_directory_path()) / exec_name).string();

    clang_driver::executor comp;
    comp.name = "clang++";
    comp.std = &clang_driver::lang::std::cxx20;
    comp.include_path(include);
    comp.include_path("./");
    comp.input_file(share/"cpp_build/build_entry.cpp");
    comp.output = exec;
    if(comp.execute())
        terminate();

    program_executor build;
    build.name = exec;
    build.args.insert(build.args.begin(), argv+1, argv+argc);
    if(build.execute())
        terminate();
    
    return EXIT_SUCCESS;
}