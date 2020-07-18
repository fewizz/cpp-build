#pragma once

#include <string>
#include <functional>
#include "gcc_like_driver_executor.hpp"

namespace build {

struct configuration {
    std::string name;
    std::function<void(gcc_like_driver::gcc_like_driver_executor& comp)> compiler_options_applier;
};

configuration release {
    "release",
    [](gcc_like_driver::gcc_like_driver_executor& comp) {
        
    }
};

configuration debug {
    "debug",
    [](gcc_like_driver::gcc_like_driver_executor& comp) {
        comp.debug_information_type = gcc_like_driver::native;
    }
};

}