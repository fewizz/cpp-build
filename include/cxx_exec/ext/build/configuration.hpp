#pragma once

#include <functional>
#include <string>
#include <vector>
#include "../../gcc_like_driver_executor.hpp"

using namespace std;

namespace compiler_driver {
    using namespace gcc_like_driver;
    using executor = gcc_like_driver_executor;
}

using compiler = compiler_driver::executor;

struct configuration {
    const string name;
    function<void(compiler& comp)> compiler_options_applier;
};

vector<configuration> configurations = {
    {
        "release",
        [](compiler& comp) {
        
        }
    },
    {
        "debug",
        [](compiler& comp) {
            comp.debug_information_type = compiler_driver::debug_information_type::native;
        }
    }
};

configuration& get_configuration(string name) {
    auto conf = find_if(configurations.begin(), configurations.end(), [=](auto& conf){return conf.name==name;});
    if(conf == configurations.end()) throw runtime_error("can't find configuration with name '"+name+"'");
    return *conf;
}

configuration& release = get_configuration("release");
configuration& debug = get_configuration("debug");