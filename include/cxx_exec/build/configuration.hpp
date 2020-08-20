#pragma once

#include <functional>
#include <string>
#include <vector>
#include <stdexcept>
#include "../gcc_like_driver.hpp"

struct configuration {
    const std::string name;
    const std::function<void(gcc_like_driver::command_builder&)> applier;

    bool operator==(const configuration& other) const {
        return this == &other;
    }

    void apply(gcc_like_driver::command_builder& cc) const { applier(cc); }
	
	static std::vector<configuration> configurations;
	
	static inline configuration& by_name(std::string_view name) {
		auto conf = find_if(configurations.begin(), configurations.end(), [=](auto& conf){return conf.name==name;});
		if(conf == configurations.end()) throw std::runtime_error("can't find configuration with name '"+std::string{name}+"'");
		return *conf;
	}
	
	static configuration& release;
	static configuration& debug;
};

std::vector<configuration> configuration::configurations = {
    { "release", [](gcc_like_driver::command_builder& comp) { } },
    { "debug", [](gcc_like_driver::command_builder& comp) { comp.debug(gcc_like_driver::native); } }
};

configuration& configuration::release = configuration::by_name("release");
configuration& configuration::debug = configuration::by_name("debug");