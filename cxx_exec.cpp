#include "cxx_exec/clang_driver.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

using namespace std;
using namespace filesystem;

int main(int argc,char* argv[]) {
    path root = path(argv[0]).parent_path().parent_path();

    path cpp = absolute(path{argv[1]});

    string out_exe_temp_dir = cpp.string();
    replace_if(
        out_exe_temp_dir.begin(),
        out_exe_temp_dir.end(),
        [](char ch){ return ch == ':' || ch == '.'; },
        '_'
    );

    path exec_out = absolute(temp_directory_path()) / out_exe_temp_dir;
    exec_out.replace_extension();
    create_directories(exec_out);

    clang::default_cxx20_driver_executor comp;
    comp.include_quote_path(root/"include/cxx_exec");
    comp.input_file(argv[1]);
    comp.input_file(root/"share/cxx_exec/exec_entry.cpp");
    comp.output = exec_out;
    comp();

    try {
        command_executor{exec_out.string(), {argv+2, argv+argc}}();
    } catch(...) {} //We're not interested in this.
    
    return 0;
}