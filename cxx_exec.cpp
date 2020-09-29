#include "gcc_like_driver.hpp"
#include "environment.hpp"
#include "clap/gnu_clap.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;

#ifdef _WIN32
#include <libloaderapi.h>
#include <errhandlingapi.h>
#include <winerror.h>
static inline string current_exec_path() {
    vector<char> chars;

	int w;
	do {
		chars.resize(chars.size()+0x100);
    	w = GetModuleFileNameA(nullptr, chars.data(), chars.size());
	} while(GetLastError() == ERROR_INSUFFICIENT_BUFFER);
	return {chars.begin(), chars.begin() + w};
}
#endif

int main(int argc, char* argv[]) {
    path cxx_exec = current_exec_path();

    // program parameters
    vector<string_view> args{argv + 1, argv + argc};

    if (args.empty())
      throw runtime_error("c++ file not provided");

    path root = cxx_exec.parent_path().parent_path();
    path cxx = absolute(args[0]);
    if(not exists(cxx)) throw runtime_error("c++ file '"+cxx.string()+"' doesn't exists");

    bool verbose, gdb;
    string std;
    path exec;
    auto delimiter = find(args.begin() + 1, args.end(), "--");
    gnu::clap{}
        .flag("verbose", verbose)
        .flag("gdb", gdb)
        .value("exec", exec)
        .value("standard", std)
        .parse(args.begin() + 1, delimiter);

    if(std.empty()) std = "c++20";
    bool temp_exec = exec.empty();

    if (temp_exec)
        exec = temp_directory_path() / to_string(getpid());

    exec = absolute(exec);
    if(exec.extension().empty())
        exec.replace_extension(environment::exec_extension);

    if(verbose) {
        cout << "cxx-exec executable path: "+cxx_exec.string()+"\n";
        cout << "root: "+root.string()+"\n";
        cout << "exec path: "+exec.string()+"\n";
        cout.flush();
    }

    auto cc = environment::cxx_compile_command_builder()
        .std(std)
        .include(root/"include/cxx_exec")
        .verbose(verbose)
        .debug(native);
    try {
        environment::execute(
            cc.compilation_of({root/"share/cxx_exec/cxx_exec_entry.cpp", cxx}).to(exec)
        );
    } catch(...) { return EXIT_FAILURE; }

    auto args_begin = delimiter == args.end() ? args.end() : delimiter + 1;
    auto exec_command = gdb ? cmd::command{"gdb", exec}
                            : cmd::command{exec, args_begin, args.end()};

    try {
        environment::execute(exec_command);
    } catch(...) {} //We're not interested in this.

    if (temp_exec) remove(exec);

    return EXIT_SUCCESS;
}
