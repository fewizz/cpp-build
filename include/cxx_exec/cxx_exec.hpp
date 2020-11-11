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
    std::string m_args;

    command_builder(const path& source)
    : source{ source } {}

    auto& output(const path& output) {
        m_output = output;
        return *this;
    }

    operator cmd::command () {
        std::vector<std::string> args;
        args.push_back(source.string());
        if(m_output) args.push_back("--output="+m_output->string());
        return {"cxx-exec", args};
    }

};

}