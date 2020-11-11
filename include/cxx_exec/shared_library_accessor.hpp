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
    std::filesystem::path shared_library_location(const shared_library_accessor& lib);
}

class shared_library_accessor {
protected:
    friend shared_library_accessor environment::load_shared_library(const std::filesystem::path&);
    friend std::filesystem::path environment::shared_library_location(const shared_library_accessor& lib);

    HINSTANCE instance;

    shared_library_accessor(HINSTANCE instance) : instance{instance} { }

public:

    shared_library_accessor(shared_library_accessor&& r) : instance{ r.instance }
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

    ~shared_library_accessor() noexcept(false) {
        if(!instance) return;
        if(!FreeLibrary(std::exchange(instance, nullptr)))
            throw std::runtime_error{"free library"};
    }
};