#pragma once

#include <functional>
#include <initializer_list>
#include <string>
#include <vector>
#include <stdexcept>
#include <map>
#include "gcc_like_driver.hpp"

struct configuration {
    const std::function<void(gcc_like_driver::command_builder&)> applier;

    bool operator==(const configuration& other) const {
        return this == &other;
    }

    std::string_view name() {
        for(const auto& c : configurations)
            if(c.second == *this)
                return c.first;
        throw std::runtime_error("configuration isn't registered");
    }

    void apply(gcc_like_driver::command_builder& cc) const { applier(cc); }
	
	static std::map<std::string, configuration>  configurations;
	
	static inline configuration& by_name(const std::string& name) {
        auto conf_iter = configurations.find(name);
		if(conf_iter == configurations.end())
            throw std::runtime_error("can't find configuration with name '"+std::string{name}+"'");
		return conf_iter->second;
	}
	
	static configuration& release;
	static configuration& debug;
};

std::map<std::string, configuration> configuration::configurations = {
    {
        "release",
        { [](gcc_like_driver::command_builder& comp) { } }
    },
    {
        "debug",
        { [](gcc_like_driver::command_builder& comp) { comp.debug(gcc_like_driver::native); }  }
    }
};

configuration& configuration::release = configuration::by_name("release");
configuration& configuration::debug = configuration::by_name("debug");