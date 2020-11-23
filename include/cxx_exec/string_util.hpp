#pragma once

#include <iterator>
#include <ranges>
#include <filesystem>
#include <type_traits>
#include <utility>

namespace string_util {

    namespace internal {
        template<class D, class...Ts>
        static constexpr bool one_of = (... or std::is_same_v<D, Ts>);

        template<class T>
        auto to_string_view(const T& value) {
            using namespace std;
            using D = decay_t<remove_cvref_t<T>>;

            if constexpr (is_same_v<D, string>)
                return string_view{value.c_str()};
            else
            if constexpr (one_of<D, string_view, const char*, char*>)
                return string_view{value};
            else
                return;
        }

        template<class T>
        auto to_string(const T& value) {
            using namespace std;
            using D = decay_t<remove_cvref_t<T>>;

            if constexpr (one_of<D, string, string_view, const char*, char*>)
                return string{value};
            else
            if constexpr(is_same_v<D, filesystem::path>)
                return value.string();
            else
                return;
        }
    }

    template<class T>
    static constexpr bool is_convertible_to_string_view_v = 
        std::is_same_v<
            std::remove_cvref_t<decltype( internal::to_string_view( std::declval<T>() ) )>,
            std::string_view
        >; 

    template<class T>
    concept convertible_to_string_view = is_convertible_to_string_view_v<T>;
    

    template<class T>
    static constexpr bool is_convertible_to_string_v =
        std::is_same_v<
            std::remove_cvref_t<decltype( internal::to_string( std::declval<T>() ) )>,
            std::string
        >;

    template<class T>
    concept convertible_to_string = is_convertible_to_string_v<T>;

    template<convertible_to_string_view T>
    std::string_view to_string_view(const T& v) {
        return internal::to_string_view(v);
    }

    template<convertible_to_string T>
    std::string to_string(const T& v) {
        return internal::to_string(v);
    }

    namespace internal {

        template<class T>
        auto to_string_view_or_string(const T& value) {
            if constexpr(is_convertible_to_string_view_v<T>)
                return to_string_view(value);
            else if(is_convertible_to_string_v<T>)
                return to_string(value);
            else return;
        }
    }

    template<class T>
    concept convertible_to_string_view_or_string =
        convertible_to_string_view<T> or convertible_to_string<T>;

    template<convertible_to_string_view_or_string T>
    std::string_view to_string_view_or_string(const T& value) {
        return internal::to_string_view_or_string(value);
    }

    template<convertible_to_string_view_or_string T>
    std::string::size_type size(const T& t) {
        using namespace std;
        using D = decay_t<remove_cvref_t<T>>;
        if constexpr(internal::one_of<D, string, string_view>)
            return t.length();
        else if constexpr(internal::one_of<D, const char*, char*>)
            return strlen(t);
        else if constexpr(is_same_v<D, filesystem::path>)
            return t.string().length();
    }

    namespace internal {
        template<convertible_to_string_view_or_string... Strs>
        inline std::string join(
            char delimiter,
            std::string&& res,
            const convertible_to_string_view_or_string auto& str0,
            const Strs&... strs
        ) {
            using namespace std;
            res += delimiter;
            res += to_string_view_or_string(str0);
            if constexpr(sizeof...(Strs) == 0)
                return move(res);
            else
                return join(move(res), strs...);
        }
    }

    template<convertible_to_string_view_or_string... Strs>
    inline std::string join(char delimiter, const convertible_to_string_view_or_string auto& str0, const Strs&... strs) {
        using namespace std;

        string::size_type s = (size(str0) + ... + size(strs));
        string str;
        str.reserve(s);
        str += to_string_view_or_string(str0);

        if constexpr(sizeof...(Strs) == 0)
            return str;
        else
            return internal::join(delimiter, move(str), strs...);
    }

    template<std::ranges::range R>
    requires convertible_to_string_view_or_string<std::ranges::range_value_t<R>>
    inline std::string::size_type size(char delimiter, const R& range) {
        std::string::size_type s = 0;

        for(const auto& it : range) s += size(it);

        s += std::max(std::string::size_type(0), std::ranges::size(range) - 1);
        return s;
    }

    template<std::ranges::range R>
    requires convertible_to_string_view_or_string<std::ranges::range_value_t<R>>
    inline std::string join(char delimiter, const R& range) {
        using namespace std;
        std::string res;
        res.reserve(size(delimiter, range));

        res += *ranges::begin(range);

        for(const auto& elem : range | std::ranges::views::drop(1)) {
            res += delimiter;
            res += to_string_view_or_string(elem);
        }

        return std::move(res);
    }
}