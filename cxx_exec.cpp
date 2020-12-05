#include "cb/gcc_like_driver.hpp"
#include "environment.hpp"
#include "clap/gnu_clap.hpp"
#include <clap/parser.hpp>
#include <filesystem>
#include <algorithm>
#include <iterator>
#include <memory>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include "shared_lib_accessor.hpp"
#include "unc/by_dependencies_date.hpp"

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;

int main(int argc, char* argv[]) {
    vector<string_view> args{argv, argv + argc};

    if (args.size() < 1)
        throw runtime_error("c++ file is not provided");

    path cxx = absolute(args[1]);
    if(not exists(cxx)) throw runtime_error("c++ file '"+cxx.string()+"' doesn't exists");

    bool verbose = false, compile_only = false;
    string std;
    path output_path;
    auto delimiter = std::ranges::find(args, "--");
    vector<string> lib_paths;
    vector<string> libs;

    gnu::clap{}
        .flag('v', "verbose", verbose)
        .flag('c', "compile-only", compile_only)
        .value('o', "output", output_path)
        .value('s', "standard", std)
        .values<string>('L', "lib-path", std::back_inserter( lib_paths ))
        .values<string>('l', "lib", std::back_inserter( libs ))
        .parse(args.begin() + 2, delimiter);

    bool output_is_temp = output_path.empty();

    if (output_is_temp)
        output_path = temp_directory_path() / to_string(getpid());

    output_path = absolute(output_path);
    if(output_path.extension().empty())
        output_path.replace_extension(environment::shared_lib_extension);

    if(verbose)
        cout << "output path: "+output_path.string() << endl;

    auto cc = environment::cxx_compile_command_builder()
        .std(std.empty() ? "c++20" : std)
        .debug(native)
        .shared(true)
        .position_independent_code(true)
        .lib_paths(lib_paths)
        .libs(libs);
    try {
        if(output_is_temp or by_dependencies_date(cc, cxx, output_path) () ) {
            if(verbose) {
                cout << "compiling" << endl;
                cc.verbose(verbose);
            }
            auto compilation_command = cc.compilation_of(cxx).to(output_path);
            if(verbose)
                cout << "compilation command: " << compilation_command.string() << endl;
            environment::execute(compilation_command);
        }
    } catch(...) { return EXIT_FAILURE; }

    if(not compile_only) {
        try {
            vector<char*> program_args;
            auto output_path_str = output_path.string();
            program_args.emplace_back(output_path_str.data());

            if(delimiter != args.end()) {
                int delemiter_index = std::distance(args.begin(), delimiter);
                int args_begin_index = delemiter_index + 1;
                program_args.insert(program_args.end(), argv + args_begin_index, argv + argc);
            }
            
            environment::load_shared_library(output_path)
                .run<int(int, char*[])>(
                    "main",
                    program_args.size(),
                    program_args.data()
                );
        } catch(...) {}
    }

    if (output_is_temp) remove(output_path);

    return EXIT_SUCCESS;
}
