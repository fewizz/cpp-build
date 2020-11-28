cd $(dirname ${BASH_SOURCE[0]})

echo "uninstall"
rm -vf $1/bin/cxx-exec
rm -rvf $1/include/cxx_exec

echo "compile"
g++ -std=c++20 -Iinclude/cxx_exec -g -o $1/bin/cxx-exec cxx_exec.cpp && { \
echo "copy headers"; \
cp -avr include $1/; }