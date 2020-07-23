#include <vector>
#include <string>

using namespace std;

void exec(vector<string> args);

int main(int argc,char** argv) {
    exec({argv+1, argv+argc});
    return EXIT_SUCCESS;
}