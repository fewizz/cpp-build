#usage: bash build.sh 'output'
echo "compiling cpp_build.cpp..."
clang++ -std=c++17 -Iinclude -o $1 cpp_build.cpp