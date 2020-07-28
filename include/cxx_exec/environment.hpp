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
    if(auto comp = std::getenv("CXX"))
        return {comp};
    return {"clang++"};
}

static inline gcc_like_driver::command_builder cxx_compile_command_builder() {
    return {cxx_compiler()};
}

static inline void execute(cmd::command);

} environment;

inline void environment::execute(cmd::command c) {
    ::environment.process(c);
}