#pragma once

#include <functional>
#include <string>
#include <vector>
#include "../../gcc_like_driver.hpp"

using namespace std;

using compiler = gcc_like_driver::command_builder;

struct configuration {
    const string name;
    function<void(compiler&)> applier;

    bool operator==(const configuration& other) const {
        return this == &other;
    }

    void apply(compiler& cc) { applier(cc); }
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
            comp.debug(gcc_like_driver::native);
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