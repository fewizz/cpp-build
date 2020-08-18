#pragma once

#include <filesystem>
#include <iterator>
#include <concepts>
#include "../gcc_like_driver.hpp"
#include <span>
#include "../environment.hpp"
#include <vector>
#include "../ar.hpp"
#include <ranges>

namespace build {

template<class It>
concept path_iterator = std::input_iterator<It> && std::same_as<std::filesystem::path, std::iter_value_t<It>>;

struct objects_t : std::vector<std::filesystem::path> {
    using std::vector<std::filesystem::path>::vector;

    void to_thin_archive(std::filesystem::path path) {
        environment::execute(
            ar::insert()
            .to_thin_archive(path)
            .members(begin(), end())
        );
    } 
};

template<path_iterator It>
struct sources_view_t : std::span<std::filesystem::path> {
    using std::span<std::filesystem::path>::span;

    auto& compile_to_objects(std::filesystem::path dir, gcc_like_driver::command_builder& cc) {
        objects_t objs;
        cc.out_type(gcc_like_driver::object_file);

        for(std::filesystem::path& src : *this) {
            auto out = dir/src.filename().replace_extension(".o");
            cc.inputs.clear();
            cc.in(src);
            cc.out(out);
            environment::execute(cc);
            objs.push_back(out);
        }

        return objs;
    }
};

template<path_iterator It>
auto& sources(It begin, It end) {
    return sources_view_t{begin, end};
}

template<std::ranges::range R>
auto& sources(R& r) {
    return sources(r.begin(), r.end());
}

} // build