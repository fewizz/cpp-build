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

static vector<filesystem::path> input_files;

on_startup __shared_lib() {
    output_extension = environment::shared_lib_extension;

    __after_build = [](const vector<path>& objects, vector<path>& updated_objects) {
        info("create shared lib");

        cc.shared(true);
        cc.position_independent_code(true);
        
        auto inputs = objects;
        inputs.insert(inputs.end(), input_files.begin(), input_files.end());

        environment::execute(
            cc.compilation_of(inputs)
            .to(
                output_path_provider()
            )
        );

        cc.shared(false);
        cc.position_independent_code(false);
    };
}