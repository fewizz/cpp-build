#include <vector>
#include <string>
#include <optional>

using namespace std;

enum output_type {
    executable,
    static_library,
    dynamic_library
};

optional<string> name;
optional<output_type> output_type;
vector<string> sources;

void configure();

int exec(vector<string> arfgs) {
    configure();
}