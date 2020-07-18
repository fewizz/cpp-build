#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <system_error>
#include <filesystem>
#include <numeric>

struct command_executor {
    std::string name;
    std::vector<std::string> args;

    command_executor (
        std::string name,
        std::vector<std::string> args = {}
    )
    : name{name}, args{args} {};
    
    void execute() {
        execute(args);
    }

protected:
    void execute(std::vector<std::string>& args) const {
        auto args_str = std::accumulate(
            args.begin(),
            args.end(),
            std::string{},
            [](const auto& s1, const auto& s2) {
                return s1 + " " + s2;
            }
        );
        
        std::string full_command = name + " " + args_str;

        if(int code = std::system(full_command.c_str())) 
            throw std::system_error {
                code,
                std::generic_category(),
                "program, executed with command \""
                +full_command+
                "\" returned exit code: "
                +std::to_string(code)
            };
    }
};