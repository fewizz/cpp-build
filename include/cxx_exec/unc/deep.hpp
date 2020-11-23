#pragma once

#include "by_dependencies_date.hpp"
#include "nlohmann/json.hpp"
#include "fstream"
#include <filesystem>
#include <functional>
#include <unordered_map>

inline auto for_each_hash(
    const std::ranges::range auto& r,
    std::function<void(const std::filesystem::path&, std::string)> handler
){
    cmd::command c{"sha256sum", r};
    c.append_arg("| cut -d \" \" -f 1");

    auto pipe = environment::open_pipe(c);

    for(const auto& p : r) {
        std::string hash;
        pipe >> hash;

        handler(p, std::move(hash));
    }
}

inline std::string hash_of_file(std::filesystem::path file) {
    std::string hash;
    cmd::command c{"sha256sum", file.string(), "| cut -d \" \" -f 1"};
    environment::open_pipe(c) >> hash;
    return hash;
}

inline update_need_checker deep(
    const gcc_like_driver::command_builder& cc,
    std::filesystem::path src,
    std::filesystem::path out,
    std::filesystem::path dep_info
) {
    using namespace nlohmann;
    using namespace std;
    using namespace filesystem;

    return [=, &cc]() {
        string compilation_command = cc.compilation_of(array{src}).to_object(out).string();

        string exec_path;
        environment::open_pipe({"which", cc.program()}) >> exec_path;

        string compiler_hash = hash_of_file(exec_path);

        vector<path> deps = dependencies(src, cc);

        auto write = [&](){
            json json_out;
            json_out["compiler"] = compiler_hash;
            json_out["command"] = compilation_command;
    
            json json_deps;

            for_each_hash(deps, [&](const path& dep, string hash) {
                json_deps[dep.string()] = hash;
            });
    
            json_out["deps"] = json_deps;

            ofstream{dep_info} << setw(4) << json_out << endl;
        };

        if(not (exists(out) and exists(dep_info))) {
            write();
            return true;
        }

        json json_in;
        ifstream{dep_info} >> json_in;

        json json_deps = json_in["deps"];
        for(const auto& dep : deps) {
            if(
                not json_deps.contains(dep.string())
                or
                hash_of_file(dep) != json_deps[dep.string()]
            ) {
                write();
                return true;
            }
        }

        return false;
    };
};