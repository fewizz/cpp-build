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
    optional<output_type> m_output_type;// -c, -S, -E
    optional<path> output;              // --output
    optional<lang_std> m_std;           // --std='arg'
    optional<string> compiler_files;    // -B'prefix'
    optional<path> system_root;         // --sysroot'dir'
    optional<path> working_directory;   // -working-directory='dir'
    optional<bool> verb;                // -v

    vector<path> include_paths;         // -Idir
    vector<path> include_quote_paths;   // -iquote dir
    optional<bool> m_make_rule;
    optional<path> m_make_rule_file;

public:
	using library_t = string;
	using input_file_t = path;

    vector<variant<library_t, input_file_t>> inputs;
	
    struct definition_t {
        string name;
        optional<string> value;
        bool operator == (const definition_t& d) const {
            return name==d.name;
        }
        bool operator < (const definition_t& d) const {
            return name < d.name;
        }
    };

protected:
    set<definition_t> m_definitions;

public:

    command_builder(string name) : name{name}{};

    command_builder(string name, lang_std std)
        :name{name}, m_std{std}{}

    command_builder& std(gcc_like_driver::lang_std s) {
        m_std = s; return *this;
    }

    command_builder& debug(gcc_like_driver::debug_information_type dit) {
        debug_information_type = dit; return *this;
    }

    command_builder& out(path p) { output = p; return *this; }
    path out() { return *output; }

    command_builder& out_type(output_type ot) {
        if(ot == def) m_output_type.reset();
        else m_output_type = ot;
        return *this;
    }

    command_builder& out(path p, output_type ot) {
        out(p); return out_type(ot);
    }

    command_builder& in(path p) {
        inputs.push_back(input_file_t{ p }); return *this;
    }

    template<ranges::range R>
    command_builder& in(const R& ins) {
        for(auto p : ins) in(p); return *this;
    }

    command_builder& in(initializer_list<path> ins) {
        return in(ins);
    }

    void lib(string name) {
        inputs.push_back(library_t{ name });
    }

    command_builder& include(path p) { include_paths.push_back(p); return *this; }

    command_builder& quote_include(path p) { include_quote_paths.push_back(p); return *this; }

    command_builder& definition(definition_t d) {
        m_definitions.insert(d); return *this;
    }

    command_builder& definitions(initializer_list<definition_t> il) {
        for(auto& d : il) definition(d); return *this;
    }

    command_builder& verbose(bool val) {verb = val; return *this;}

    command_builder& make_rule(bool val) {
        m_make_rule = val;
        if(!val) m_make_rule_file.reset();
        return *this;
    }
    command_builder& make_rule(path p) {
        m_make_rule = true; m_make_rule_file = p; return *this;
    }

    operator cmd::command() {
        vector<string> args;

        if(debug_information_type)
            args.push_back("-"+string{debug_information_type->option});
        if(m_output_type)
            args.push_back("-"+string{*m_output_type});
        if(verb and *verb)
            args.push_back("-v");
        if(working_directory)
            args.push_back("-working-directory="+working_directory->string());
        if(m_std)
            args.push_back("-std="+string{m_std->name});
        if(output)
            args.push_back("--output="+output->string());
        if(m_make_rule and *m_make_rule)
            args.push_back("-M");
        if(m_make_rule_file)
            args.push_back("-MF "+m_make_rule_file->string());
        
        for(auto [name, def] : m_definitions) {
            string res = "\"-D"+name;
            if(def)
                res+="="+*def;
            res+="\"";
            args.push_back(res);
        }
        
        for(auto p : include_paths)
            args.push_back("-I"+p.string());
        
        for(auto p : include_quote_paths)
            args.push_back("-iquote "+p.string());
        
        for(auto& input : inputs) {
			if(holds_alternative<library_t>(input)) {
				auto& lib_name = get<library_t>(input);
				args.push_back("-l"+lib_name);
			}
			else {
				auto& in_file = get<input_file_t>(input);
				args.push_back(in_file.string());
			}
		}

        return {name, args.begin(), args.end()};
    }
};

}