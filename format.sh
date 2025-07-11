#!/usr/bin/env bash

set -eo pipefail

cd "$(git rev-parse --show-toplevel)"


if [ "$#" -gt 0 ] && [ "$1" != "-i" ]
then
    cat << EOF
Usage:

  $0     to check formatting
  $0 -i  to fix formatting (if possible)
EOF
    exit 1
fi


if [ -x "$(command -v clang-format-18)" ]
then
    CLANG_FORMAT="clang-format-18"
elif [ -x "$(command -v clang-format)" ] && clang-format --version | grep "version 18" > /dev/null
then
    CLANG_FORMAT="clang-format"
else
    echo could not find clang-format 18 in PATH, please install.
    exit 1
fi


FAIL=0

if [ "${1-}" = "-i" ]
then
    # clang-format
    git ls-files -- '*.cpp' '*.hpp' \
      | grep --invert-match "^third_party" \
      | xargs --max-args=10 --max-procs="$(nproc)" "$CLANG_FORMAT" -i

    # newline at eof
    #
    # list files in repo
    #   ignore third_party code
    #   remove filenames indicating non-text content
    #   last char as decimal ascii is 10 (i.e. is newline) OR append newline
    git ls-files \
      | grep --invert-match "^third_party" \
      | grep --invert-match -e "\.png$" -e "\.zip$" \
      | xargs --max-procs="$(nproc)" -I {} sh -c '[ "$(tail -c 1 {} | od -A n -t d1)" = "   10" ] || echo "" >> {}'

else
    # clang-format
    git ls-files -- '*.cpp' '*.hpp' \
      | grep --invert-match "^third_party" \
      | xargs --max-args=10 --max-procs="$(nproc)" "$CLANG_FORMAT" --dry-run -Werror \
      || FAIL=1

    # newline at eof
    #
    # list files in repo
    #   ignore third_party code
    #   remove filenames indicating non-text content
    #   take last char of the files, count lines and chars,
    #   fail if not equal (i.e. not every char is a newline)
    git ls-files \
      | grep --invert-match "^third_party" \
      | grep --invert-match -e "\.png$" -e "\.zip$" \
      | xargs --max-args=10 --max-procs="$(nproc)" tail -qc 1  | wc -cl \
      | awk '$1 != $2 { print $2-$1, "missing newline(s) at EOF. Please run \"format.sh -i\" to fix"; exit 1 }' \
      || FAIL=1
fi

exit "$FAIL"
