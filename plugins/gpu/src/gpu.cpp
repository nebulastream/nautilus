
#include "nautilus/gpu/gpu.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/function.hpp"
#include "nautilus/gpu/config.hpp"
#include <cstdint>

#ifdef ENABLE_CUDA_BACKEND
#include "cuda/CUDACompilationBackend.hpp"
#endif
#ifdef ENABLE_METAL_BACKEND
#include "metal/MetalCompilationBackend.hpp"
#endif

// ============================================================================
// Wrapper functions with extern "C" linkage for intrinsic registration.
// These provide the CPU fallback and stable function pointer addresses
// that GPU backends match to replace with GPU intrinsics.
// ============================================================================

extern "C" {

uint32_t nautilus_gpu_thread_idx_x() {
	return 0;
}
uint32_t nautilus_gpu_thread_idx_y() {
	return 0;
}
uint32_t nautilus_gpu_thread_idx_z() {
	return 0;
}

uint32_t nautilus_gpu_block_idx_x() {
	return 0;
}
uint32_t nautilus_gpu_block_idx_y() {
	return 0;
}
uint32_t nautilus_gpu_block_idx_z() {
	return 0;
}

uint32_t nautilus_gpu_block_dim_x() {
	return 1;
}
uint32_t nautilus_gpu_block_dim_y() {
	return 1;
}
uint32_t nautilus_gpu_block_dim_z() {
	return 1;
}

uint32_t nautilus_gpu_grid_dim_x() {
	return 1;
}
uint32_t nautilus_gpu_grid_dim_y() {
	return 1;
}
uint32_t nautilus_gpu_grid_dim_z() {
	return 1;
}

void nautilus_gpu_sync_threads() {
	// no-op on CPU
}

void nautilus_gpu_set_grid(uint32_t, uint32_t, uint32_t) {
	// no-op on CPU (kernel runs as single thread regardless)
}

void nautilus_gpu_set_block(uint32_t, uint32_t, uint32_t) {
	// no-op on CPU
}

} // extern "C"

namespace nautilus::gpu {

val<uint32_t> threadIdx_x() {
	return invoke<uint32_t>(nautilus_gpu_thread_idx_x);
}
val<uint32_t> threadIdx_y() {
	return invoke<uint32_t>(nautilus_gpu_thread_idx_y);
}
val<uint32_t> threadIdx_z() {
	return invoke<uint32_t>(nautilus_gpu_thread_idx_z);
}

val<uint32_t> blockIdx_x() {
	return invoke<uint32_t>(nautilus_gpu_block_idx_x);
}
val<uint32_t> blockIdx_y() {
	return invoke<uint32_t>(nautilus_gpu_block_idx_y);
}
val<uint32_t> blockIdx_z() {
	return invoke<uint32_t>(nautilus_gpu_block_idx_z);
}

val<uint32_t> blockDim_x() {
	return invoke<uint32_t>(nautilus_gpu_block_dim_x);
}
val<uint32_t> blockDim_y() {
	return invoke<uint32_t>(nautilus_gpu_block_dim_y);
}
val<uint32_t> blockDim_z() {
	return invoke<uint32_t>(nautilus_gpu_block_dim_z);
}

val<uint32_t> gridDim_x() {
	return invoke<uint32_t>(nautilus_gpu_grid_dim_x);
}
val<uint32_t> gridDim_y() {
	return invoke<uint32_t>(nautilus_gpu_grid_dim_y);
}
val<uint32_t> gridDim_z() {
	return invoke<uint32_t>(nautilus_gpu_grid_dim_z);
}

void syncThreads() {
	invoke<void>(nautilus_gpu_sync_threads);
}

void setGrid(val<uint32_t> x, val<uint32_t> y, val<uint32_t> z) {
	invoke<void, uint32_t, uint32_t, uint32_t>(nautilus_gpu_set_grid, x, y, z);
}

void setBlock(val<uint32_t> x, val<uint32_t> y, val<uint32_t> z) {
	invoke<void, uint32_t, uint32_t, uint32_t>(nautilus_gpu_set_block, x, y, z);
}

} // namespace nautilus::gpu

// Static initializer registers GPU compilation backends with the core registry.
// Lives in this TU because gpu.cpp is always pulled in by users of the public
// nautilus::gpu API, which guarantees the registrar fires without needing
// linker --whole-archive tricks.
namespace {
struct GPUPluginInit {
	GPUPluginInit() {
		auto* registry = nautilus::compiler::CompilationBackendRegistry::getInstance();
#ifdef ENABLE_CUDA_BACKEND
		registry->registerBackend("cuda", std::make_unique<nautilus::compiler::cuda::CUDACompilationBackend>());
#endif
#ifdef ENABLE_METAL_BACKEND
		registry->registerBackend("metal", std::make_unique<nautilus::compiler::metal::MetalCompilationBackend>());
#endif
	}
};
static GPUPluginInit nautilus_gpu_plugin_init_;
} // namespace
