#include "cpp_build/clang_executor.hpp"
#include <filesystem>
#include <iostream>

using namespace std;
using namespace filesystem;

int main(int argc,char** argv) {
    path bin_path = path(argv[0]).parent_path();
    path out_path = temp_directory_path() / "build_cpp.exe";

    gcc_driver::executor d;
    d.program_name = "clang++";
    d.std = &gcc_driver::lang_std::cxx17;
    d.include_paths.push_back("./");
    d.input_files.push_back(
        bin_path.parent_path() / "share/cpp_build/build_entry.cpp"
    );
    d.output_name = out_path.string();
    if(d.execute())
        terminate();

    cl_program_executor build;
    build.program_name = out_path.string();
    for_each(argv+1, argv+argc, [&](char* arg){
        build.args.emplace_back(arg);
    });
    if(build.execute())
        terminate();
    return EXIT_SUCCESS;
}