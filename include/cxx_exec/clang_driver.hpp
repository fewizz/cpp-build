#pragma once

#include "gcc_driver.hpp"

namespace clang {
    using namespace gcc;

    struct default_cxx_driver_executor : driver_executor {
        default_cxx_driver_executor(lang_std::t _std)
            :driver_executor("clang++", _std){}
    };

    struct default_cxx20_driver_executor : default_cxx_driver_executor {
        default_cxx20_driver_executor()
            :default_cxx_driver_executor(lang_std::cxx20){}
    };
}