#!/bin/bash
# This script checks the presence of MPK copyright string in each C or C++
# source or header file, adding one when missing. Author name and email
# are taken from first git commit introducing the file; for untracked files,
# these are taken from git config.
# Copyright years are taken from first and last commits involving the file;
# for untracked files, current year is used.
#
# Copyright (C) 2025 MPK Software, St.-Petersburg, Russia
#
# Author: Stepan Orlov <majorsteve@mail.ru>

set -e

THIS_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
ROOT_DIR="$( cd "$THIS_DIR/.." >/dev/null 2>&1 && pwd )"
THIS_SCRIPT="$THIS_DIR/$(basename "${BASH_SOURCE[0]}")"

cd "$ROOT_DIR"

INPUT_FILE="$1"

COPYRIGHT_TEMPLATE="/** @file
 * @brief TODO: Brief docstring.
 *
 * TODO: More documentation here
 *
 * Copyright (C) COPYRIGHT_YEARS MPK Software, St.-Petersburg, Russia
 *
 * @author AUTHOR_NAME <AUTHOR_EMAIL>
 */
"

check_copyright_comment() {
  local filename="$1"

  awk '
    /\/\*/ {
      in_comment = 1;
      next;
    }
    /\*\// {
      if (in_comment)
      {
        in_comment = 0;
        exit;
      }
    }
    in_comment && /Copyright/ {
      found_copyright = 1;
      exit;
    }
    END {
      if (found_copyright) {
        exit 0;
      } else {
        exit 1;
      }
    }
  ' "$filename"
}

copyright_comment()
{
  local AUTHOR_NAME="$1"
  local AUTHOR_EMAIL="$2"
  local COPYRIGHT_YEARS="$3"
  local RESULT="$COPYRIGHT_TEMPLATE"
  RESULT="${RESULT/AUTHOR_NAME/$AUTHOR_NAME}"
  RESULT="${RESULT/AUTHOR_EMAIL/$AUTHOR_EMAIL}"
  RESULT="${RESULT/COPYRIGHT_YEARS/$COPYRIGHT_YEARS}"
  echo "$RESULT"
}

add_copyright_comment()
{
  echo "Adding copyright comment to file '$INPUT_FILE'"
  GIT_RESULT="$(git ls-files "$INPUT_FILE")"
  local AUTHOR_NAME
  local AUTHOR_EMAIL
  local COPYRIGHT_YEARS
  local FIRST_COMMIT
  local FIRST_YEAR
  local LAST_YEAR
  local COPYRIGHT_YEARS

  if [[ -n "$GIT_RESULT" ]]; then
    FIRST_COMMIT=$(git log --diff-filter=A --follow --format=%H -- "$INPUT_FILE" | tail -n 1)
  fi

  if [[ -n "$FIRST_COMMIT" ]]; then
    # File is tracked
    AUTHOR_NAME=$(git show -s --format='%an' "$FIRST_COMMIT")
    AUTHOR_EMAIL=$(git show -s --format='%ae' "$FIRST_COMMIT")
    FIRST_YEAR=$(git log --diff-filter=A --follow --format='%as' -- "$INPUT_FILE" | tail -n 1 | cut -d '-' -f 1)
    LAST_YEAR=$(git log --follow --format='%as' -- "$INPUT_FILE" | head -n 1 | cut -d '-' -f 1)

    if [[ "$FIRST_YEAR" -ne "$LAST_YEAR" ]]; then
      COPYRIGHT_YEARS="$FIRST_YEAR-$LAST_YEAR"
    else
      COPYRIGHT_YEARS="$LAST_YEAR"
    fi

  else
    # File is untracked
    AUTHOR_NAME="$(git config user.name)"
    AUTHOR_EMAIL="$(git config user.email)"
    COPYRIGHT_YEARS="$(date +%Y)"
  fi

  # Add the comment to the file
  { copyright_comment "$AUTHOR_NAME" "$AUTHOR_EMAIL" "$COPYRIGHT_YEARS";
    cat "$INPUT_FILE"; } > "$INPUT_FILE.tmp" && mv "$INPUT_FILE.tmp" "$INPUT_FILE"
}

if [[ -n "$INPUT_FILE" ]]; then

  if [[ ! -f "$INPUT_FILE" ]]; then
    echo "Error: File '$INPUT_FILE' not found."
    exit 1
  fi

  check_copyright_comment "$INPUT_FILE" || add_copyright_comment "$INPUT_FILE"

else
  SUBDIRS="agc_app agc_perf gc gc_app gc_cli gc_visual"

  find $SUBDIRS \( \
    -name "*.h" -or \
    -name "*.hpp" -or \
    -name "*.c" -or \
    -name "*.cpp" \
    \) -exec "$THIS_SCRIPT" {} \;
fi
