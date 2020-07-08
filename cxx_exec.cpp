#include "cxx_exec/clang_driver.hpp"
#include "clap/gnu_clap.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <cstdlib>

using namespace std;
using namespace filesystem;
using namespace clap;

int main(int argc,char* argv[]) {
    gnu_clap clap;
    bool verbose;
    clap.flag("verbose", verbose);
    clap.parse(argv, argv);
    path root = path(argv[0]).parent_path().parent_path();

    if(argc <= 1)
        throw runtime_error("cpp file not provided");
    path cpp = absolute(path{argv[1]});
    if(!exists(cpp))
        throw runtime_error("cpp file not exists");

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
    comp.include_quote_path(root/"include");
    comp.input_file(argv[1]);
    comp.input_file(root/"share/cxx_exec/exec_entry.cpp");

    comp.output = exec_out;
    comp();

    try {
        command_executor{exec_out.string(), {argv+2, argv+argc}}();
    } catch(...) {} //We're not interested in this.
    
    return 0;
}