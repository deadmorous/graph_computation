#!/bin/bash

OUTPUT_FILE="$1"
CONFIG="$2"

mkdir -p $(dirname "$OUTPUT_FILE")

echo "#include \"build/config_vars.hpp\"

namespace build {

const ConfigType current_config_type =
    ConfigType::${CONFIG};

} // namespace build
" >$OUTPUT_FILE
