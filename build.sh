#usage: bash build.sh 'output'
echo "compiling..."
clang++ -std=c++20 -Iinclude -o $1 cxx_exec.cpp