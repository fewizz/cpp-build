#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <string>
#include <filesystem>

#include <libloaderapi.h>

class shared_lib_accessor;

namespace environment {
    shared_lib_accessor load_shared_library(const std::filesystem::path&);
}

class shared_lib_accessor {
protected:
    friend shared_lib_accessor environment::load_shared_library(const std::filesystem::path&);

    HINSTANCE instance;

    shared_lib_accessor(HINSTANCE instance) : instance{instance} { }

public:

    shared_lib_accessor(shared_lib_accessor&& r) : instance{ r.instance }
    {
        r.instance = nullptr;
    }

    template<class Function, class... Args>
    std::invoke_result_t<Function, Args...> run(std::string name, Args... args) {
        auto addr = GetProcAddress(instance, name.c_str());

        if(!addr) throw std::runtime_error{"run '"+name+"'"};
        
        return ((Function*)addr)(args...);
    }

    bool has(std::string name) {
        return GetProcAddress(instance, name.c_str());
    }

    ~shared_lib_accessor() noexcept(false) {
        if(!instance) return;
        if(!FreeLibrary(std::exchange(instance, nullptr)))
            throw std::runtime_error{"free library"};
    }
};