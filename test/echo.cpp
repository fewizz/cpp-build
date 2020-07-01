#include <iostream>
#include <algorithm>

using namespace std;

void exec(std::vector<std::string> args) {
    for_each(args.begin(), args.end(), [](string& str){
        cout << str << " ";
    });
}