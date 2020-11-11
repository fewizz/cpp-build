#pragma once

#include <filesystem>
#include "command_processor.hpp"
#include "gcc_like_driver.hpp"
#include <cstdlib>
#include "unix/ipstream.hpp"
#include "shared_lib_accessor.hpp"

namespace environment {

inline void process(cmd::command command) {
    if(int code = std::system(command.string().c_str()))
    throw std::runtime_error {
        "'"+command.string()+"' command's exit code is '"
        +std::to_string(code)+"'"
    };
}

inline std::string variable(std::string name, std::string def={}) {
    auto val = std::getenv(name.c_str());
    return val not_eq nullptr ? std::string{val} : def;
}

inline std::string cxx_compiler() {
    return variable("CXX", "g++");
}

inline gcc_like_driver::command_builder cxx_compile_command_builder() {
    return {cxx_compiler()};
}

inline void execute(cmd::command c) {
    process(c);
}

#include <io.h>

inline void change_dir(std::filesystem::path dir) {
    if(chdir(dir.string().c_str()) == -1)
        throw std::runtime_error {
            "change directory to '"+dir.string()+"'"
        };
}

inline void change_dir(std::filesystem::path dir, std::function<void()> fun) {
    auto prev = getcwd(nullptr, 0);

    try {
        change_dir(dir);
        fun();
        change_dir(prev);
    }
    catch(...) {
        free(prev);
        throw;
    }
}

inline shared_lib_accessor load_shared_library(const std::filesystem::path& path) {
    auto instance = LoadLibrary(path.string().c_str());
    if(!instance) throw std::runtime_error{"load library '"+path.string()+"'"};
    return {instance};
}

const inline std::string exec_extension =
#ifdef _WIN32
".exe"
#else
""
#endif
;

const inline std::string dynamic_lib_extension =
#ifdef _WIN32
".dll"
#else
".so"
#endif
;

}