#pragma once

#include <bits/iterator_concepts.h>
#include <string>
#include <filesystem>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#include <ranges>
#include <variant>
#include <iterator>
#include "string_util.hpp"

namespace cmd {

class command {
    std::string m_command;

public:
    command(const auto&... strs)
    :
    m_command{string_util::join(' ', strs...)} {}

    template<std::input_iterator It>
    command(const auto& name, const It& beg, const It& end)
    :
    command(name, std::ranges::subrange{beg, end}) {}

    template<std::ranges::range R>
    command(const auto& name, const R& args)
    :
    m_command{string_util::to_string(name) + " " + string_util::join(' ', args)} {}

    std::string string() const {
        return m_command;
    }

    std::string program() const {
        return m_command.substr(0, m_command.find(' '));
    }

    void append_arg(const auto& a) {
        m_command += ' ';
        m_command += string_util::to_string_view_or_string(a);
    }
};

}
