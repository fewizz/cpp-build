#pragma once

#include <iterator>
#include <ranges>
#include <filesystem>
#include <type_traits>
#include <utility>

namespace string_util {
    template<class T>
    auto convert_to_string_view(T&& value) {
        using namespace std;
        using D = decay_t<remove_cvref_t<T>>;

        if constexpr (is_same_v<D, string> and is_lvalue_reference_v<T>)
            return string_view{value.c_str()};
        else
        if constexpr (is_same_v<D, string_view> or is_same_v<D, const char*> or is_same_v<D, char*>)
            return string_view{forward<T>(value)};
        else
            return;
    }

    template<class T>
    auto convert_to_string(T&& value) {
        using namespace std;
        using D = decay_t<remove_cvref_t<T>>;

        if constexpr (is_same_v<D, string> or is_same_v<D, string_view> or is_same_v<D, const char*> or is_same_v<D, char*>)
            return string{std::forward<T>(value)};
        else if constexpr(is_same_v<D, filesystem::path>)
            return value.string();
        else
            return;
    }

    template<class T>
    static constexpr bool is_convertible_to_string_view_v = 
        std::is_same_v<
            std::remove_cvref_t<decltype( convert_to_string_view( std::declval<T>() ) )>,
            std::string_view
        >; 

    template<class T>
    concept convertible_to_string_view = is_convertible_to_string_view_v<T>;
    

    template<class T>
    static constexpr bool is_convertible_to_string_v =
        std::is_same_v<
            std::remove_cvref_t<decltype( convert_to_string( std::declval<T>() ) )>,
            std::string
        >;

    template<class T>
    concept convertible_to_string = is_convertible_to_string_v<T>;

    template<class T>
    concept convertible_to_string_view_or_string =
        convertible_to_string_view<T> or convertible_to_string<T>;

    template<class T>
    auto convert_to_string_view_or_string(T&& value) {
        if constexpr(is_convertible_to_string_view_v<T>)
            return convert_to_string_view(std::forward<T>(value));
        else if(is_convertible_to_string_v<T>)
            return convert_to_string(std::forward<T>(value));
        else return;
    }

    template<convertible_to_string_view_or_string S>
    std::string::size_type size(const S& s) {
        using namespace std;
        using D = decay_t<remove_cvref_t<S>>;
        if constexpr(is_same_v<D, string> or is_same_v<D, string_view>)
            return s.length();
        else if constexpr(is_same_v<D, const char*> or is_same_v<D, char*>)
            return strlen(s);
    }
    
    template<convertible_to_string_view_or_string... Strs>
    inline auto __join(char delimiter, std::string&& res, convertible_to_string_view_or_string auto& str0, Strs... strs) {
        res += delimiter;
        res += str0;
        if constexpr(sizeof...(Strs) == 0)
            return std::move(res);
        else
            return __join(std::move(res), strs...);
    }

    template<convertible_to_string_view_or_string... Strs>
    inline auto join(char delimiter, convertible_to_string_view_or_string auto& str0, Strs... strs) {
        auto s = (... + size(strs));
        std::string str;
        str.reserve(s);
        str += str0;

        if constexpr(sizeof...(Strs) == 0)
            return str;
        else
            return __join(delimiter, std::move(str), strs...);
    }

    template<std::ranges::range R>
    requires convertible_to_string_view_or_string<std::ranges::range_value_t<R>>
    inline std::string::size_type size(const R& range, char delimiter) {
        std::string::size_type s = 0;

        for(const auto& it : range) s += size(it);

        s += std::max(std::string::size_type(0), std::ranges::size(range) - 1);
        return s;
    }

    template<std::ranges::range R>
    requires convertible_to_string_view_or_string<std::ranges::range_value_t<R>>
    inline std::string join(const R& range, char delimiter) {
        using namespace std;
        std::string s;
        s.reserve(size(range, delimiter));

        s += *ranges::begin(range);

        for(auto& it : range | std::ranges::views::drop(1)) {
            s += delimiter;
            s += it;
        }

        return std::move(s);
    }
}