#pragma once

#include <filesystem>
#include <initializer_list>
#include <optional>
#include <set>
#include "command.hpp"
#include <string_view>
#include <variant>
#include <ranges>

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

constexpr struct output_type_t : string_view {
    constexpr output_type_t(const auto option):string_view(option){}
}
def{""},
object_file{"c"},
assembler_code{"S"},
preprocessed_src{"E"};

struct output_type:output_type_t {
    output_type(const output_type_t it):output_type_t{it}{}

    static constexpr output_type_t
    def{""},
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

struct command_builder {
protected:
    string name;

    optional<debug_information_type> debug_information_type;
    optional<input_type> input_type;    // -x
    optional<string> m_std;           // --std='arg'
    optional<string> compiler_files;    // -B'prefix'
    optional<path> system_root;         // --sysroot'dir'
    optional<path> working_directory;   // -working-directory='dir'
    optional<bool> verb;                // -v

    vector<path> include_paths;         // -Idir
    vector<path> include_quote_paths;   // -iquote dir
    vector<string> m_libs;
public:

    struct definition_t {
        string name;
        optional<string> value;

        std::string string() const {
            std::string res{"-D "};
            int def_size = name.size() + (value ? value->size() + 1 : 0);
            res.reserve(res.size()+def_size);
            res+=name;
            if(value) {
                res+='=';
                res+=*value;
            }
            return res;
        }
    };

protected:
    vector<definition_t> m_definitions;

public:
    command_builder(string_view name) : name{name}{};

    command_builder(string_view name, const lang_std& std)
        :name{name}, m_std{std.name}{}

    auto& std(const gcc_like_driver::lang_std& std) {
        m_std = std.name; return *this;
    }

    auto& std(string_view std) {
        m_std = std; return *this;
    }

    auto& debug(const gcc_like_driver::debug_information_type& dit) {
        debug_information_type = dit; return *this;
    }

    auto& lib(string_view name) {
        m_libs.emplace_back(name);
        return *this;
    }

    auto& libs(initializer_list<string_view>&& ins) {
        for(auto l : ins) lib(l); return *this;
    }

    auto& include(const path& p) { include_paths.push_back(p); return *this; }

    auto& include(initializer_list<path>&& includes) {
        include_paths.insert(include_paths.end(), includes);
        return *this;    
    }

    auto& include(const ranges::range auto& range) {
        for(const auto& v : range) include_paths.push_back(v); return *this;
    }

    auto& quote_include(const path& p) { include_quote_paths.push_back(p); return *this; }

    auto& definition(const definition_t& d) {
        m_definitions.push_back(d); return *this;
    }
    
    auto& definition(string def) { // yep, will need to copy anyway
        return definition(definition_t{std::move(def)});
    }

    auto& definitions(initializer_list<definition_t>&& il) {
        for(auto& d : il) definition(d); return *this;
    }

    auto& verbose(bool val) {verb = val; return *this;}

    cmd::command compilation(const initializer_list<path>& inputs, const output_type_t& ot, const path& out) const {
        return {name, args_to_string_vec(inputs, ot, out)};
    }

    cmd::command compilation(const ranges::range auto& inputs, const output_type_t& ot, const path& out) const {
        return {name, args_to_string_vec(inputs, ot, out)};
    }

    cmd::command make_rule_creation(const initializer_list<path>& inputs) const { return _mrc(inputs); }

    cmd::command make_rule_creation(const ranges::range auto& inputs) const { return _mrc(inputs); }
protected:
    cmd::command _mrc(const ranges::range auto& inputs) const {
        auto args = args_to_string_vec(inputs, std::nullopt, std::nullopt);
        args.push_back("-M");
        return {name, args};
    }

    vector<string> args_to_string_vec(
        const ranges::range auto& inputs,
        const std::optional<output_type> ot,
        const std::optional<path> output
    ) const {
        vector<string> args;

        if(debug_information_type)
            args.emplace_back("-"+string{debug_information_type->option});
        if(ot and *ot != def)
            args.emplace_back("-"+string{*ot});
        if(verb and *verb)
            args.emplace_back("-v");
        if(working_directory)
            args.emplace_back("-working-directory="+working_directory->string());
        if(m_std)
            args.emplace_back("-std="+*m_std);
        if(output)
            args.emplace_back("--output="+output->string());
        
        for(const auto& def : m_definitions)
            args.emplace_back(def.string());
        
        for(auto p : include_paths)
            args.emplace_back("-I"+p.string());
        
        for(auto p : include_quote_paths)
            args.emplace_back("-iquote "+p.string());
        
        for(auto& input : inputs)
            args.emplace_back(input.string());
        
        for(auto& lib : m_libs)
            args.emplace_back("-l"+lib);

        return args;
    }
};

}