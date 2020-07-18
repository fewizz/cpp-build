#pragma once

#include <string>

namespace command {

struct command_processor_base {
    virtual void process(std::string command) const = 0;
};

}