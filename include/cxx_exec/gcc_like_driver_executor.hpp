#pragma once

#include "command_executor.hpp"
#include <filesystem>
#include <optional>

namespace gcc_like_driver {

using namespace std;
using namespace filesystem;

struct input_type_t {
    string_view name;
    constexpr input_type_t(const string_view name):name{name}{}
};

struct input_type:input_type_t {
    input_type(const input_type_t it):input_type_t{it.name}{}
    
    static constexpr input_type_t
    c{"c"}, c_header{"c-header"}, cxx_output{"c++-output"},
    cxx{"c++"}, cxx_header{"c++-header"}, cxx_cpp_output{"c++-cpp-output"},
    assembler{"assembler"}, assembler_with_cpp{"assembler-with-cpp"};
};

constexpr struct lang_std_t {
    string_view name;
    constexpr lang_std_t(const string_view name):name{name}{}
}
c89{"c89"}, c94{"iso9899:199409"}, gnu89{"gnu89"},
c99{"c99"}, gnu99{"gnu99"}, c11{"c11"}, gnu11{"gnu11"},
c17{"c17"}, gnu17{"gnu17"}, c2x{"c2x"}, gnu2x{"gnu2x"},

cxx98{"c++98"}, gnucxx98{"gnu++98"}, cxx11{"c++11"},
gnucxx11{"gnu++11"}, cxx14{"c++14"}, gnucxx14{"gnu++14"},
cxx17{"c++17"}, gnucxx17{"gnu++17"},
cxx20{"c++20"}, gnucxx20{"gnu++20"};

struct lang_std:lang_std_t {
    constexpr lang_std(const lang_std_t it):lang_std_t{it}{}

    static constexpr lang_std_t
    c89{gcc_like_driver::c89}, c94{gcc_like_driver::c94}, gnu89{gcc_like_driver::gnu89},
    c99{gcc_like_driver::c99}, gnu99{gcc_like_driver::gnu99}, c11{gcc_like_driver::c11}, gnu11{gcc_like_driver::gnu11},
    c17{gcc_like_driver::c17}, gnu17{gcc_like_driver::gnu17}, c2x{gcc_like_driver::c2x}, gnu2x{gcc_like_driver::gnu2x},

    cxx98{gcc_like_driver::cxx98}, gnucxx98{gcc_like_driver::gnucxx98}, cxx11{gcc_like_driver::cxx11},
    gnucxx11{gcc_like_driver::gnucxx11}, cxx14{gcc_like_driver::cxx14}, gnucxx14{gcc_like_driver::gnucxx14},
    cxx17{gcc_like_driver::cxx17}, gnucxx17{gcc_like_driver::gnucxx17},
    cxx20{gcc_like_driver::cxx20}, gnucxx20{gcc_like_driver::gnucxx20};
};

struct output_type_t {
    string_view option;
    constexpr output_type_t(const string_view option):option{option}{}
};
struct output_type:output_type_t {
    output_type(const output_type_t it):output_type_t{it}{}

    static constexpr output_type_t
    object_file{"c"},
    assembler_code{"S"},
    preprocessed_src{"E"};
};

constexpr struct debug_information_type_t {
    string_view option;
    constexpr debug_information_type_t(const string_view option):option{option}{}
}
native{"g"}, gdb{"ggdb"}, dwarf{"gdwarf"},
stabs{"gstabs"}, xcoff{"gxcoff"}, vms{"gvms"};

struct debug_information_type:debug_information_type_t {
    debug_information_type(const debug_information_type_t& it):debug_information_type_t{it}{}

    static constexpr debug_information_type_t
    native{gcc_like_driver::native}, gdb{gcc_like_driver::gdb}, drawrf{gcc_like_driver::dwarf},
    stabs{gcc_like_driver::stabs}, xcoff{gcc_like_driver::xcoff}, vms{gcc_like_driver::vms};
};

struct gcc_like_driver_executor : protected command_executor {
    using command_executor::args;

    optional<debug_information_type> debug_information_type;
    optional<input_type> input_type;   // -x
    optional<output_type> output_type; // -c, -S, -E
    optional<path> output;             // --output
    optional<lang_std> std;            // --std='arg'
    optional<string> compiler_files;   // -B'prefix'
    optional<path> system_root;        // --sysroot'dir'
    optional<path> working_directory;  // -working-directory='dir'
    optional<bool> verb;               // -v

    gcc_like_driver_executor(string name) : command_executor{name}{};

    gcc_like_driver_executor(string name, lang_std std)
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

    void execute() const {
        vector<string> args{command_executor::args};

        if(debug_information_type)
            args.push_back("-"+string{debug_information_type->option});
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

}