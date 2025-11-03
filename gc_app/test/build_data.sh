#!/bin/bash

set -eu

if [[ "$#" != "1" ]]; then
    echo Usage: $0 target_dir >&2
    exit 1
fi

SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

TARGET_DIR="$1"

if [[ ! -d "$TARGET_DIR" ]]; then
    echo "Target directory '$TARGET_DIR' does not exist" >&2
    exit 1
fi

SRC_DIR="$SCRIPT_DIR/data"
DST_DIR="$TARGET_DIR/data"
echo "Building gc-app test data in directory '$DST_DIR'"
mkdir -p "$DST_DIR"
cp "$SRC_DIR/acorn.png" "$DST_DIR/"
