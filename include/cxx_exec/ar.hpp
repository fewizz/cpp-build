#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <set>
#include "command.hpp"

namespace ar {

struct command_builder {
    std::optional<std::filesystem::path> archive;
    std::vector<std::filesystem::path> members;

    struct option_t {
        char ch;
        std::set<char> modifiers;

        operator std::string() {
            std::string result{"-"};
            result+=ch;
            for(auto m : modifiers) result+=m;
            return result;
        }
    };

    std::optional<option_t> option;

    operator cmd::command() {
        std::vector<std::string> args;
        if(!archive)
            throw std::runtime_error("archive name is not specified for ar");
        
        args.push_back(*option);
        args.push_back(archive->string());
        
        for(auto& mp : members)
            args.push_back(mp.string());
        
        return {"ar", args.begin(), args.end()};
    }
};

enum class create { t, f };

template<create Create>
struct members_specifier {
    command_builder cb;

    members_specifier<create::f> member(std::filesystem::path m) { cb.members.push_back(m); return {cb}; }

    template<class Iter>
    members_specifier<create::f> members(Iter begin, Iter end) {
        cb.members.insert(cb.members.end(), begin, end); return {cb}; 
    }

    template<create C=Create> std::enable_if_t<C==create::t, members_specifier<create::f>>
    create_if_not_exists() { cb.option->modifiers.insert('c'); return {cb}; }

    operator cmd::command() { return cb; }
};

enum class archive_prefix { of, to, in };
enum class verbose { t, f };

template<verbose Verbose, archive_prefix Prefix>
struct modifier_choose {
    command_builder cb;

    template<verbose V=Verbose> std::enable_if_t<V==verbose::t, modifier_choose<verbose::f, Prefix>>
    verbose() { cb.option->modifiers.insert('v'); return {cb}; }

    template<archive_prefix P=Prefix> std::enable_if_t<P==archive_prefix::to, members_specifier<create::t>>
    to_archive(std::filesystem::path p) { cb.archive = p; return {cb}; }

    template<archive_prefix P=Prefix> std::enable_if_t<P==archive_prefix::of, members_specifier<create::t>>
    of_archive(std::filesystem::path p) { cb.archive = p; return {cb}; }

    template<archive_prefix P=Prefix> std::enable_if_t<P==archive_prefix::in, members_specifier<create::t>>
    in_archive(std::filesystem::path p) { cb.archive = p; return {cb}; }
};

static inline modifier_choose<verbose::t, archive_prefix::to> insert() {
    command_builder cb;
    cb.option = {'r'};
    return {cb};
}

static inline modifier_choose<verbose::t, archive_prefix::of> contents() {
    command_builder cb;
    cb.option = {'t'};
    return {cb};
}

static inline modifier_choose<verbose::t, archive_prefix::in> del() {
    command_builder cb;
    cb.option = {'d'};
    return {cb};
}

}