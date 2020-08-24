#pragma once

#include <filesystem>
#include <iterator>
#include <concepts>
#include "../gcc_like_driver.hpp"
#include <set>
#include "../environment.hpp"
#include <stdio.h>
#include <string_view>
#include <vector>
#include "../ar.hpp"
#include <ranges>
#include <fstream>
#include <sstream>
#include "../unix/ipstream.hpp"

template<class It>
concept path_iterator = std::input_iterator<It> && std::same_as<std::filesystem::path, std::iter_value_t<It>>;

struct object_set : std::set<std::filesystem::path> {
    using std::set<std::filesystem::path>::set;

    void to_thin_archive(const std::filesystem::path& path) {
        if(std::filesystem::exists(path)) remove(path);
        environment::execute(
            ar::insert()
            .to_thin_archive(path)
            .members(begin(), end())
        );
    }

    void to_thin_static_lib(std::filesystem::path dir, std::string_view name) {
        to_thin_archive(dir.append("lib").concat(name).replace_extension(".a"));
    }
};

struct source_set : std::set<std::filesystem::path> {
    using std::set<std::filesystem::path>::set;

    template<std::ranges::range R>
    source_set(const R& r)
    : std::set<std::filesystem::path>::set(r.begin(), r.end()) {}

    static inline source_set from_dir(std::filesystem::path dir, std::string_view ext) {
        source_set s;
        for(auto de : std::filesystem::directory_iterator{dir})
            if(de.is_regular_file() and de.path().extension() == ext)
                s.insert(de.path());
        return s;
    }

protected:
    bool outdated(
        const std::filesystem::path& src,
        const std::filesystem::path& out,
        const gcc_like_driver::command_builder& cc
    ) {
        std::vector<std::filesystem::path> deps;
        {
            unix::ipstream stream{cc.make_rule_creation({src})};
            stream.ignore(std::numeric_limits<std::streamsize>::max(), ':');

            while(stream) {
                std::string str;
                stream >> str;
                if(str.empty() or str.size() == 1 and str[0] == '\\')
                    continue;
                deps.emplace_back(str);
            }
        }

        for(auto dep : deps) if(last_write_time(dep) > last_write_time(out)) return true;
        return false;
    }

    void check_deps_and_compile(
        const std::filesystem::path& src,
        const gcc_like_driver::output_type& ot,
        const std::filesystem::path& out,
        const gcc_like_driver::command_builder& cc
    ) {
        if(not std::filesystem::exists(out) or outdated(src, out, cc)) {
            environment::execute(cc.compilation({src}, ot, out));
        }
    }
public:
    auto compile_to_objects(const std::filesystem::path& dir, const gcc_like_driver::command_builder& cc) {
        object_set objs;
        std::filesystem::create_directories(dir);

        for(const std::filesystem::path& src : *this) {
            auto out = dir/src.filename().replace_extension(".o");
            check_deps_and_compile(src, gcc_like_driver::object_file, out, cc);
            objs.insert(out);
        }

        return objs;
    }

    void compile_to_executable(std::filesystem::path out, gcc_like_driver::command_builder& cc) {
        auto dir = std::filesystem::path{out}.remove_filename();

        if(not dir.empty()) std::filesystem::create_directories(dir);

        auto has_outdated_srcs = [&]() {
            for(std::filesystem::path src : *this)
                if(outdated(src, out, cc)) return true;
            return false;
        };

        if(std::filesystem::exists(out) and not has_outdated_srcs()) return;

        environment::execute(cc.compilation(*this, gcc_like_driver::def, out));
    }
};