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


CLANG_FORMAT=""

# Try versioned binaries from highest to lowest
for version in 22 21 20 19 18 17 16; do
    if [ -x "$(command -v clang-format-${version})" ]; then
        CLANG_FORMAT="clang-format-${version}"
        break
    fi
done

# Fall back to unversioned clang-format if available
if [ -z "$CLANG_FORMAT" ] && [ -x "$(command -v clang-format)" ]; then
    CLANG_FORMAT="clang-format"
fi

if [ -z "$CLANG_FORMAT" ]; then
    echo "could not find clang-format in PATH, please install."
    exit 1
fi

echo "Using: $($CLANG_FORMAT --version)"

# Cross-platform CPU count
if command -v nproc > /dev/null 2>&1; then
    NPROC="$(nproc)"
else
    NPROC="$(sysctl -n hw.logicalcpu 2>/dev/null || echo 1)"
fi

FAIL=0

if [ "${1-}" = "-i" ]
then
    # clang-format
    git ls-files -- '*.cpp' '*.hpp' \
      | grep --invert-match "^third_party" \
      | xargs --max-args=10 --max-procs="$NPROC" "$CLANG_FORMAT" -i

    # newline at eof
    #
    # list files in repo
    #   ignore third_party code
    #   remove filenames indicating non-text content
    #   last char as decimal ascii is 10 (i.e. is newline) OR append newline
    git ls-files \
      | grep --invert-match "^third_party" \
      | grep --invert-match -e "\.png$" -e "\.zip$" \
      | xargs --max-procs="$NPROC" -I {} sh -c '[ "$(tail -c 1 {} | wc -l | tr -d "[:space:]")" = "1" ] || printf "\n" >> {}'

else
    # clang-format
    git ls-files -- '*.cpp' '*.hpp' \
      | grep --invert-match "^third_party" \
      | xargs --max-args=10 --max-procs="$NPROC" "$CLANG_FORMAT" --dry-run -Werror \
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
      | xargs --max-args=10 --max-procs="$NPROC" tail -qc 1  | wc -cl \
      | awk '$1 != $2 { print $2-$1, "missing newline(s) at EOF. Please run \"format.sh -i\" to fix"; exit 1 }' \
      || FAIL=1
fi

exit "$FAIL"
