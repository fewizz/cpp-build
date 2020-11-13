#pragma once

#include <functional>
#include <filesystem>
#include <utility>
#include <vector>

using update_need_checker = std::function<bool()>;

inline update_need_checker always = [](){ return true; };
inline update_need_checker never = [](){ return false; };

inline update_need_checker if_outdated_by_date(const std::filesystem::path& src, const std::filesystem::path& out) {
    return [&]() {
        return
            not std::filesystem::exists(out)
            or std::filesystem::last_write_time(src) > std::filesystem::last_write_time(out);
    };
};

#include "unix/ipstream.hpp"
#include "gcc_like_driver.hpp"

inline update_need_checker if_outdated_by_date_make(
    const gcc_like_driver::command_builder& cc,
    const std::filesystem::path& src,
    const std::filesystem::path& out
) {
    return [&]() {
        if(not std::filesystem::exists(out)) return true;

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
        for(auto dep : deps)
            if(std::filesystem::last_write_time(dep) > std::filesystem::last_write_time(out))
                return true;
        return false;
    };
};

