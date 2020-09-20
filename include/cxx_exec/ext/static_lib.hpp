#include <filesystem>
#include <vector>
#include <string>
#include "../build/build.hpp"
#include "../build/configuration.hpp"
#include "clap/gnu_clap.hpp"
#include "../environment.hpp"
#include "../gcc_like_driver.hpp"

using namespace std;
using namespace filesystem;
using namespace gnu;
using namespace gcc_like_driver;

string name();
source_set sources();

void info(auto str) {
    cout << "["+name()+"] " << str << "\n" << flush;
}

void configure(clap& clap, command_builder& cc);
function<void(path)> on_pre_build = {};

void exec(vector<string> args) {
    string config_name;
    bool clean = false;
    
    gnu::clap clap;
    clap.value('c', "configuration", config_name);
    clap.flag("clean", clean);

    auto cc = environment::cxx_compile_command_builder();

    configure(clap, cc);
    clap.parse(args);

    path build_dir = "build";
    if(clean) {
        info("clean");
        remove_all(build_dir);
    }

    if(config_name.empty()) return;
    auto config = configuration::by_name(config_name);

    config.apply(cc);

    path config_dir = build_dir/config_name;
    auto objects_dir = config_dir/"objects";
    create_directories(objects_dir);

    if(on_pre_build)
        on_pre_build(config_dir);

    info("compile");
    object_set objs = sources().compile_to_objects(objects_dir, cc);
    info("create thin static lib");
    objs.to_thin_static_lib(config_dir, name());
}