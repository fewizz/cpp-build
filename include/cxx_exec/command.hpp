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

using namespace string_util;

class command {
    std::string m_command;

public:
    template<convertible_to_string_view_or_string... Strs>
    command(Strs&&... strs) {
        m_command = join(' ', std::forward<Strs>(strs)...);
    }

    template<std::input_iterator It>
    requires(convertible_to_string_view_or_string<std::iter_value_t<It>>)
    command(convertible_to_string_view_or_string auto& name, const It& beg, const It& end)
    : command(name, std::ranges::subrange{beg, end}){}

    template<std::ranges::range R>
    requires(convertible_to_string_view_or_string<std::ranges::range_value_t<R>>)
    command(convertible_to_string_view_or_string auto& name, const R& args) {
        m_command = convert_to_string(name) + " " + join(args, ' ');
    }

    std::string string() const {
        return m_command;
    }

    std::string program() const {
        return m_command.substr(0, m_command.find(' '));
    }
};

}
