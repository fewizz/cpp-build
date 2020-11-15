#pragma once

#include "../shared_lib_accessor.hpp"
#include "clap/gnu_clap.hpp"
#include "../gcc_like_driver.hpp"
#include <string_view>

struct build_accessor : shared_lib_accessor {

    build_accessor(shared_lib_accessor&& shared_accessor)
    : shared_lib_accessor(std::move(shared_accessor)) {}

    std::string_view name() {
        return run<const char*()>("__name");
    }

    void build() {
        run<void()>("build");
    }
};