#pragma once

#include <string>

using namespace std;

constexpr struct binary_type {
    string_view name;

    bool operator==(const binary_type& t){return t.name == name;}
}
executable{"exec"},
static_library{"static"},
dynamic_libraryP{"dynamic"};