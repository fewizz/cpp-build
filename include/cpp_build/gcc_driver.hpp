#pragma once

#include "program_executor.hpp"
#include <memory>
#include <array>
#include <algorithm>
#include <filesystem>

namespace gcc_driver {

struct type_t {
    const std::vector<std::string> names;
};

namespace type {

const type_t c{{"c"}};
const type_t cxx{{"C", "cc", "CC", "cp", "cpp", "CPP", "c++", "C++", "cxx", "CXX"}};
const type_t c_header{{"h"}};
const type_t cxx_header{{"H", "hh", "hpp"}};
const type_t pp_c{{"i"}};
const type_t pp_asm{{"asm"}};
const type_t cxx_module{{"ccm", "c++m", "cppm", "cxxm"}};
const type_t pp_cxx_module{{"iim"}};
const type_t object{{"lib", "obj"}};
const type_t module_file{{"pcm"}};

}

struct lang_t {
    const char* name;
    constexpr lang_t(const char* name):name{name}{}
};

namespace lang {

constexpr lang_t c("c"), c_header("c-header"), cxx_output("c++-output"),
cxx("c++"), cxx_header("c++-header"), cxx_cpp_output("c++-cpp-output"),
assembler("assembler"), assembler_with_cpp("assembler-with-cpp"), none("");

struct std_t {
    const lang_t language;
    const ::std::vector<::std::string> names;
};

namespace std {

std_t
    c89{lang::c, {"c89", "c90", "iso9899:1990"}},
    c94{lang::c, {"iso9899:199409"}},
    gnu89{lang::c, {"gnu89", "gnu90"}},
    c99{lang::c, {"c99", "iso9899:1999", "c9x", "iso9899:199x"}},
    gnu99{lang::c, {"gnu99", "gnu9x"}},
    c11{lang::c, {"c11", "iso9899:2011", "c1x", "iso9899:201x"}},
    gnu11{lang::c, {"gnu11", "gnu1x"}},
    c17{lang::c, {"c17", "iso9899:2017", "c18", "iso9899:2018"}},
    gnu17{lang::c, {"gnu17", "gnu18"}},
    c2x{lang::c, {"c2x", "iso9899:2017", "c18", "iso9899:2018"}},
    gnu2x{lang::c, {"gnu2x", "iso9899:2017", "c18", "iso9899:2018"}},

    cxx98{lang::cxx, {"c++98", "c++03"}},
    gnucxx98{lang::cxx, {"gnu++98", "gnu++03"}},
    cxx11{lang::cxx, {"c++11", "c++0x"}},
    gnucxx11{lang::cxx, {"gnu++11", "gnu++0x"}},
    cxx14{lang::cxx, {"c++14", "c++1y"}},
    gnucxx14{lang::cxx, {"gnu++14", "gnu++1y"}},
    cxx17{lang::cxx, {"c++17", "c++1z"}},
    gnucxx17{lang::cxx, {"gnu++17", "gnu++1z"}},
    cxx20{lang::cxx, {"c++20", "c++2a"}},
    gnucxx20{lang::cxx, {"gnu++20", "gnu++2a"}},

    none{lang::none, {""}};
}
}

struct executor : public program_executor {
    lang_t lang{lang::none};

    // --output
    std::filesystem::path output;
    // --std='arg'
    lang::std_t* std{&lang::std::none};

    // -B'prefix'
    std::string compiler_files;
    // --sysroot'dir'
    std::filesystem::path system_root;
    // -working-directory='dir'
    std::filesystem::path working_directory;

    std::vector<std::filesystem::path> input_files;
    void input_file(std::filesystem::path p) { input_files.push_back(p); }
    // -Idir
    std::vector<std::filesystem::path> include_paths;
    void include_path(std::filesystem::path p) { include_paths.push_back(p); }

    int execute() override {
        std::vector<std::string> _args;

        if(!working_directory.empty())
            _args.push_back("-working-directory="+working_directory.string());
        if(std)
            _args.push_back("-std=" + std->names.front());
        
        if(!output.empty())
            _args.push_back("--output="+output.string());
        
        std::for_each(
            include_paths.begin(),
            include_paths.end(),
            [&](std::filesystem::path path) {
                _args.push_back("-I"+path.string());
            }
        );
        std::for_each(
            input_files.begin(),
            input_files.end(),
            [&](std::filesystem::path path) {
                _args.push_back(path.string());
            }
        );

        _args.insert(_args.begin(), args.begin(), args.end());
        return program_executor::execute(_args);
    }
};

}