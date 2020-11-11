#pragma once

#include "../shared_library_accessor.hpp"
#include "clap/gnu_clap.hpp"
#include "../gcc_like_driver.hpp"
#include <string_view>

struct static_lib_accessor : shared_library_accessor {

    static_lib_accessor(shared_library_accessor&& shared_accessor)
    : shared_library_accessor(std::move(shared_accessor)) {}

    std::string_view name() {
        return run<const char*()>("name");
    }

    void build(
        const std::filesystem::path& path,
        gcc_like_driver::command_builder& cc
    ) {
        run<void(const void*, void*)>("_build", &path, &cc);
    }
};