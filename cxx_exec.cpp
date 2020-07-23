#include "cxx_exec/clang_driver_executor.hpp"
#include "cxx_exec/environment.hpp"
#include "clap/gnu_clap.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace filesystem;

int main(int argc, char* argv[]) {
    vector<string_view> args{argv, argv+argc};

    path root = path(args[0]).parent_path().parent_path();
    if(args.size() <= 1)
        throw runtime_error("c++ file not provided");
    path cxx = absolute(args[1]);
    if(!exists(cxx))
        throw runtime_error("c++ file doesn't exists");

    bool verbose, gdb;
    gnu::clap clap;
    clap
        .flag("verbose", verbose)
        .flag("gdb", gdb);

    auto delimiter = find(args.begin()+2, args.end(), "--");

    clap.parse(args.begin()+2, delimiter);

    path exec = absolute(temp_directory_path()) / to_string(getpid());

    create_directories(exec);

    auto comp = environment::cxx_compiler();
    comp.std = gcc_like_driver::cxx20;
    comp.include_quote_path(root/"include");
    comp.input_file(root/"share/cxx_exec/cxx_exec_entry.cpp");
    comp.input_file(cxx);
    comp.verbose(verbose);
    if(gdb) comp.debug_information_type = clang::driver::gdb;
    comp.output = exec;
    try{
        comp.execute();
    } catch(...) {return EXIT_FAILURE;}

    auto args_begin = delimiter==args.end() ? args.end() : delimiter+1;

    try {
        if(gdb)
            environment::execute("gdb "+exec.string());
        else environment::execute(exec, args_begin, args.end());
    } catch(...) {} //We're not interested in this.

    return EXIT_SUCCESS;
}