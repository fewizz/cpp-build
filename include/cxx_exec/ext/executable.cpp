#include "build_base.cpp"

static vector<path> input_files;

on_startup __executable() {
    output_extension = environment::exec_extension;

    __after_build = [](const vector<path>& objects, vector<path>& updated_objects) {
        info("create executable");

        auto inputs = objects;
        inputs.insert(inputs.end(), input_files.begin(), input_files.end());

        environment::execute(
            cc.compilation_of(inputs)
            .to(
                output_path_provider()
            )
        );
    };
}