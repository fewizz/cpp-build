echo "compiling..."
clang++ -std=c++20 -Iinclude -v -fuse-ld=lld -o $1/bin/cxx-exec cxx_exec.cpp