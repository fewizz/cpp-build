#usage: bash build.sh 'output'
clang++ --std=c++17 -Iinclude -v -o $1 cpp_build.cpp