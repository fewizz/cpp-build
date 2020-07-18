#pragma once

namespace command {

struct command_executor_base {
    virtual void execute() const = 0;
};

}