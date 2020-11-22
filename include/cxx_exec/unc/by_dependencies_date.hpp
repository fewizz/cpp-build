#pragma once

#include "update_need_checker.hpp"
#include "../gcc_like_driver.hpp"
#include "../environment.hpp"
#include <filesystem>
#include <functional>

inline void for_each_dependency(
    const std::filesystem::path& src,
    const gcc_like_driver::command_builder& cc,
    std::function<void(const std::filesystem::path&)> handler
) {
    using namespace std;
    using namespace filesystem;

    auto stream = environment::open_pipe(cc.make_rule_creation(src));
    stream.ignore(numeric_limits<streamsize>::max(), ':');

    while(stream) {
        string str;
        stream >> str;
        if(str.empty() or str.size() == 1 and str[0] == '\\')
            continue;
        handler(str);
    }
}

inline std::vector<std::filesystem::path> dependencies(
    const std::filesystem::path& src,
    const gcc_like_driver::command_builder& cc
) {
    using namespace std;
    using namespace filesystem;

    vector<path> deps;

    for_each_dependency(src, cc, [&](const path& dep) {
        deps.push_back(dep);
    });

    return std::move(deps);
}

inline update_need_checker by_dependencies_date(
    const gcc_like_driver::command_builder& cc,
    std::filesystem::path src,
    std::filesystem::path out
) {
    using namespace std;
    using namespace filesystem;
    
    return [=, &cc]() {
        if(not exists(out)) return true;
        
        for(auto dep : dependencies(src, cc))
            if(last_write_time(dep) > last_write_time(out))
                return true;
        return false;
    };
};