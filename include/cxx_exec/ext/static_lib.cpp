#include <filesystem>
#include <stdlib.h>
#include <vector>
#include <string>
#include "../build/configuration.hpp"
#include "../ar.hpp"
#include "clap/gnu_clap.hpp"
#include "../environment.hpp"
#include "../gcc_like_driver.hpp"

using namespace std;
using namespace filesystem;
using namespace gnu;
using namespace gcc_like_driver;

//string name();
extern "C" const char* name();// { return name().c_str(); }

vector<path> sources();

void info(auto str) {
    cout << "["+string{name()}+"] " << str << "\n" << flush;
}

void configure(gnu::clap& clap, command_builder& cc);
function<void(path)> on_pre_build = {};

int main(int argc, char* argv []) {
    string config_name;
    bool clean = false;
    
    gnu::clap clap;
    clap.value('c', "configuration", config_name);
    clap.flag("clean", clean);

    auto cc = environment::cxx_compile_command_builder();

    configure(clap, cc);
    clap.parse(argv, argv + argc);

    path build_dir = "build";
    if(clean) {
        info("clean");
        remove_all(build_dir);
    }

    if(config_name.empty()) return EXIT_SUCCESS;
    auto config = configuration::by_name(config_name);

    config.apply(cc);

    path config_dir = build_dir/config_name;
    auto objects_dir = config_dir/"objects";
    create_directories(objects_dir);

    if(on_pre_build) on_pre_build(config_dir);

    vector<path> objects;

    for(auto source_path : sources()) {
        info("compile " + source_path.string());
        path object_path = objects_dir/source_path.filename().replace_extension(".o");

        environment::execute(
            cc
                .compilation_of(source_path)
                .to_object(object_path)
        );

        objects.push_back(object_path);
    }
    info("create thin static lib");

    environment::execute(
        ar::command_builder{
            config_dir/(string{name()} + ".a"),
            ar::insert{}.verbose().make_thin_archive()
        }.members(objects)
    );
}