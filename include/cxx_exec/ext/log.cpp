#include <iostream>
#include <functional>
#include <string>

using namespace std;

function<string(const string&)> log_formatter = [](const string& message) { return message; };

inline void info(auto str) {
    cout << log_formatter(str) << "\n" << flush;
}