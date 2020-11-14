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
    using namespace std;
    using namespace filesystem;
    
    return [&]() {
        if(not exists(out)) return true;

        vector<path> deps;
        
        {
            unix::ipstream stream{cc.make_rule_creation({src})};
            stream.ignore(numeric_limits<streamsize>::max(), ':');

            while(stream) {
                string str;
                stream >> str;
                if(str.empty() or str.size() == 1 and str[0] == '\\')
                    continue;
                deps.emplace_back(str);
            }
        }
        for(auto dep : deps)
            if(last_write_time(dep) > last_write_time(out))
                return true;
        return false;
    };
};

