#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include <system_error>
#include <filesystem>
#include <numeric>

struct program_executor {
    std::string name;
    std::vector<std::string> args;

    program_executor(std::string name, std::vector<std::string> args = {})
        : name{name}, args{args} {};

    program_executor(std::filesystem::path path, std::vector<std::string> args = {})
        : name{path.string()}, args{args} {};
    
    virtual void execute() {
        execute(args);
    }

    void operator()() {
        execute();
    }

protected:
    void execute(std::vector<std::string>& args) {
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
            throw std::system_error(
                code,
                std::generic_category(),
                "program, executed with command \""
                +full_command+
                "\" returned exit code: "
                +std::to_string(code)
            );
    }
};