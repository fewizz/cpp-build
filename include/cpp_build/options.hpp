#pragma once

#include <vector>
#include <string>
#include <functional>
#include <algorithm>
#include <map>

using args_vec = std::vector<std::string>;

struct gnu_options_parser {
    using handler_t = std::function<void(std::string)>;
    std::map<char, handler_t> name_to_handler;
    std::map<std::string, handler_t> long_name_to_handler;

    void option(std::vector<char> names, std::vector<std::string> long_names, handler_t handler) {
        std::for_each(
            names.begin(),
            names.end(),
            [&](auto n){name_to_handler.emplace(n, handler);}
        );
        std::for_each(
            long_names.begin(),
            long_names.end(),
            [&](auto n){long_name_to_handler.emplace(n, handler);}
        );
    }

    void parse(args_vec& args) {
        for(auto arg = args.begin(); arg != args.end(); arg++) {
            int size = arg->size();
            char first_ch = arg->at(0);
            char second_ch = arg->at(1);

            handler_t& handler =
                second_ch == '-' ?
                long_name_to_handler.at(arg->substr(2)) :
                name_to_handler.at(second_ch);

            bool has_arg = arg+1 != args.end() && (arg+1)->at(0) != '-';
            if(has_arg)
                arg++;
            std::string opt_arg = has_arg ? *arg : "";
            handler(opt_arg);
        }
    }
};