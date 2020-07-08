echo "compiling..."
clang++ -std=c++20 -Iinclude -v -o $1/bin/cxx-exec cxx_exec.cpp