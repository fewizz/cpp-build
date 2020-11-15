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

#include "build_base.cpp"

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