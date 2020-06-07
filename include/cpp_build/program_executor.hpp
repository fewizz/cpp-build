#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <numeric>

struct program_executor {
    int virtual execute() = 0;
};

struct cl_program_executor : public program_executor {
    std::string program_name;
    std::vector<std::string> args;

    int execute() override {
        auto args_str = std::accumulate(
            args.begin(),
            args.end(),
            std::string{},
            [](auto& s1, auto& s2) {
                return s1 + " " + s2;
            }
        );
        return std::system((program_name + " " + args_str).c_str());
    }
};