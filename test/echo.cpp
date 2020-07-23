#include <algorithm>

void exec(vector<string> args) {
    for_each(args.begin(), args.end(), [](string& str){
        cout << str << " ";
    });
}