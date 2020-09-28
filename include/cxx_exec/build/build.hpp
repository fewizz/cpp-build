#pragma once

#include <filesystem>
#include <initializer_list>
#include <iterator>
#include <concepts>
#include "../gcc_like_driver.hpp"
#include <set>
#include "../environment.hpp"
#include <stdexcept>
#include <stdio.h>
#include <string_view>
#include <vector>
#include "../ar.hpp"
#include <ranges>
#include <fstream>
#include <ranges>
#include <sstream>
#include "../unix/ipstream.hpp"

template<class It>
concept path_iterator = std::input_iterator<It> && std::same_as<std::filesystem::path, std::iter_value_t<It>>;

template <class In>
struct compile_with {
    In input;

    const std::filesystem::path output;
    const gcc_like_driver::output_type ot;

    void with(const gcc_like_driver::command_builder& cc) {
        environment::execute(cc.compilation(input, ot, output));
    }
};

template <class In>
struct compile_to {
    In input;

    auto to(const std::filesystem::path out, const gcc_like_driver::output_type ot) {
        return compile_with{ std::move(input), out, ot };
    }

    auto to_object(const std::filesystem::path out) {
        return to(out, gcc_like_driver::object_file);
    }

    auto to(const std::filesystem::path out) {
        return to(out, gcc_like_driver::def);
    }
};

auto compile(std::ranges::range auto p) { return compile_to{p}; }

template <class In>
auto compile(std::initializer_list<In> p) { return compile_to{std::move(p)}; }