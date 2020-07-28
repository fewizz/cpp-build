#include <vector>
#include <string>
#include <iostream>

using namespace std;

void exec(vector<string> args) {
    for(auto& arg : args)
        cout << arg << " ";
}