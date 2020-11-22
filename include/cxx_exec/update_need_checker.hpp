#pragma once

#include <functional>
#include <filesystem>
#include <utility>
#include <vector>

using update_need_checker = std::function<bool()>;

inline update_need_checker always = [](){ return true; };
inline update_need_checker never = [](){ return false; };

inline update_need_checker by_date(const std::filesystem::path& src, const std::filesystem::path& out) {
    return [&]() {
        return
            not std::filesystem::exists(out)
            or std::filesystem::last_write_time(src) > std::filesystem::last_write_time(out);
    };
};

#include "unix/ipstream.hpp"
#include "gcc_like_driver.hpp"
#include "environment.hpp"

inline update_need_checker by_deps_date(
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
            auto stream = environment::open_pipe(cc.make_rule_creation(src));
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

#include "command.hpp"
#include <iostream>
#include "nlohmann/json.hpp"
#include <fstream>

inline update_need_checker deep(
    const gcc_like_driver::command_builder& cc,
    const std::filesystem::path& src,
    const std::filesystem::path& out,
    const std::filesystem::path& dep_info
) {
    using namespace nlohmann;
    using namespace std;
    using namespace filesystem;

    return [&](){
        bool needs_rebuild = false;
        if(not (exists(out) && exists(dep_info)))
            needs_rebuild = true;

        string exec_path;
        environment::open_pipe(
            {
                "which",
                array{cc.program()}
            }
        ) >> exec_path;

        string compiler_hash;
        environment::open_pipe({"sha256sum", vector{ exec_path }}) >> compiler_hash;

        json json_in ;
        ifstream{dep_info} >> json_in;

        if(json_in.contains("compiler") && json_in["compiler"] == compiler_hash) {
            needs_rebuild = true;
            goto generate;
        }

    generate:
        json json_out;
        json_out["compiler"] = compiler_hash;
        json_out["command"] = vector{cc.compilation_of(array{src}).to_object(out).string()};

        {
            auto stream = environment::open_pipe(cc.make_rule_creation(src));
            stream.ignore(numeric_limits<streamsize>::max(), ':');

            json deps;

            while(stream) {
                string str;
                stream >> str;
                if(str.empty() or str.size() == 1 and str[0] == '\\')
                    continue;
                string hash;
                environment::open_pipe({"sha256sum", vector{ str }}) >> hash;
                deps[str] = hash;
            }

            json_out["deps"] = deps;
        }

        path dep_info_p = dep_info;
        if(not dep_info.has_extension()) dep_info_p.replace_extension(".json");
        ofstream{dep_info_p} << setw(4) << json_out;

        return false;
    };
};

