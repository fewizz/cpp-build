#include <vector>
#include <string>
#include <optional>
#include <filesystem>
#include <map>

#include "../../environment.hpp"
#include "clap/gnu_clap.hpp"

#include "project.hpp"

using namespace std;
using namespace std::filesystem;

using configurator = function<void(builder&)>;
map<builder, configurator> builder_to_configurator;

builder& make_buidler(string name, configurator conf) {
    auto& [iter, inserted] = builder_to_configurator.emplace({{name}, conf});
    return iter->first;
}

void exec(vector<string> args) {
    //bool info_name;
    //project_builder builder;

    gnu::clap clap;
    //clap.flag("info-name", info_name);
    string conf;
    clap.option("configuration", [&](string_view arg) {
        conf = arg;
    });
    clap.parse(args.begin(), args.end());

    for(auto conf : configurators) {
        conf(args);
    }

    /*configure();

    auto error_if_null = [&](auto& o, string name){
        if(!o) throw runtime_error(name+" not specified");
    };

    error_if_null(build.build_configuration, "build configuration");
    error_if_null(build.name, "name");
    error_if_null(output_type, "output_type");

    if(info_name)
        cout << *build.name;

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
        path build = "build/"+build_configuration->get().name+"_"+string{output_type->name};
        path output = build/"objects"/path{source}.replace_extension(".o");
        create_directories(path{output}.remove_filename());
        comp.output = output;
        comp.execute();

        if(*output_type == static_library)
            environment::execute("ar -rcv "+build.string()+"/"+"lib"+*name+".a "+output.string());
    }*/
}