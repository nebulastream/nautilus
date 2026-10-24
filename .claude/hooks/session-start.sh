#!/bin/bash
# Prepares a Claude Code cloud session for building and testing Nautilus:
# GCC 14 and Clang 21 (the CI compilers), ninja, ccache, valgrind (for the
# perf-investigation skill) and clang-format 22
# (the version the CI format check uses). CC/CXX point to Clang 21 through
# .claude/settings.json. Idempotent; local sessions are left alone. See
# AGENTS.md for the build and test commands.
set -euo pipefail

if [ "${CLAUDE_CODE_REMOTE:-}" != "true" ]; then
	exit 0
fi

log() { echo "[nautilus session-start] $*" >&2; }

SUDO=""
if [ "$(id -u)" -ne 0 ]; then
	SUDO="sudo"
fi

apt_install() {
	$SUDO apt-get update -qq
	DEBIAN_FRONTEND=noninteractive $SUDO apt-get install -y -qq "$@" >/dev/null
}

missing=()
command -v g++-14 >/dev/null || missing+=(g++-14)
command -v ninja >/dev/null || missing+=(ninja-build)
command -v ccache >/dev/null || missing+=(ccache)
command -v cmake >/dev/null || missing+=(cmake)
command -v valgrind >/dev/null || missing+=(valgrind)
if [ "${#missing[@]}" -gt 0 ]; then
	log "installing ${missing[*]}"
	apt_install "${missing[@]}"
fi

# Clang 21, the project's default compiler, from apt.llvm.org.
if ! command -v clang++-21 >/dev/null; then
	log "installing Clang 21"
	codename="$(. /etc/os-release && echo "${VERSION_CODENAME}")"
	curl -fsSL https://apt.llvm.org/llvm-snapshot.gpg.key | $SUDO tee /etc/apt/trusted.gpg.d/apt.llvm.org.asc >/dev/null
	echo "deb https://apt.llvm.org/${codename}/ llvm-toolchain-${codename}-21 main" |
		$SUDO tee /etc/apt/sources.list.d/llvm-21.list >/dev/null
	# libclang-rt provides the sanitizer runtimes for the asan and tsan presets.
	apt_install clang-21 libclang-rt-21-dev
fi

# clang-format 22 from PyPI, exposed as clang-format-22 so format.sh prefers it
# over an older system clang-format.
TOOLS="${HOME}/.cache/nautilus-tools"
if [ ! -x "${TOOLS}/bin/clang-format-22" ]; then
	log "installing clang-format 22"
	python3 -m venv "${TOOLS}/venv"
	PIP_DISABLE_PIP_VERSION_CHECK=1 "${TOOLS}/venv/bin/pip" install -q "clang-format==22.1.8"
	mkdir -p "${TOOLS}/bin"
	ln -sf "${TOOLS}/venv/bin/clang-format" "${TOOLS}/bin/clang-format-22"
fi

if [ -n "${CLAUDE_ENV_FILE:-}" ]; then
	{
		echo "export PATH=\"${TOOLS}/bin:\$PATH\""
	} >>"${CLAUDE_ENV_FILE}"
fi

log "ready: $(clang++-21 --version | head -1); $("${TOOLS}/bin/clang-format-22" --version)"
