#pragma once

#include "command_executor.hpp"
#include <filesystem>
#include <optional>

namespace gcc { namespace driver {
using namespace std;
using namespace filesystem;

struct input_type {
    const char* name;
    constexpr input_type(const char* name):name{name}{}
};

namespace input_types {
    constexpr input_type 
    c("c"), c_header("c-header"), cxx_output("c++-output"),
    cxx("c++"), cxx_header("c++-header"), cxx_cpp_output("c++-cpp-output"),
    assembler("assembler"), assembler_with_cpp("assembler-with-cpp");
}

struct lang_std {
    const char* name;
    constexpr lang_std(const char* name):name{name}{}
};

namespace lang_stds {
    constexpr lang_std
    c89{"c89"}, c94{"iso9899:199409"}, gnu89{"gnu89"},
    c99{"c99"}, gnu99{"gnu99"}, c11{"c11"}, gnu11{"gnu11"},
    c17{"c17"}, gnu17{"gnu17"}, c2x{"c2x"}, gnu2x{"gnu2x"},

    cxx98{"c++98"}, gnucxx98{"gnu++98"}, cxx11{"c++11"},
    gnucxx11{"gnu++11"}, cxx14{"c++14"}, gnucxx14{"gnu++14"},
    cxx17{"c++17"}, gnucxx17{"gnu++17"},
    cxx20{"c++20"}, gnucxx20{"gnu++20"};
}

struct executor : protected command_executor {
    optional<input_type> input_type;   // -x
    optional<path> output;             // --output
    optional<lang_std> std;         // --std='arg'
    optional<string> compiler_files;   // -B'prefix'
    optional<path> system_root;        // --sysroot'dir'
    optional<path> working_directory;  // -working-directory='dir'

    executor(string name) : command_executor{name}{};

    executor(string name, lang_std std)
        :command_executor{name}, std{std}{}

    vector<path> input_files;
    void input_file(path p) { input_files.push_back(p); }

    vector<path> include_paths; // -Idir
    void include_path(path p) { include_paths.push_back(p); }

    vector<path> include_quote_paths; // -iquote dir
    void include_quote_path(path p) { include_quote_paths.push_back(p); }

    void execute() {
        vector<string> args{command_executor::args};

        if(working_directory)
            args.push_back("-working-directory="+working_directory->string());
        if(std)
            args.push_back("-std="+string{std->name});
        
        if(output)
            args.push_back("--output="+output->string());
        
        for(auto p : include_paths) {
            args.push_back("-I"+p.string());
        }
        for(auto p : include_quote_paths) {
            args.push_back("-iquote "+p.string());
        }
        for(auto p : input_files) {
            args.push_back(p.string());
        }

        return command_executor::execute(args);
    }
};

}}