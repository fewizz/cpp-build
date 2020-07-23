#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include "cxx_exec/environment.hpp"
#include "clap/gnu_clap.hpp"
#include "cxx_exec/environment.hpp"

using namespace std;
using namespace std::filesystem;

// name
optional<string> name;

// configuration
namespace compiler_driver {
    using namespace gcc_like_driver;
    using executor = gcc_like_driver_executor;
}

using compiler = compiler_driver::executor;

struct configuration {
    const string name;
    function<void(compiler& comp)> compiler_options_applier;
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
            comp.debug_information_type = compiler_driver::debug_information_type::native;
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

optional<reference_wrapper<configuration>> build_configuration;

struct binary_type {
}
executable,
static_library,
dynamic_library;

struct dependency {
    virtual void resolve() = 0;
    virtual path library_path() = 0;
    virtual ~dependency() {}
};

struct cxx_build : dependency {
    path p;

    cxx_build(path p):p{p}{}

    void resolve() override {
        string dir = p.remove_filename().string();
        string file = p.filename().string();
        environment::execute("cd "+dir+" && cxx-exec "+file);
    }

    path library_path() override {
        path dir = p.remove_filename();
        path lib = dir/"build"/build_configuration->get().name/("lib"+*name);
        return lib;
    }
};

vector<dependency> dependencies;
optional<binary_type> output_type;

struct source_set : vector<path> {
    void include_file(path p) {
        push_back(p);
    }

    void include_directory_recursive(path p, string extension = ".cpp") {
        for(auto& it : recursive_directory_iterator{p}) {
            if(!it.is_regular_file())
                continue;

            if(it.path().extension() == extension)
                push_back(it);
        }
    }
} sources;

void configure();

void exec(vector<string> args) {
    bool info_name;
    gnu::clap clap;
    clap.flag("info-name", info_name);
    clap.option("configuration", [&](string_view arg) {
        build_configuration = get_configuration(string{arg});
    });
    clap.parse(args.begin(), args.end());

    configure();

    auto error_if_null = [&](auto& o, string name){
        if(!o) throw runtime_error(name+" not specified");
    };

    error_if_null(build_configuration, "build configuration");
    error_if_null(name, "name");
    error_if_null(output_type, "output_type");

    if(info_name)
        cout << *name;

    for(auto& dep : dependencies) {
        dep.resolve();
    }

    auto comp = environment::cxx_compiler();
    comp.include_path("./include");

    build_configuration->get().compiler_options_applier(comp);
    for(auto& source : sources) {
        comp.input_files.clear();
        comp.input_file(source);
        comp.output_type = compiler_driver::output_type::object_file;
        path output =
            ("build/object/"+build_configuration->get().name)/source.replace_extension(".o");
        create_directories(path{output}.remove_filename());
        comp.output = output;
        comp.execute();
    }    
}