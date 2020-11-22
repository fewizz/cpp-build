#include "build_base.cpp"
#include "clap/gnu_clap.hpp"

on_startup __static_lib() {
    output_extension = ".a";

    __after_build = [](const vector<path>& objects, vector<path>& updated_objects) {
        info("create thin static lib");

        environment::execute(
            ar::command_builder{
                output_path_provider(),
                ar::insert{}.verbose().make_thin_archive()
            }.members(objects)
        );

        return true;
    };
}