#!/bin/bash

OUTPUT_FILE="$1"
CONFIG="$2"
GCLIB_INCLUDES="$3"
COMPILE_OPTIONS="$4"

mkdir -p $(dirname "$OUTPUT_FILE")

echo "#include \"build/config_vars.hpp\"

namespace build {

const ConfigType current_config_type =
    ConfigType::${CONFIG};

const std::string_view gclib_include_dirs =
    \"${GCLIB_INCLUDES}\";

const std::string_view gclib_compile_options =
    \"${COMPILE_OPTIONS}\";

} // namespace build
" >$OUTPUT_FILE
