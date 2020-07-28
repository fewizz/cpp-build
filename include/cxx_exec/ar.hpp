#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <set>
#include "command.hpp"

namespace ar {

struct command_builder {
    std::optional<std::string> archive_name;
    std::vector<std::filesystem::path> members;

    struct option_t {
        char ch;
        std::set<char> modifiers;
    };

    std::optional<option_t> option;

    void insert() {
        option = {'r'};
    };

    void verbose(bool val) {
        if(val) option->modifiers.insert('v');
        else option->modifiers.erase('v');
    }

    operator cmd::command() {
        std::vector<std::string> args;
        if(!archive_name)
            throw std::runtime_error("archive name is not specified for ar");
        args.push_back(std::string{1, option->ch});
        args.insert(args.end(), option->modifiers.begin(), option->modifiers.end());
        args.push_back(*archive_name);
        
        for(auto& p : members)
            args.push_back(p.string());
        
        return {"ar", args.begin(), args.end()};
    }
};

}