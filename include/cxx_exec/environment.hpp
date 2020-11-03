#pragma once

#include <filesystem>
#include "command_processor.hpp"
#include "gcc_like_driver.hpp"
#include <cstdlib>
#include "unix/ipstream.hpp"

struct environment : cmd::processor {

void process(cmd::command command) override {
    if(int code = std::system(command.string().c_str()))
    throw std::runtime_error {
        "'"+command.string()+"' command's exit code is '"
        +std::to_string(code)+"'"
    };
}

static inline std::string cxx_compiler() {
    return variable("CXX", "g++");
}

static inline gcc_like_driver::command_builder cxx_compile_command_builder() {
    return {cxx_compiler()};
}

static inline void execute(cmd::command);

static inline std::string variable(std::string name, std::string def={}) {
    auto val = std::getenv(name.c_str());
    return val not_eq nullptr ? std::string{val} : def;
}

static const inline std::string exec_extension =
#ifdef _WIN32
".exe"
#else
""
#endif
;

static const inline std::string dynamic_lib_extension =
#ifdef _WIN32
".dll"
#else
".so"
#endif
;

} environment;

inline void environment::execute(cmd::command c) {
    ::environment.process(c);
}