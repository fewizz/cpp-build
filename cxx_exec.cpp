#include "cxx_exec/gcc_like_driver.hpp"
#include "cxx_exec/environment.hpp"
#include "clap/gnu_clap.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace filesystem;

#ifdef _WIN32
#include <libloaderapi.h>
static inline string current_exec_path() {
    vector<char> chars(0xFF);
    int w = GetModuleFileNameA(nullptr, chars.data(), chars.size());
    return {chars.begin(), chars.begin() + w};
}
#endif

int main(int argc, char* argv[]) {
    path cxx_exec = current_exec_path();

    vector<string_view> args{argv, argv+argc};

    if(args.size() <= 1)
        throw runtime_error("c++ file not provided");
    
    path root = cxx_exec.parent_path().parent_path();
    path cxx = absolute(args[1]);
    if(not exists(cxx))
        throw runtime_error("c++ file doesn't exists");

    bool verbose, gdb;
    gnu::clap clap;
    clap
        .flag("verbose", verbose)
        .flag("gdb", gdb);

    auto delimiter = find(args.begin()+2, args.end(), "--");

    clap.parse(args.begin()+2, delimiter);

    if(verbose) {
        cout << "cxx_exec executable: "+cxx_exec.string()+"\n";
        cout << "root: "+root.string()+"\n";
    }

    path exec = absolute(temp_directory_path()) / to_string(getpid());

    create_directories(path{exec}.remove_filename());

    auto cc = environment::cxx_compile_command_builder()
        .std(gcc_like_driver::cxx20)
        .quote_include(root/"include")
        .input(root/"share/cxx_exec/cxx_exec_entry.cpp")
        .input(cxx)
        .verbose(verbose)
        .out(exec);
    if(gdb) cc.debug(gcc_like_driver::gdb);
    try {
        environment::execute(cc);
    } catch(...) {return EXIT_FAILURE;}

    auto args_begin = delimiter==args.end() ? args.end() : delimiter+1;
    auto exec_command = 
        gdb ? cmd::command{"gdb", exec} : cmd::command{exec, args_begin, args.end()};
    
    try {
        environment::execute(exec_command);
    } catch(...) {} //We're not interested in this.

    return EXIT_SUCCESS;
}