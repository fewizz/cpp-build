#pragma once

#include <ios>
#include <string>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include "../command.hpp"

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

    int_type underflow() override {
        _advance();
        char ch = fgetc(file);
        ungetc(ch, file);
        return ch;
    }

protected:
    int_type uflow() override {
        _advance();
        char ch = fgetc(file);
        if(ch != EOF) { 
            this->setg(this->eback(), this->gptr()+1, this->gptr()+1);
            prev_read++;
        }
        return ch;
    }

    std::streamsize xsgetn(CharT* s, std::streamsize count) override {
        _advance();
        auto r = fread(s, sizeof(CharT), count, file);
        prev_read+=r;
        this->setg(this->eback(), this->gptr()+r, this->gptr()+r);
        return r;
    }

    void _advance() {
        if(auto diff = this->gptr() - prev_read) {
            fseek(file, diff, SEEK_CUR);
            prev_read+=diff;
        }
    }
};

template<class CharT, class Traits = std::char_traits<CharT>>
struct basic_ipstream : std::basic_istream<CharT, Traits> {
    unix::basic_pipebuf<CharT> filebuf;

    basic_ipstream(cmd::command command)
    :
    filebuf{popen(command.string().c_str(), "r")}
    {
        std::basic_ios<char>::init(&filebuf);
    }
};

using ipstream = basic_ipstream<char>;

}