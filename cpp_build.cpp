#include "cpp_build/clang_driver.hpp"
#include <filesystem>

using namespace std;
using namespace filesystem;

int main(int argc,char** argv) {
    path bin = path(argv[0]).parent_path();
    path share = bin.parent_path() / "share";
    path out = temp_directory_path() / "build_cpp";

    clang_driver::executor comp;
    comp.name = "clang++";
    comp.std = &clang_driver::lang::std::cxx17;
    comp.include_path("./");
    comp.input_file(share / "cpp_build/build_entry.cpp");
    comp.output = out;
    if(comp.execute())
        terminate();

    cl_program_executor build;
    build.name = out.string();
    build.args.insert(build.args.begin(), argv+1, argv+argc);
    if(build.execute())
        terminate();
    
    return EXIT_SUCCESS;
}