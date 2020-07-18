#pragma once

#include <filesystem>
#include "command_processor.hpp"
#include "gcc_like_driver_executor.hpp"
#include <cstdlib>

namespace environment {

inline gcc_like_driver::gcc_like_driver_executor cxx_compiler() {
    if(auto comp = std::getenv("CXX"))
        return {comp};
    return {"clang++"};
}

constexpr struct : command::command_processor_base {
    using command::command_processor_base::process;

    void process(std::string command) const override {
        if(int code = std::system(command.c_str()))
        throw std::runtime_error {
            "'"+command+"' command's exit code is '"
            +std::to_string(code)+"'"
        };
    }
} command_processor;

inline void execute(std::string command) {
    command_processor.process(command);
}

template<class NT, class It>
void execute(NT name0, It args_begin, It args_end) {
    std::string name{ std::filesystem::path{name0}.string() };

    std::string::size_type total_size = name.length();

    std::for_each(args_begin, args_end, [&](auto& arg) {
        total_size+=std::string_view(arg).length() + 1;
    });

    std::string command{total_size, ' ', std::allocator<char>()};

    auto pos = command.begin();

    command.replace(pos, (pos+=name.length()), name);
    pos++;

    std::for_each(args_begin, args_end, [&](auto& arg) {
        std::string_view v{arg};
        command.replace(pos, (pos+=v.length()), v);
        pos++;
    });

    command_processor.process(command);
}

}