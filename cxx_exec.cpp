#include "gcc_like_driver.hpp"
#include "environment.hpp"
#include "clap/braced_clap.hpp"
#include <clap/parser.hpp>
#include <filesystem>
#include <algorithm>
#include <stdexcept>
#include <unistd.h>
#include <iostream>
#include "shared_library_accessor.hpp"

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;

int main(int argc, char* argv[]) {
    argv++; argc--;

    vector<string_view> args{argv, argv + argc};

    if (args.empty())
        throw runtime_error("c++ file not provided");

    path cxx = absolute(args.front());
    if(not exists(cxx)) throw runtime_error("c++ file '"+cxx.string()+"' doesn't exists");

    bool verbose;
    string std;
    path output_path;
    auto delimiter = find(args.begin() + 1, args.end(), "--");

    clap::braced_clap{}
        .option("verbose", clap::value_parser<char>(verbose))
        .braced(
            "output",
            {
                { "path", clap::value_parser<char>(output_path) },
            }
        )
        .option("standard", clap::value_parser<char>(std))
        .parse(args.begin() + 1, delimiter);

    bool output_is_temp = output_path.empty();

    if (output_is_temp)
        output_path = temp_directory_path() / to_string(getpid());

    output_path = absolute(output_path);
    if(output_path.extension().empty())
        output_path.replace_extension(environment::dynamic_lib_extension);

    if(verbose)
        cout << "output path: "+output_path.string()+"\n" << flush;

    auto cc = environment::cxx_compile_command_builder()
        .std(std.empty() ? "c++20" : std)
        .verbose(verbose)
        .debug(native)
        .shared(true)
        .position_independent_code(true);
    try {
        environment::execute(cc.compilation_of(cxx).to(output_path));
    } catch(...) { return EXIT_FAILURE; }

    try {
        shared_library_accessor lib{ output_path };
        int off = std::distance(args.begin(), delimiter);
        lib.run<int(int, char*[])>("main", argc - off, argv + off);
    } catch(...) {} //We're don't care

    if (output_is_temp) remove(output_path);

    return EXIT_SUCCESS;
}
