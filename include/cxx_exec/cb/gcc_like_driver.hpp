#pragma once

#include <array>
#include <filesystem>
#include <initializer_list>
#include <optional>
#include <set>
#include "../command.hpp"
#include <string_view>
#include <type_traits>
#include <variant>
#include <ranges>
#include <vector>

namespace gcc_like_driver {

struct input_type_t {
    std::string_view name;
    constexpr input_type_t(const std::string_view name):name{name}{}
};

struct input_type:input_type_t {
    input_type(const input_type_t it):input_type_t{it.name}{}
    
    static constexpr input_type_t
    c{"c"}, c_header{"c-header"}, cxx_output{"c++-output"},
    cxx{"c++"}, cxx_header{"c++-header"}, cxx_cpp_output{"c++-cpp-output"},
    assembler{"assembler"}, assembler_with_cpp{"assembler-with-cpp"};
};

constexpr struct lang_std_t {
    std::string_view name;
    constexpr lang_std_t(const std::string_view name):name{name}{}
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

constexpr struct output_type_t : std::string_view {
    constexpr output_type_t(const auto option):std::string_view(option){}
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
    std::string_view option;
    constexpr debug_information_type_t(const std::string_view option):option{option}{}
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
    using string = std::string;
    using string_view = std::string_view;
    using path = std::filesystem::path;
    template<class T> using initializer_list = std::initializer_list<T>;
    template<class T> using optional = std::optional<T>;
    template<class T> using vector = std::vector<T>;
protected:
    string name;

    optional<debug_information_type> debug_information_type;
    optional<input_type> input_type;    // -x
    optional<string> m_std;             // --std='arg'
    optional<string> compiler_files;    // -B'prefix'
    optional<path> system_root;         // --sysroot'dir'
    optional<path> working_directory;   // -working-directory='dir'
    optional<bool> verb;                // -v
    optional<bool> m_pic;
    optional<bool> m_shared;

    vector<path> include_paths;         // -Idir
    vector<path> include_quote_paths;   // -iquote dir
    vector<string> m_libs;
    vector<path> m_lib_paths;
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

    std::string program() const {
        return name;
    }

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

    auto& libs(std::ranges::range auto& libs) {
        for(auto l : libs) lib(l); return *this;
    }

    auto& lib_paths(std::ranges::range auto& paths) {
        for(auto p : paths) m_lib_paths.push_back(p);
        return *this;
    }

    auto& include(const path& p) { include_paths.push_back(p); return *this; }

    auto& include(initializer_list<path>&& includes) {
        include_paths.insert(include_paths.end(), includes);
        return *this;    
    }

    template<std::ranges::range R>
    requires std::is_same_v<std::remove_cvref_t<std::ranges::range_value_t<R>>, std::filesystem::path>
    auto& include(const R& range) {
        for(const auto& v : range) include_paths.push_back(v); return *this;
    }

    auto& quote_include(const path& p) { include_quote_paths.push_back(p); return *this; }

    auto& definition(const definition_t& d) {
        m_definitions.push_back(d); return *this;
    }
    
    auto& definition(string def) {
        return definition(definition_t{std::move(def)});
    }

    auto& definitions(initializer_list<definition_t>&& il) {
        for(auto& d : il) definition(d); return *this;
    }

    auto& verbose(bool val) {verb = val; return *this;}

    auto& position_independent_code(bool val) { m_pic = val; return *this;}

    auto& shared(bool val) { m_shared = val; return *this;}

    //

    template <class I>
    struct compilation_to {
        const command_builder& cc;
        std::remove_cvref_t<I> inputs;

        cmd::command to(output_type ot, path out) const {
            return cc.compilation(inputs, ot, out);
        }

        cmd::command to(path out) const {
            return to(def, out);
        }

        cmd::command to_object(path out) const {
            return to(object_file, out);
        }
    };

    auto compilation_of(const path& src) const {
        return compilation_to<vector<path>>{*this, {src} };
    }

    auto compilation_of(const std::ranges::range auto& inputs) const {
        return compilation_to<decltype(inputs)>{*this, inputs};
    }

    template <class T>
    auto compilation_of(const initializer_list<T>& inputs) const {
        return compilation_to<vector<T>>{ *this, inputs };
    }

    cmd::command compilation(const initializer_list<path>& inputs, const output_type_t& ot, const path& out) const {
        return {name, args_to_string_vec(inputs, ot, out)};
    }

    cmd::command compilation(const std::ranges::range auto& inputs, const output_type_t& ot, const path& out) const {
        return {name, args_to_string_vec(inputs, ot, out)};
    }

    cmd::command make_rule_creation(const initializer_list<path>& inputs) const { return _mrc(inputs); }
    cmd::command make_rule_creation(const std::ranges::range auto& inputs) const { return _mrc(inputs); }
    cmd::command make_rule_creation(const path& path) const { return _mrc(std::array{path}); }

    vector<string> args_to_string_vec(
        const std::ranges::range auto& inputs,
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
        if(m_shared and *m_shared)
            args.emplace_back("-shared");
        if(m_pic and *m_pic)
            args.emplace_back("-fpic");
        
        for(const auto& def : m_definitions)
            args.emplace_back(def.string());
        
        for(auto p : include_paths)
            args.emplace_back("-I"+p.string());
        
        for(auto p : include_quote_paths)
            args.emplace_back("-iquote "+p.string());
        
        for(auto& input : inputs)
            args.emplace_back(input.string());

        for(auto& lib_p : m_lib_paths)
            args.emplace_back("-L"+lib_p.string());

        for(auto& lib : m_libs)
            args.emplace_back("-l"+lib);

        return args;
    }
    
protected:
    cmd::command _mrc(const std::ranges::range auto& inputs) const {
        auto args = args_to_string_vec(inputs, std::nullopt, std::nullopt);
        args.push_back("-M");
        return {name, args};
    }
};

}