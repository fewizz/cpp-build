
#include <filesystem>
#include <vector>
#include <string>
#include "../build/build.hpp"
#include "../build/configuration.hpp"
#include "clap/gnu_clap.hpp"
#include "../environment.hpp"
#include "../gcc_like_driver.hpp"

struct static_lib {
    std::string name;
    std::vector<std::filesystem::path> include_paths;
    source_set sources;
};

static_lib configure(gnu::clap& clap);

void exec(std::vector<std::string> args) {
    std::string config_name;

    gnu::clap clap;
    clap.value('c', "configuration", config_name);

    auto lib = configure(clap);
    clap.parse(args);

    auto config = configuration::by_name(config_name);

    auto cc = environment::cxx_compile_command_builder();
    cc.std(gcc_like_driver::cxx20);
    config.apply(cc);

    cc.include(lib.include_paths);

    std::filesystem::path config_dir = "build/"+config_name;
    auto objects_dir = config_dir / "objects";
    std::filesystem::create_directories(objects_dir);

    std::cout << "["+lib.name+"] compile\n" << std::flush;
    object_set objs = lib.sources.compile_to_objects(objects_dir, cc);
    std::cout << "["+lib.name+"] create thin static lib\n" << std::flush;
    objs.to_thin_static_lib(config_dir, lib.name);
}