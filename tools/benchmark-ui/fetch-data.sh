#!/usr/bin/env bash
# Fetch the published benchmark data next to index.html for local development:
#   ./fetch-data.sh && python3 -m http.server 8000
# data.js is nautilus' tracing benchmark (dev/bench), nebulastream-data.js the
# NebulaStream benchmark (dev/nebulastream); the page shows whichever exists.
set -euo pipefail
cd "$(dirname "$0")"

fetch() { # <path on the pages branch> <local file>
	if git show "origin/pages:$1" > "$2" 2>/dev/null; then
		echo "$2 written from origin/pages:$1"
	elif git fetch origin pages 2>/dev/null && git show "origin/pages:$1" > "$2" 2>/dev/null; then
		echo "$2 written from origin/pages:$1 (after fetch)"
	elif curl -fsSL "https://nebulastream.github.io/nautilus/$1" -o "$2"; then
		echo "$2 downloaded from the live dashboard"
	else
		rm -f "$2"
		echo "$1 is not published yet"
	fi
}

fetch dev/bench/data.js data.js
fetch dev/nebulastream/data.js nebulastream-data.js
