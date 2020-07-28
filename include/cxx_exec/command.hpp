#pragma once

#include <string>
#include <filesystem>
#include <vector>

namespace cmd {

class command {
    std::string m_command;
public:

    command(std::string command) 
    :
    m_command{command}
    {}

    command(std::string command, std::filesystem::path arg) 
    :
    m_command{command+" "+arg.string()}
    {}

    template<class ArgsIter>
    command(std::string p, ArgsIter begin, ArgsIter end) {
        std::string result;
        result+=p;
        for_each(begin, end, [&](auto& arg) {
            result+=" "+std::string{arg};
        });
        m_command = result;
    }

    template<class ArgsIter>
    command(const char* p, ArgsIter begin, ArgsIter end)
    :
    command(std::string{p}, begin, end)
    {}

    template<class ArgsIter>
    command(std::filesystem::path p, ArgsIter begin, ArgsIter end)
    :
    command(p.string(), begin, end)
    {}

    std::string string() {
        return m_command;
    }
};

/*struct builder {
    virtual command build() = 0;
    operator command() { return build(); }
};*/

}