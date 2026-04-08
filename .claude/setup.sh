#!/usr/bin/env bash
# SessionStart hook for Claude Code on the web.
# Ensures Clang 21 and basic build tools are available, then configures CMake.
# The actual `cmake --build` is intentionally NOT run here — it would blow past
# the SessionStart hook timeout. Let Claude run the build on demand instead.
set -euo pipefail

LOG_FILE="${CLAUDE_PROJECT_DIR:-$(pwd)}/.claude/setup.log"
mkdir -p "$(dirname "$LOG_FILE")"
exec > >(tee -a "$LOG_FILE") 2>&1
echo "==> $(date -u +%Y-%m-%dT%H:%M:%SZ) setup.sh starting"

# Only run the apt-get install path on Debian/Ubuntu (i.e. the web sandbox).
# On macOS and other hosts, assume the developer has clang-21 installed already.
if [ ! -f /etc/debian_version ]; then
	echo "==> Not a Debian/Ubuntu host, skipping apt install."
	exit 0
fi

NEED_INSTALL=false
if ! command -v clang++-21 &>/dev/null; then NEED_INSTALL=true; fi
if ! command -v clang-format-21 &>/dev/null; then NEED_INSTALL=true; fi

if [ "$NEED_INSTALL" = true ]; then
	echo "==> Installing Clang 21 toolchain..."
	sudo apt-get update -qq
	sudo apt-get install -y -qq wget gnupg lsb-release software-properties-common

	wget -qO- https://apt.llvm.org/llvm-snapshot.gpg.key | sudo tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc >/dev/null
	UBUNTU_CODENAME=$(lsb_release -cs)
	echo "deb http://apt.llvm.org/${UBUNTU_CODENAME}/ llvm-toolchain-${UBUNTU_CODENAME}-21 main" | sudo tee /etc/apt/sources.list.d/llvm-21.list >/dev/null
	sudo apt-get update -qq
	sudo apt-get install -y -qq clang-21 clang++-21 clang-format-21 llvm-21 lld-21

	# Convenience symlinks
	sudo ln -sf /usr/bin/clang-21 /usr/local/bin/clang
	sudo ln -sf /usr/bin/clang++-21 /usr/local/bin/clang++
	sudo ln -sf /usr/bin/clang-format-21 /usr/local/bin/clang-format
	sudo ln -sf /usr/bin/llvm-diff-21 /usr/local/bin/llvm-diff-19
	echo "==> Clang 21 installed successfully."
else
	echo "==> Clang 21 already available."
fi

# Install build tools if missing
for tool in cmake ninja-build ccache; do
	if ! command -v "${tool%%-*}" &>/dev/null; then
		echo "==> Installing ${tool}..."
		sudo apt-get install -y -qq "$tool"
	fi
done

# Install stack trace dependencies if missing
if ! dpkg -s binutils-dev &>/dev/null 2>&1; then
	echo "==> Installing stack trace dependencies..."
	sudo apt-get install -y -qq binutils-dev libdw-dev
fi

# Configure CMake once; skip if a build dir already exists.
if [ ! -f build/build.ninja ]; then
	echo "==> Configuring CMake build..."
	mkdir -p build
	cd build
	cmake -DCMAKE_BUILD_TYPE=Release \
		-DCMAKE_C_COMPILER=clang-21 \
		-DCMAKE_CXX_COMPILER=clang++-21 \
		-G Ninja \
		..
	cd ..
else
	echo "==> build/build.ninja already present, skipping CMake configure."
fi

echo "==> Setup complete. Clang 21 is the default compiler."
clang++-21 --version | head -1
