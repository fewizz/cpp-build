#pragma once

#include "../shared_lib_accessor.hpp"
#include "clap/gnu_clap.hpp"
#include "../gcc_like_driver.hpp"
#include <string_view>

struct static_lib_accessor : shared_lib_accessor {

    static_lib_accessor(shared_lib_accessor&& shared_accessor)
    : shared_lib_accessor(std::move(shared_accessor)) {}

    std::string_view name() {
        return run<const char*()>("__name");
    }

    void build(
        const std::filesystem::path& path,
        gcc_like_driver::command_builder& cc
    ) {
        run<void(const void*, void*)>("__build", &path, &cc);
    }
};