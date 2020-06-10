#pragma once

#include "program_executor.hpp"
#include <memory>
#include <array>
#include <algorithm>
#include <filesystem>

namespace gcc {

/*struct type_t {
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

}*/

namespace input_type {

constexpr struct t {
    const char* name{};
    constexpr t(const char* name):name{name}{}
    constexpr t():name{nullptr}{}
}
c("c"), c_header("c-header"), cxx_output("c++-output"),
cxx("c++"), cxx_header("c++-header"), cxx_cpp_output("c++-cpp-output"),
assembler("assembler"), assembler_with_cpp("assembler-with-cpp");

}

namespace lang_std {
    struct t {
        std::string name;
        operator bool() { return !name.empty(); }
    }
    c89{"c89"}, c94{"iso9899:199409"}, gnu89{"gnu89"},
    c99{"c99"}, gnu99{"gnu99"}, c11{"c11"}, gnu11{"gnu11"},
    c17{"c17"}, gnu17{"gnu17"}, c2x{"c2x"}, gnu2x{"gnu2x"},

    cxx98{"c++98"}, gnucxx98{"gnu++98"}, cxx11{"c++11"},
    gnucxx11{"gnu++11"}, cxx14{"c++14"}, gnucxx14{"gnu++14"},
    cxx17{"c++17"}, gnucxx17{"gnu++17"},
    cxx20{"c++20"}, gnucxx20{"gnu++20"};
}

struct driver_executor : public program_executor {
    input_type::t input_type;                // -x
    std::filesystem::path output;            // --output
    lang_std::t std;                         // --std='arg'
    std::string compiler_files;              // -B'prefix'
    std::filesystem::path system_root;       // --sysroot'dir'
    std::filesystem::path working_directory; // -working-directory='dir'

    driver_executor(std::string name, lang_std::t _std)
        :program_executor{name}, std{_std}{}

    std::vector<std::filesystem::path> input_files;
    void input_file(std::filesystem::path p) { input_files.push_back(p); }

    std::vector<std::filesystem::path> include_paths; // -Idir
    void include_path(std::filesystem::path p) { include_paths.push_back(p); }

    driver_executor(std::string name) : program_executor{name}{};

    void execute() override {
        using namespace std;
        vector<string> args{program_executor::args};

        if(!working_directory.empty())
            args.push_back("-working-directory="+working_directory.string());
        if(std)
            args.push_back("-std="+std.name);
        
        if(!output.empty())
            args.push_back("--output="+output.string());
        
        for_each(
            include_paths.begin(),
            include_paths.end(),
            [&](filesystem::path path) {
                args.push_back("-I"+path.string());
            }
        );
        for_each(
            input_files.begin(),
            input_files.end(),
            [&](filesystem::path path) {
                args.push_back(path.string());
            }
        );

        return program_executor::execute(args);
    }
};

}