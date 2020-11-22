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

namespace cmd {

template<class T>
constexpr auto convert_to_string(T&& value) {
    using namespace std;
    using D = decay_t<remove_cvref_t<T>>;

    if constexpr (is_same_v<D, string> or is_same_v<D, string_view> or is_same_v<D, const char*> or is_same_v<D, char*>)
        return string{std::forward<T>(value)};
    else if constexpr (is_same_v<D, filesystem::path>)
        return value.string();
    else
        return;
}

template<class T>
concept convertible_to_string =
    std::is_same_v<
        std::remove_cvref_t<decltype( convert_to_string( std::declval<T>() ) )>,
        std::string
    >;

class command {
    std::string m_command;

public:
    template<convertible_to_string... Strs>
    command(Strs... strs) {
        m_command = (... + (convert_to_string(strs) + " "));
    }

    template<std::input_iterator It>
    requires(convertible_to_string<std::iter_value_t<It>>)
    command(const convertible_to_string auto& name, const It& beg, const It& end)
    : command(name, std::ranges::subrange{beg, end}){}

    template<std::ranges::range R>
    requires(convertible_to_string<std::ranges::range_value_t<R>>)
    command(const convertible_to_string auto& name, const R& args) {
        auto on_append = [&](auto action) {
            action(convert_to_string(name));

            for(const auto& arg : args) {
                action(" ");
                action(convert_to_string(arg));
            }
        };

        int size = 0;
        on_append([&](std::string v){size+=v.size();});
        m_command.reserve(size);
        on_append([&](std::string v){m_command+=v;});
    }

    std::string string() const {
        return m_command;
    }

    std::string program() const {
        std::string c = string();
        return c.substr(0, c.find(' '));
    }
};

}
