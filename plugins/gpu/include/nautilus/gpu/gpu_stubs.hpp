#pragma once

#include <cstdint>

/// @file gpu_stubs.hpp
/// @brief Internal stub functions for GPU intrinsics.
///
/// These functions serve two purposes:
/// 1. Their addresses are used as identity keys in the MLIR intrinsic plugin
///    registry, allowing the GPU pass to recognize and replace them.
/// 2. They provide CPU fallback behavior when ENABLE_GPU_BACKEND is OFF:
///    threadIdx returns 0, blockDim returns 1, etc.

namespace nautilus::gpu::stubs {

// Thread indexing -- CPU fallback: single thread (index 0)
inline uint32_t gpu_threadIdx_x() {
	return 0;
}
inline uint32_t gpu_threadIdx_y() {
	return 0;
}
inline uint32_t gpu_threadIdx_z() {
	return 0;
}

// Block indexing -- CPU fallback: single block (index 0)
inline uint32_t gpu_blockIdx_x() {
	return 0;
}
inline uint32_t gpu_blockIdx_y() {
	return 0;
}
inline uint32_t gpu_blockIdx_z() {
	return 0;
}

// Block dimensions -- CPU fallback: 1 thread per block
inline uint32_t gpu_blockDim_x() {
	return 1;
}
inline uint32_t gpu_blockDim_y() {
	return 1;
}
inline uint32_t gpu_blockDim_z() {
	return 1;
}

// Grid dimensions -- CPU fallback: 1 block in grid
inline uint32_t gpu_gridDim_x() {
	return 1;
}
inline uint32_t gpu_gridDim_y() {
	return 1;
}
inline uint32_t gpu_gridDim_z() {
	return 1;
}

// Barrier synchronization -- CPU fallback: no-op
inline void gpu_syncThreads() {
}

// Launch config sentinels -- CPU fallback: no-op
inline void gpu_set_grid(uint32_t, uint32_t, uint32_t) {
}
inline void gpu_set_block(uint32_t, uint32_t, uint32_t) {
}

} // namespace nautilus::gpu::stubs
