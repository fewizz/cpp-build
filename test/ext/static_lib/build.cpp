#include "ext/static_lib.hpp"

static_lib configure(gnu::clap& clap) {
    return {
        "code",
        {"include"},
        {"code.cpp"}
    };
}