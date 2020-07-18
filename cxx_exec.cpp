#include "cxx_exec/clang_driver_executor.hpp"
#include "cxx_exec/environment.hpp"
#include "clap/gnu_clap.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace filesystem;

int main(int argc, char* argv[]) {
    vector<string_view> args{argv, argv+argc};
    gnu::clap clap;
    path root = path(args[0]).parent_path().parent_path();

    if(args.size() <= 1)
        throw runtime_error("c++ file not provided");
    path cxx = absolute(path{args[1]});
    if(!exists(cxx))
        throw runtime_error("c++ file doesn't exists");

    bool verbose;
    clap.flag("verbose", verbose);

    auto delimiter = find(args.begin()+2, args.end(), string_view{"--"});

    clap.parse(args.begin()+2, delimiter);

    string out_exe_temp_dir = cxx.string();
    replace_if(
        out_exe_temp_dir.begin(),
        out_exe_temp_dir.end(),
        [](char ch){ return ch == ':' || ch == '.'; },
        '_'
    );

    path exec = absolute(temp_directory_path()) / out_exe_temp_dir;
    exec.replace_extension();
    create_directories(exec);

    auto comp = environment::cxx_compiler();
    comp.std = gcc_like_driver::cxx20;
    comp.include_quote_path(root/"include");
    comp.input_file(cxx);
    comp.input_file(root/"share/cxx_exec/exec_entry.cpp");
    comp.verbose(verbose);
    comp.output = exec;
    comp.execute();

    auto args_begin = delimiter==args.end() ? args.end() : delimiter+1;

    try {
        environment::execute(exec, args_begin, args.end());
    } catch(...) {} //We're not interested in this.

    return EXIT_SUCCESS;
}