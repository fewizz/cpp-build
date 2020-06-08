#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <numeric>

struct program_executor {
    std::string name;
    std::vector<std::string> args;

    virtual int execute() {
        return execute(args);
    }

protected:
    int execute(std::vector<std::string>& _args) {
        auto args_str = std::accumulate(
            _args.begin(),
            _args.end(),
            std::string{},
            [](const auto& s1, const auto& s2) {
                return s1 + " " + s2;
            }
        );
        return std::system((name + " " + args_str).c_str());
    }
};