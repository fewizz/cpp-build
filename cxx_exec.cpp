#include "cxx_exec/clang_driver.hpp"
#include "clap/gnu_clap.hpp"
#include <filesystem>
#include <algorithm>
#include <stdexcept>

using namespace std;
using namespace filesystem;

void main0(vector<string_view> args) {
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

    path exec_out = absolute(temp_directory_path()) / out_exe_temp_dir;
    exec_out.replace_extension();
    create_directories(exec_out);

    clang::driver::executor comp("clang++", clang::driver::lang_stds::cxx20);
    comp.include_quote_path(root/"include");
    comp.input_file(cxx);
    comp.input_file(root/"share/cxx_exec/exec_entry.cpp");
    comp.verbose(verbose);
    comp.output = exec_out;
    comp.execute();

    auto args_begin = delimiter==args.end() ? args.end() : delimiter+1;

    try {
        command_executor{exec_out.string(), {args_begin, args.end()}}.execute();
    } catch(...) {} //We're not interested in this.
}

int main(int argc, char* argv[]) {
    //vector<string_view> args;

    //for_each(argv, argv+argc, [&](auto arg){ args.emplace_back(arg); });
    main0(vector<string_view>{argv, argv+argc});

    return EXIT_SUCCESS;
}