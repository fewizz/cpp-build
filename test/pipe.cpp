#include "../include/cxx_exec/unix/ipstream.hpp"
#include <string>
#include <vector>
#include <assert.h>

using namespace std;

void exec(vector<string> args) {
    assert(args.size()>0);
    cout<<"arg: "<<args[0]<<"\n";
    unix::basic_ipstream<char> s{args[0]};

    char ch;

    while( (ch=s.get()) != EOF )
        cout << ch;
}