#pragma once

#include <filesystem>
#include <iterator>
#include <concepts>
#include "../gcc_like_driver.hpp"
#include <span>
#include "../environment.hpp"
#include <stdio.h>
#include <vector>
#include "../ar.hpp"
#include <ranges>
#include <fstream>
#include <sstream>
#include "../unix/ipstream.hpp"

template<class It>
concept path_iterator = std::input_iterator<It> && std::same_as<std::filesystem::path, std::iter_value_t<It>>;

struct objects_t : std::vector<std::filesystem::path> {
    using std::vector<std::filesystem::path>::vector;

    void to_thin_archive(std::filesystem::path path) {
        if(std::filesystem::exists(path))
            remove(path);
        environment::execute(
            ar::insert()
            .to_thin_archive(path)
            .members(begin(), end())
        );
    }

    void to_thin_static_lib(std::filesystem::path dir, std::string_view name) {
        to_thin_archive(dir/("lib"+std::string{name}+".a"));
    }
};

struct sources : std::vector<std::filesystem::path> {
    using std::vector<std::filesystem::path>::vector;

    template<std::ranges::range R>
    sources(const R& r)
    : sources(r.begin(), r.end()) {}

protected:
    bool outdated(std::filesystem::path src, std::filesystem::path out, gcc_like_driver::command_builder& cc) {
        cc.make_rule(true);

        std::vector<std::filesystem::path> deps;
        {
            unix::ipstream stream{cc};
            cc.make_rule(false);
            stream.ignore(std::numeric_limits<std::streamsize>::max(), ':');

            while(stream) {
                std::string str;
                stream >> str;
                if(str.empty() or str.size() == 1 and str[0] == '\\')
                    continue;
                deps.emplace_back(str);
            }
        }

        for(auto dep : deps)
            if(last_write_time(dep) > last_write_time(out)) return true;
        return false;
    }

    void check_deps_and_compile(
        std::filesystem::path src,
        std::filesystem::path out,
        gcc_like_driver::command_builder& cc
    ) {
        cc.inputs.clear();
        cc.in(src);

        bool compile;

        if(not std::filesystem::exists(out))
            compile = true;
        else
            compile = outdated(src, out, cc);

        if(compile) {
            cc.out(out);
            environment::execute(cc);
        }
    }
public:
    auto compile_to_objects(std::filesystem::path dir, gcc_like_driver::command_builder& cc) {
        objects_t objs;
        std::filesystem::create_directories(dir);

        cc.out_type(gcc_like_driver::object_file);
        for(std::filesystem::path& src : *this) {
            auto out = dir/src.filename().replace_extension(".o");
            check_deps_and_compile(src, out, cc);
            objs.push_back(out);
        }

        return objs;
    }

    auto compile_to_executable(std::filesystem::path out, gcc_like_driver::command_builder& cc) {
        auto dir = std::filesystem::path{out}.remove_filename();

        if(not dir.empty())
            std::filesystem::create_directories(dir);
        
        cc.out_type(gcc_like_driver::def);

        bool compile = false;
        if(not std::filesystem::exists(out))
            compile = true;
        else for(std::filesystem::path& src : *this) {
            cc.inputs.clear();
            cc.in(src);
            if(outdated(src, out, cc)) {
                compile = true;
                break;
            }
        }
        cc.inputs.clear();
        cc.in(*this);
        cc.out(out);
        environment::execute(cc);
        return *this;
    }
};