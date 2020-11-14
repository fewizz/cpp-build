#include <filesystem>
#include <functional>
#include <stdexcept>
#include <stdlib.h>
#include <string_view>
#include <vector>
#include <string>
#include "../configuration.hpp"
#include "../ar.hpp"
#include "clap/gnu_clap.hpp"
#include "../environment.hpp"
#include "../gcc_like_driver.hpp"
#include "../update_need_checker.hpp"

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;

#define on_startup __attribute__((constructor)) void

string_view name();
vector<path> sources();

extern "C" const char* __name() {return name().data(); }

static path output_dir = "build";
static function<path()> object_dir_provider = [](){ return output_dir/"object"; };
static gcc_like_driver::command_builder cc = environment::cxx_compile_command_builder();
static function<void(vector<string_view>)> args_parser;

std::function<update_need_checker(
    const gcc_like_driver::command_builder&,
    const path&,
    const path&
)> update_need_checker_provider =
    [](const gcc_like_driver::command_builder& cc, const path& src, const path& out) {
        return if_outdated_by_date_make(cc, src, out);
    };

inline void info(auto str) {
    cout << "["+string{name()}+"] " << str << "\n" << flush;
}

void build();

extern "C" void __build() {
    build();
}

function<void(command_builder&)> before_build = {};
function<void(gnu::clap&)> before_clap_parse = {};

int main(int argc, char* argv []) {
    if(args_parser) args_parser(vector<string_view> {argv, argv + argc});

    build();
}

void build() {
    if(before_build) before_build(cc);

    vector<path> objects;
    vector<path> updated_objects;

    for(auto source_path : sources()) {
        path object_path = object_dir_provider()/source_path.filename().replace_extension(".o");
        create_directories(object_dir_provider());

        if(update_need_checker_provider(cc, source_path, object_path) () ) {
            info("compile " + source_path.string());

            environment::execute(
                cc.compilation_of(source_path).to_object(object_path)
            );

            updated_objects.push_back(object_path);
        }

        objects.push_back(object_path);
    }

    if(updated_objects.empty()) {
        info("up-to date");
        return;
    }

    info("create thin static lib");

    environment::execute(
        ar::command_builder{
            output_dir/(string{name()} + ".a"),
            ar::insert{}.verbose().make_thin_archive()
        }.members(objects)
    );
}