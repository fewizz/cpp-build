#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include "command.hpp"

namespace cxx_exec {

struct command_builder {
    using path = std::filesystem::path;

    path source;
    std::optional<path> m_output;
    std::optional<bool> m_compile_only;
    std::optional<bool> m_verbose;
    std::string m_args;

    command_builder(const path& source)
    : source{ source } {}

    auto& output(const path& output) {
        m_output = output;
        return *this;
    }

    auto& compile_only(bool val) {
        m_compile_only = val;
        return *this;
    }

    auto& verobe(bool val) {
        m_verbose = val;
        return *this;
    }

    auto& operands(std::string ops) {
        m_args = ops;
        return *this;
    }

    operator cmd::command () {
        std::vector<std::string> args;
        args.push_back(source.string());
        if(m_output) args.push_back("--output="+m_output->string());
        if(m_verbose and *m_verbose) args.push_back("--verbose");
        if(m_compile_only and *m_compile_only) args.push_back("--compile-only");
        if(not m_args.empty()) {
            args.push_back("--");
            args.push_back(m_args);
        }
        return {"cxx-exec", args};
    }

};

inline command_builder compilation_of(const std::filesystem::path& src) {
    return command_builder{src}.compile_only(true);
}


inline command_builder execution_of(const std::filesystem::path& src) {
    return command_builder{src};
}

}