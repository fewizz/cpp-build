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
    char* prev_read = 0;

    basic_pipebuf(FILE* f)
    :
    file{f}
    {
        this->setg(0, 0, 0);
    }

    ~basic_pipebuf() override {
        pclose(file);
    }

    int_type uflow() override {
        auto diff = this->gptr() - prev_read;
        if(diff) {
            fseek(file, diff, SEEK_CUR);
            prev_read+=diff;
        }
        char ch = fgetc(file);
        if(ch != EOF) { 
            this->setg(this->eback(), this->gptr()+1, this->egptr()+1);
            prev_read++;
        }
        return ch;
    }

    int_type underflow() override {
        auto diff = this->gptr() - prev_read;
        if(diff) {
            fseek(file, diff, SEEK_CUR);
            prev_read+=diff;
        }
        char ch = fgetc(file);
        ungetc(ch, file);
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