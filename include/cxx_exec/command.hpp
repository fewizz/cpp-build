#pragma once

#include <string>
#include <filesystem>
#include <vector>
#include <ranges>

namespace cmd {

class command {
    std::string m_command;
public:

    command(std::string command) 
    :
    m_command{command}
    {}

    command(std::string command, std::filesystem::path arg) 
    :
    m_command{command+" "+arg.string()}
    {}

    template<std::input_iterator It>
    command(const auto& name, const It& beg, const It& end)
    : command(name, std::ranges::subrange{beg, end}){}

    command(const auto& name, const std::ranges::range auto& args) {
        if constexpr(std::is_same_v<decltype(name), std::filesystem::path>)
            m_command+=name.string();
        else m_command+=name;

        for(auto& arg : args) {
            m_command+=" ";
            m_command+=arg;
        }
    }

    std::string string() {
        return m_command;
    }
};

}
