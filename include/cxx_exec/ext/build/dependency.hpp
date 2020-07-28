#pragma once

#include <string>
#include <filesystem>
#include "../../environment.hpp"
#include "binary.hpp"

using namespace std;
using namespace std::filesystem;

struct dependency {
    dependency(){}

    dependency(const dependency&)=delete;
    dependency& operator=(const dependency&)=delete;
    dependency(dependency&&)=default;
    dependency& operator=(dependency&&)=default;
    virtual ~dependency() = default;
};

struct binary_dependency : dependency {
    virtual path binary_path() = 0;
};