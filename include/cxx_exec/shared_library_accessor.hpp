#pragma once

#include <libloaderapi.h>
#include <memory>
#include <stdexcept>
#include <type_traits>
#include <utility>
#include <string>
#include <filesystem>

#include <windows.h>
#include <stdio.h>

class shared_library_accessor {
    using path = std::filesystem::path;

    HINSTANCE instance;

public:

    shared_library_accessor(path path) {
        instance = LoadLibrary(path.string().c_str());

        if(!instance) throw std::runtime_error{"couldn't load library '"+path.string()+"'"};
    }

    template<class Function, class... Args>
    std::invoke_result_t<Function, Args...> run(std::string name, Args... args) {
        auto addr = GetProcAddress(instance, name.c_str());

        if(!addr) throw std::runtime_error{"failed to load '"+name+"'"};
        
        return ((Function*)addr)(args...);
    }

    ~shared_library_accessor() noexcept(false) {
        if(!FreeLibrary(std::exchange(instance, nullptr)))
            throw std::runtime_error{"couldn't free library"};
    }
};