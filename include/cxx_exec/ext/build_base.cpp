#include <filesystem>
#include "../cb/gcc_like_driver.hpp"
#include <iostream>
#include "../environment.hpp"
#include "../cb/ar.hpp"
#include "named_log.cpp"

using namespace std;
using namespace filesystem;
using namespace gcc_like_driver;
extern "C" void build();

using sources_t =
    vector<variant<
        vector<path>,
        path
    >>;

vector<path> sources();

extern "C" const char* __name() { return name().data(); }

static path output_dir = "build";
static function<path()> object_dir_provider = [](){ return output_dir/"object"; };
static string output_extension;
static string output_prefix;
static function<path()> output_path_provider = [](){ return output_dir/(output_prefix + string{name()} + output_extension); };

static gcc_like_driver::command_builder cc = environment::cxx_compile_command_builder();
static function<void(vector<string_view>)> args_parser;

#include "../unc/deep.hpp"
std::function<update_need_checker(
    const gcc_like_driver::command_builder&,
    const path&,
    const path&
)> update_need_checker_provider =
    [](const gcc_like_driver::command_builder& cc, const path& src, const path& out) {
        return deep(cc, src, out, path{out}.replace_extension(".json"));
    };

function<void()> before_build;
function<void()> after_build;
output_type* __output_type;
function<void(const vector<path>& objects, vector<path>& updated_objects)> __after_build;

int main(int argc, char* argv []) {
    if(args_parser) args_parser(vector<string_view> {argv + 1, argv + argc - 1});
    build();
}

void build() {
    if(before_build) before_build();

    vector<path> objects;
    vector<path> updated_objects;

    for(auto source_path : sources()) {
        path object_path = object_dir_provider()/source_path.filename().replace_extension(".o");
        create_directories(object_dir_provider());

        cmd::command compilation_command = cc.compilation_of(source_path).to_object(object_path);

        if(update_need_checker_provider(cc, source_path, object_path) () ) {
            info("compile " + source_path.string());
            environment::execute(compilation_command);
            updated_objects.push_back(object_path);
        }

        objects.push_back(object_path);
    }

    if(updated_objects.empty() and exists(output_path_provider())) {
        info("up-to date");
        return ;
    }
    __after_build(objects, updated_objects);
    if(after_build) after_build();
}