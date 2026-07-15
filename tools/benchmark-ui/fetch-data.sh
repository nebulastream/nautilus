#!/usr/bin/env bash
# Fetch the published benchmark data next to index.html for local development:
#   ./fetch-data.sh && python3 -m http.server 8000
set -euo pipefail
cd "$(dirname "$0")"

if git show origin/pages:dev/bench/data.js > data.js 2>/dev/null; then
	echo "data.js written from origin/pages:dev/bench/data.js"
elif git fetch origin pages 2>/dev/null && git show origin/pages:dev/bench/data.js > data.js 2>/dev/null; then
	echo "data.js written from origin/pages:dev/bench/data.js (after fetch)"
else
	curl -fsSL https://nebulastream.github.io/nautilus/dev/bench/data.js -o data.js
	echo "data.js downloaded from the live dashboard"
fi
