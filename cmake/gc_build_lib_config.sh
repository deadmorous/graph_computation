#!/bin/bash

set -e

LIBRARY="$1"
OUTPUT_YAML_FILE="$2"
OUTPUT_CPP_FILE="$3"
CONFIG="$4"
INCLUDE_DIRS="$5"
BINARY_PATH="$6"
COMPILE_OPTIONS="$7"

mkdir -p $(dirname "$OUTPUT_FILE")

echo "BUILDING LIB CONFIG FILE $OUTPUT_FILE"

echo "include_dirs:
$(echo $INCLUDE_DIRS |sed -r "s/;+\$//;s/;+/\n/g" |sed "s/^/  - /")
binary_path: $BINARY_PATH
compile_options: $COMPILE_OPTIONS
" >$OUTPUT_YAML_FILE

format_include_dirs()
{
    pad="                "
    echo "{"

    echo $INCLUDE_DIRS \
        |sed -r "s/;+\$//;s/;+/\n/g" \
        |sed "s/^/${pad}    \"/;s/\$/\",/"
    echo "${pad}}"
}

echo "
#include \"lib_config/lib_config.hpp\"

namespace build {

namespace {

struct LibConfigRegistrar
{
    LibConfigRegistrar()
    {
        register_lib_config(
            \"$LIBRARY\",
            {
                .include_dirs = $(format_include_dirs),
                .binary_path = \"$BINARY_PATH\",
                .compile_options = \"$COMPILE_OPTIONS\"
            }
        );
    }
};

__attribute__((used))
LibConfigRegistrar lib_config_registrar;

} // anonymous namespace

} // namespace build
" >$OUTPUT_CPP_FILE
