#pragma once

#include <string>
#include <filesystem>
#include <string_view>
#include <type_traits>
#include <vector>
#include <ranges>
#include <variant>
#include <iterator>

namespace cmd {

class command {
    std::string m_command;

public:

    command(std::string_view command) 
    : m_command{command} {}

    template<std::input_iterator It>
    command(const auto& name, const It& beg, const It& end)
    : command(name, std::ranges::subrange{beg, end}){}

    command(const auto& name, const std::ranges::range auto& args) {
        struct to_string_t {
            std::string_view operator()(const char* cstr) { return cstr; }
            std::string_view operator()(std::string_view val) { return val; }
            std::string_view operator()(const std::string& str) { return str; }
            std::string operator()(const std::filesystem::path& path) { return path.string(); }
        } to_string;

        auto on_append = [&](auto action) {
            action(to_string(name));

            for(const auto& arg : args) {
                action(" ");
                action(to_string(arg));
            }
        };

        int size = 0;
        on_append([&](std::string_view v){size+=v.size();});
        m_command.reserve(size);
        on_append([&](std::string_view v){m_command+=v;});
    }

    std::string string() const {
        return m_command;
    }
};

}
