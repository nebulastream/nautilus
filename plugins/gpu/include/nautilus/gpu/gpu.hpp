#pragma once

#include "nautilus/function.hpp"
#include "nautilus/gpu/gpu_stubs.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <cstddef>
#include <cstdint>

namespace nautilus::gpu {

// ============================================================================
// Thread indexing intrinsics
// ============================================================================

/// Returns the thread index within the block for dimension x/y/z.
/// CPU fallback: returns 0 (single thread).
inline val<uint32_t> threadIdx_x() {
	return invoke<uint32_t>(&stubs::gpu_threadIdx_x);
}
inline val<uint32_t> threadIdx_y() {
	return invoke<uint32_t>(&stubs::gpu_threadIdx_y);
}
inline val<uint32_t> threadIdx_z() {
	return invoke<uint32_t>(&stubs::gpu_threadIdx_z);
}

/// Returns the block index within the grid for dimension x/y/z.
/// CPU fallback: returns 0 (single block).
inline val<uint32_t> blockIdx_x() {
	return invoke<uint32_t>(&stubs::gpu_blockIdx_x);
}
inline val<uint32_t> blockIdx_y() {
	return invoke<uint32_t>(&stubs::gpu_blockIdx_y);
}
inline val<uint32_t> blockIdx_z() {
	return invoke<uint32_t>(&stubs::gpu_blockIdx_z);
}

/// Returns the block dimension (threads per block) for dimension x/y/z.
/// CPU fallback: returns 1.
inline val<uint32_t> blockDim_x() {
	return invoke<uint32_t>(&stubs::gpu_blockDim_x);
}
inline val<uint32_t> blockDim_y() {
	return invoke<uint32_t>(&stubs::gpu_blockDim_y);
}
inline val<uint32_t> blockDim_z() {
	return invoke<uint32_t>(&stubs::gpu_blockDim_z);
}

/// Returns the grid dimension (number of blocks) for dimension x/y/z.
/// CPU fallback: returns 1.
inline val<uint32_t> gridDim_x() {
	return invoke<uint32_t>(&stubs::gpu_gridDim_x);
}
inline val<uint32_t> gridDim_y() {
	return invoke<uint32_t>(&stubs::gpu_gridDim_y);
}
inline val<uint32_t> gridDim_z() {
	return invoke<uint32_t>(&stubs::gpu_gridDim_z);
}

/// Block-level barrier synchronization.
/// CPU fallback: no-op.
inline void syncThreads() {
	invoke<void>(&stubs::gpu_syncThreads);
}

// ============================================================================
// Kernel launch
// ============================================================================

/// Grid dimensions for kernel launch (number of thread blocks).
struct GridDim {
	val<uint32_t> x;
	val<uint32_t> y;
	val<uint32_t> z;

	/// 1D grid.
	GridDim(val<uint32_t> x) : x(x), y((uint32_t) 1), z((uint32_t) 1) {
	}

	/// 3D grid.
	GridDim(val<uint32_t> x, val<uint32_t> y, val<uint32_t> z) : x(x), y(y), z(z) {
	}
};

/// Block dimensions for kernel launch (threads per block).
struct BlockDim {
	val<uint32_t> x;
	val<uint32_t> y;
	val<uint32_t> z;

	/// 1D block.
	BlockDim(val<uint32_t> x) : x(x), y((uint32_t) 1), z((uint32_t) 1) {
	}

	/// 3D block.
	BlockDim(val<uint32_t> x, val<uint32_t> y, val<uint32_t> z) : x(x), y(y), z(z) {
	}
};

// Internal: traced launch config intrinsics.
inline void setGrid(val<uint32_t> x, val<uint32_t> y, val<uint32_t> z) {
	invoke<void, uint32_t, uint32_t, uint32_t>(&stubs::gpu_set_grid, x, y, z);
}
inline void setBlock(val<uint32_t> x, val<uint32_t> y, val<uint32_t> z) {
	invoke<void, uint32_t, uint32_t, uint32_t>(&stubs::gpu_set_block, x, y, z);
}

/// Launch a NautilusFunction as a GPU kernel with explicit grid/block dimensions.
///
/// The grid and block dimensions are traced values, so they can be computed
/// dynamically from data (e.g. `GridDim{(n + 255) / 256}`).
///
/// During tracing, this emits:
///   1. setGrid(grid.x, grid.y, grid.z)   -> backend stores as launch config
///   2. setBlock(block.x, block.y, block.z) -> backend stores as launch config
///   3. kernel(args...)                     -> backend emits kernel launch with stored config
///
/// CUDA backend generates PTX kernel + cuLaunchKernel host call.
/// Metal backend generates MSL kernel + Metal dispatch.
///
/// **CPU fallback:** setGrid/setBlock are no-ops, kernel runs once (threadIdx=0).
///
/// @code
/// static auto myKernel = NautilusFunction{"vecAdd",
///     [](val<float*> a, val<float*> b, val<float*> c) {
///         auto tid = gpu::threadIdx_x() + gpu::blockIdx_x() * gpu::blockDim_x();
///         c[tid] = a[tid] + b[tid];
///     }};
///
/// void host(val<float*> a, val<float*> b, val<float*> c, val<uint32_t> n) {
///     auto blocks = (n + val<uint32_t>(255)) / val<uint32_t>(256);
///     gpu::launch(myKernel, gpu::GridDim{blocks}, gpu::BlockDim{256}, a, b, c);
/// }
/// @endcode
template <typename F, typename... Args>
void launch(NautilusFunction<F>& kernel, GridDim grid, BlockDim block, Args&&... args) {
	setGrid(grid.x, grid.y, grid.z);
	setBlock(block.x, block.y, block.z);
	kernel(std::forward<Args>(args)...);
}

} // namespace nautilus::gpu
