#!/usr/bin/env bash
# Builds this nautilus checkout the way NebulaStream's vcpkg port does
# (nebulastream/vcpkg/vcpkg-registry/ports/nautilus/portfile.cmake) and swaps it
# in for the nautilus package prebuilt into a nebulastream/nes-ci container.
# Runs inside that container; the swap only touches the container's writable
# layer, never the image.
#
# Usage: build-nautilus.sh <nautilus-src> <nebulastream-src> <build-dir>
set -euo pipefail

NAUTILUS_SRC=$(realpath "$1")
NES_SRC=$(realpath "$2")
BUILD_DIR=$3

ARCH=$(uname -m | sed 's/x86_64/x64/;s/aarch64/arm64/')
TRIPLET="${ARCH}-linux-${VCPKG_SANITIZER}-${VCPKG_STDLIB}"
INSTALLED="${NES_PREBUILT_VCPKG_ROOT}/installed"
PACKAGE_LIST=$(echo "${INSTALLED}"/vcpkg/info/nautilus_*_"${TRIPLET}".list)

# The chainloaded toolchain is the one vcpkg used for every package in the
# image (clang-${LLVM_TOOLCHAIN_VERSION}, libc++, C++23, -fPIC); a differently
# configured nautilus would hide ABI problems or invent new ones. The option
# list mirrors the port's vcpkg_cmake_configure() call, except that every
# backend is built, so NebulaStream's workload can run on each of them
# (NAUTILUS_OPTIONS). The C++ backend compiles its code with the image's `cc`.
cmake -G Ninja -S "${NAUTILUS_SRC}" -B "${BUILD_DIR}" \
	-DCMAKE_BUILD_TYPE=Release \
	-DCMAKE_INSTALL_PREFIX="${INSTALLED}/${TRIPLET}" \
	-DCMAKE_TOOLCHAIN_FILE="${NES_PREBUILT_VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" \
	-DVCPKG_CHAINLOAD_TOOLCHAIN_FILE="${NES_SRC}/vcpkg/custom-triplets/toolchains/${VCPKG_STDLIB}.cmake" \
	-DVCPKG_TARGET_TRIPLET="${TRIPLET}" \
	-DVCPKG_TARGET_ARCHITECTURE="${ARCH}" \
	-DVCPKG_MANIFEST_MODE=OFF \
	-DMLIR_DIR=/clang/lib/cmake/mlir \
	-DENABLE_TESTS=OFF \
	-DENABLE_MLIR_BACKEND=ON \
	-DENABLE_LOGGING=OFF \
	-DENABLE_BC_BACKEND=ON \
	-DENABLE_C_BACKEND=ON \
	-DENABLE_ASMJIT_BACKEND=ON \
	-DENABLE_TBC_BACKEND=ON \
	-DENABLE_TBC_JIT=ON \
	-DENABLE_BUILTIN_PLUGIN=ON \
	-DENABLE_PROFILING_PLUGIN=ON \
	-DENABLE_SIMD_PLUGIN=ON \
	-DENABLE_STD_PLUGIN=ON \
	-DENABLE_SPECIALIZATION_PLUGIN=ON \
	-DENABLE_INLINING_PLUGIN=ON \
	-DENABLE_GPU_PLUGIN=OFF \
	-DENABLE_STACKTRACE=OFF \
	-DUSE_EXTERNAL_MLIR=ON \
	-DUSE_EXTERNAL_SPDLOG=ON \
	-DUSE_EXTERNAL_FMT=ON
cmake --build "${BUILD_DIR}"

# Remove every file vcpkg installed for the prebuilt package (its .list
# manifest), debug variants and the share/nautilus package config included, so
# neither stale headers nor a stale config can shadow the new install.
echo "Removing prebuilt nautilus listed in ${PACKAGE_LIST}"
grep -v '/$' "${PACKAGE_LIST}" | sed "s|^|${INSTALLED}/|" | xargs rm -f
rm -rf "${INSTALLED}/${TRIPLET}/include/nautilus" "${INSTALLED}/${TRIPLET}/share/nautilus" \
	"${INSTALLED}/${TRIPLET}/debug/share/nautilus"

cmake --install "${BUILD_DIR}"
