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

constexpr struct output_type {
    const char* option;
    constexpr output_type(const char* option):option{option}{}
}
do_not_link("c"),
do_not_assemble("S"),
do_not_compile("E");

constexpr const output_type&
    object_file = do_not_link,
    assembler_code = do_not_assemble,
    preprocessed_src = do_not_compile;

struct executor : protected command_executor {
    using command_executor::args;

    optional<input_type> input_type;   // -x
    optional<output_type> output_type; // -c, -S, -E
    optional<path> output;             // --output
    optional<lang_std> std;            // --std='arg'
    optional<string> compiler_files;   // -B'prefix'
    optional<path> system_root;        // --sysroot'dir'
    optional<path> working_directory;  // -working-directory='dir'
    optional<bool> verb;               // -v

    executor(string name) : command_executor{name}{};

    executor(string name, lang_std std)
        :command_executor{name}, std{std}{}

    struct input_file_t {
        enum type_t {
            source, library
        } type;
        string path;
    };

    vector<input_file_t> input_files;

    void input_file(path p) {
        input_files.push_back(input_file_t{input_file_t::source, p.string()});
    }
    void library(string name) {
        input_files.push_back(input_file_t{input_file_t::library, name});
    }

    vector<path> include_paths; // -Idir
    void include_path(path p) { include_paths.push_back(p); }

    vector<path> include_quote_paths; // -iquote dir
    void include_quote_path(path p) { include_quote_paths.push_back(p); }

    vector<pair<string, optional<string>>> definitions;
    void definition(string name, optional<string> def={}) {
        definitions.emplace_back(name, def);
    }

    void verbose(bool val) {verb = val;}

    void execute() {
        vector<string> args{command_executor::args};

        if(output_type)
            args.push_back("-"+string{output_type->option});
        if(verb && *verb)
            args.push_back("-v");
        if(working_directory)
            args.push_back("-working-directory="+working_directory->string());
        if(std)
            args.push_back("-std="+string{std->name});
        if(output)
            args.push_back("--output="+output->string());
        
        for(auto [name, def] : definitions) {
            string res = "\"-D"+name;
            if(def)
                res+="="+*def;
            res+="\"";
            args.push_back(res);
        }
        for(auto p : include_paths) {
            args.push_back("-I"+p.string());
        }
        for(auto p : include_quote_paths) {
            args.push_back("-iquote "+p.string());
        }
        for(auto p : input_files) {
            auto res = p;
            args.push_back(p.type == input_file_t::library ? "-l"+p.path : p.path);
        }

        return command_executor::execute(args);
    }
};

}}