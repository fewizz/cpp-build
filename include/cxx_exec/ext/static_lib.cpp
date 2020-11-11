#include <filesystem>
#include <stdlib.h>
#include <string_view>
#include <vector>
#include <string>
#include "../build/configuration.hpp"
#include "../ar.hpp"
#include "clap/gnu_clap.hpp"
#include "../environment.hpp"
#include "../gcc_like_driver.hpp"

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;

extern "C" const char* __name();

string_view name();
vector<path> sources();

inline void info(auto str) {
    cout << "["+string{name()}+"] " << str << "\n" << flush;
}

void build(const path& output_dir, command_builder& cc);

extern "C" void __build(const void* output_dir, void* cc) {
    build(*((const path*)output_dir), *((command_builder*)cc));
}

extern "C" void configure();

function<void(command_builder&)> before_build = {};
function<void(gnu::clap&)> before_clap_parse = {};

int main(int argc, char* argv []) {
    configure();

    string config_name;
    bool clean = false;
    
    gnu::clap clap;
    clap.value('c', "configuration", config_name);
    clap.flag("clean", clean);

    auto cc = environment::cxx_compile_command_builder();
    if(before_clap_parse) before_clap_parse(clap);
    clap.parse(argv, argv + argc);

    path build_dir = "build";
    if(clean) {
        info("clean");
        remove_all(build_dir);
    }

    if(config_name.empty()) return EXIT_SUCCESS;
    auto config = configuration::by_name(config_name);

    config.apply(cc);

    build(build_dir/config_name, cc);
}

void build(const path& output_dir, command_builder& cc) {
    if(before_build) before_build(cc);
    auto objects_dir = output_dir/"objects";
    create_directories(objects_dir);

    vector<path> objects;

    for(auto source_path : sources()) {
        info("compile " + source_path.string());
        path object_path = objects_dir/source_path.filename().replace_extension(".o");

        environment::execute(
            cc.compilation_of(source_path).to_object(object_path)
        );

        objects.push_back(object_path);
    }

    info("create thin static lib");

    environment::execute(
        ar::command_builder{
            output_dir/(string{name()} + ".a"),
            ar::insert{}.verbose().make_thin_archive()
        }.members(objects)
    );
}