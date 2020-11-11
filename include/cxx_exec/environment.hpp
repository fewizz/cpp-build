#pragma once

#include <filesystem>
#include "command_processor.hpp"
#include "gcc_like_driver.hpp"
#include <cstdlib>
#include "unix/ipstream.hpp"
#include "shared_library_accessor.hpp"

namespace environment {

#ifdef _WIN32
#include <libloaderapi.h>
#include <errhandlingapi.h>
#include <winerror.h>
namespace windows {
    inline std::filesystem::path module_file_name(void* module) {
        std::string path_s;

	    int w;
	    do {
		    path_s.resize(path_s.size()+0x100);
    	    w = GetModuleFileNameA(nullptr, path_s.data(), path_s.size());
	    } while(GetLastError() == ERROR_INSUFFICIENT_BUFFER);

        path_s.resize(w);
	    return path_s;
    }
}
#endif

inline void process(cmd::command command) {
    if(int code = std::system(command.string().c_str()))
    throw std::runtime_error {
        "'"+command.string()+"' command's exit code is '"
        +std::to_string(code)+"'"
    };
}

inline std::string variable(std::string name, std::string def={}) {
    auto val = std::getenv(name.c_str());
    return val not_eq nullptr ? std::string{val} : def;
}

inline std::string cxx_compiler() {
    return variable("CXX", "g++");
}

inline gcc_like_driver::command_builder cxx_compile_command_builder() {
    return {cxx_compiler()};
}

inline void execute(cmd::command c) {
    process(c);
}

#include <io.h>

inline void change_dir(std::filesystem::path dir) {
    if(chdir(dir.string().c_str()) == -1)
        throw std::runtime_error {
            "change directory to '"+dir.string()+"'"
        };
}

inline void change_dir(std::filesystem::path dir, std::function<void()> fun) {
    char prev_[256];
    auto current = getcwd(prev_, 256);

    change_dir(dir);

    fun();

    change_dir({prev_});
}

inline shared_library_accessor load_shared_library(const std::filesystem::path& path) {
    auto instance = LoadLibrary(path.string().c_str());
    if(!instance) throw std::runtime_error{"load library '"+path.string()+"'"};
    return {instance};
}

inline std::filesystem::path shared_library_location(const shared_library_accessor& lib) {
    return windows::module_file_name(lib.instance);
}

const inline std::string exec_extension =
#ifdef _WIN32
".exe"
#else
""
#endif
;

const inline std::string dynamic_lib_extension =
#ifdef _WIN32
".dll"
#else
".so"
#endif
;

}