#pragma once

#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <string>
#include <filesystem>

#include <libloaderapi.h>

class shared_library_accessor;

namespace environment {
    shared_library_accessor load_shared_library(const std::filesystem::path&);
} 

class shared_library_accessor {
    using path = std::filesystem::path;

    friend shared_library_accessor environment::load_shared_library(const path&);

    HINSTANCE instance;

    shared_library_accessor(HINSTANCE instance) : instance{instance} { }

public:

    template<class Function, class... Args>
    std::invoke_result_t<Function, Args...> run(std::string name, Args... args) {
        auto addr = GetProcAddress(instance, name.c_str());

        if(!addr) throw std::runtime_error{"run '"+name+"'"};
        
        return ((Function*)addr)(args...);
    }

    ~shared_library_accessor() noexcept(false) {
        if(!FreeLibrary(std::exchange(instance, nullptr)))
            throw std::runtime_error{"free library"};
    }
};