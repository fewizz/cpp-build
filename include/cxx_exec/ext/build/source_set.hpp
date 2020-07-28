#pragma once

#include <vector>
#include <filesystem>

using namespace std;
using namespace filesystem;

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
};