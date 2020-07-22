#pragma once

#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>

namespace unix {

template<class CharT, class Traits = std::char_traits<CharT>>
struct basic_pipebuf : std::basic_streambuf<CharT, Traits> {
    using typename std::basic_streambuf<CharT, Traits>::int_type;

    FILE* file;

    basic_pipebuf(FILE* f)
    :
    file{f}
    {}

    ~basic_pipebuf() override {
        pclose(file);
    }

    int_type uflow() override {
        return underflow();
    }

    int_type underflow() override {
        char ch = fgetc(file);
        if(ch != EOF) this->setg(this->eback(), this->gptr()+1, this->egptr()+1);
        return ch;
    }
};

template<class CharT, class Traits = std::char_traits<CharT>>
struct basic_ipstream : std::basic_istream<CharT, Traits> {
    unix::basic_pipebuf<CharT> filebuf;

    basic_ipstream(std::string command)
    :
    filebuf{popen(command.c_str(), "r")}
    {
        std::basic_ios<char>::init(&filebuf);
    }
};

using ipstream = basic_ipstream<char>;

}