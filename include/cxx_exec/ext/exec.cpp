#include <vector>
#include <string>

using namespace std;

extern "C" void exec(vector<string> params);

int main(int argc, char* argv[]) {
    exec({argv, argv + argc});
}