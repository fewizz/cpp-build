#include "gcc_like_driver.hpp"
#include "environment.hpp"
#include "clap/braced_clap.hpp"
#include <clap/parser.hpp>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include "shared_library.hpp"

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;

#ifdef _WIN32
#include <libloaderapi.h>
#include <errhandlingapi.h>
#include <winerror.h>
static inline string current_exec_path() {
    string path_s;

	int w;
	do {
		path_s.resize(path_s.size()+0x100);
    	w = GetModuleFileNameA(nullptr, path_s.data(), path_s.size());
	} while(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

    path_s.resize(w);
	return path_s;
}
#endif

int main(int argc, char* argv[]) {
    path cxx_exec = current_exec_path();

    // program parameters
    argv++; argc--;

    vector<string_view> args{argv, argv + argc};

    if (args.empty())
        throw runtime_error("c++ file not provided");

    path root = cxx_exec.parent_path().parent_path();
    path cxx = absolute(args.front());
    if(not exists(cxx)) throw runtime_error("c++ file '"+cxx.string()+"' doesn't exists");

    bool verbose;
    string std, debugger;//, output_type;
    path output;
    auto delimiter = find(args.begin() + 1, args.end(), "--");

    clap::braced_clap{}
        .option("verbose", clap::value_parser<char>(verbose))
        .option(
            "debugger",  clap::value_parser<char>(debugger)
        )
        .braced(
            "output",
            {
                { "path", clap::value_parser<char>(output) },
            }
        )
        .option("standard", clap::value_parser<char>(std))
        .parse(args.begin() + 1, delimiter);

    bool output_is_temp = output.empty();

    if (output_is_temp)
        output = temp_directory_path() / to_string(getpid());

    output = absolute(output);
    if(output.extension().empty())
        output.replace_extension(environment::dynamic_lib_extension);

    if(verbose) {
        cout << "cxx-exec executable path: "+cxx_exec.string()+"\n";
        cout << "root: "+root.string()+"\n";
        cout << "exec path: "+output.string()+"\n";
        cout.flush();
    }

    auto cc = environment::cxx_compile_command_builder()
        .std(std.empty() ? "c++20" : std)
        .include(root/"include/cxx_exec")
        .verbose(verbose)
        .debug(native)
        .shared(true)
        .position_independent_code(true);
    try {
        environment::execute(
            cc.compilation_of({root/"share/cxx_exec/cxx_exec_entry.cpp", cxx}).to(output)
        );
    } catch(...) { return EXIT_FAILURE; }

    auto args_begin = delimiter == args.end() ? args.end() : delimiter + 1;
    auto exec_command =
        not debugger.empty()
            ? cmd::command{ debugger, std::vector<string>{ output.generic_string() } }
            : cmd::command{ output, args_begin, args.end() };

    try {
        if(verbose)
            cout << "executing: "+exec_command.string()+"\n";
        shared_library sl{ output };

        int off = std::distance(args.begin(), delimiter);
        sl.run<int(int, char*[])>("main", argc - off, argv + off);

    } catch(...) {} //We're not interested in this.

    if (output_is_temp) remove(output);

    return EXIT_SUCCESS;
}
