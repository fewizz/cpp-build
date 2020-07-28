#pragma once

#include <optional>
#include <string>
#include <vector>

using namespace std;

#include "configuration.hpp"

struct builder {
    const string name;

    builder(string name):name{name}{}

    vector<reference_wrapper<configuration>> configurations;
    vector<reference_wrapper<builder>> libraries;

    virtual void build() {
        
    }

    builder(string name):name{name}{}
};