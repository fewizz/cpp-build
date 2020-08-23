#include "../include/cxx_exec/unix/ipstream.hpp"
#include <assert.h>
#include <string>
#include <vector>

using namespace std;

void exec(vector<string> args) {
    assert(args.size()>0);

    cout << "using get():\n";
    unix::ipstream s0{args[0]};

    char ch;
    while( (ch=s0.get()) != EOF )
        cout << ch;

    cout << "using << operator:\n";
    unix::ipstream s1{args[0]};
    while(s1) {
		std::string str;
		s1 >> str;
		cout << str << "\n";
	}
}
