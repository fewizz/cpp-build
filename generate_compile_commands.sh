native_absolute_path() {
    if [[ "$OSTYPE" == "msys" ]]; then
        cygpath -ma $1
    else
        realpath $1
    fi
}

printf "[\n\
    {\n\
        \"directory\": \"$(native_absolute_path $(dirname ${BASH_SOURCE[0]}))\",\n\
        \"command\": \"$(native_absolute_path $(which clang++)) -std=c++20 -Iinclude/cxx_exec cxx_exec.cpp\",\n\
        \"file\": \"cxx_exec.cpp\"\n\
    }\n\
]"\
> compile_commands.json