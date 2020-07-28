#pragma once

#include <string>
#include "command.hpp"

namespace cmd {

struct processor {
    virtual void process(command c) = 0;
    void execute(command c) { process(c); }
};

}