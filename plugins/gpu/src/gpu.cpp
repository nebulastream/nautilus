
#include "nautilus/gpu/gpu.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/function.hpp"
#include "nautilus/gpu/config.hpp"
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <unordered_map>
#include <vector>

// The CUDA *codegen* backend (ENABLE_CUDA_BACKEND) can be enabled without the
// CUDA toolkit being installed (it only emits .cu source). The unified-memory
// allocator, however, needs the CUDA runtime headers, so gate it on their
// actual presence. On Apple / toolkit-less hosts this falls back to
// page-aligned host memory suitable for Metal's newBufferWithBytesNoCopy.
#if defined(ENABLE_CUDA_BACKEND) && __has_include(<cuda_runtime.h>)
#define NAUTILUS_GPU_USE_CUDA_RUNTIME 1
#include <cuda_runtime.h>
#else
#include <unistd.h> // getpagesize
#endif

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

void* nautilus_gpu_shared_alloc(uint64_t bytes, uint64_t align) {
	// CPU fallback: single-block semantics. Bump-allocate from a thread-local
	// arena (retained for the thread's lifetime; the CPU path is for
	// correctness, not scale). Each call site gets distinct storage.
	thread_local std::vector<std::unique_ptr<uint8_t[]>> arena;
	auto a = align == 0 ? 1 : align;
	arena.emplace_back(new uint8_t[bytes + a]);
	auto base = reinterpret_cast<uintptr_t>(arena.back().get());
	auto aligned = (base + a - 1) & ~(static_cast<uintptr_t>(a) - 1);
	return reinterpret_cast<void*>(aligned);
}

void nautilus_gpu_set_grid(uint32_t, uint32_t, uint32_t) {
	// no-op on CPU (kernel runs as single thread regardless)
}

void nautilus_gpu_set_block(uint32_t, uint32_t, uint32_t) {
	// no-op on CPU
}

} // extern "C"

// ============================================================================
// Unified-memory runtime: size table + backend allocator.
// ============================================================================

namespace {
std::mutex g_buffer_table_mutex;
// Maps a unified allocation's base pointer to its page-rounded byte length.
// Populated by allocUnified/copy/wrap, consulted by the generated Metal host
// code (via nautilus_gpu_buffer_bytes) to size the no-copy MTLBuffer.
std::unordered_map<void*, uint64_t>& bufferTable() {
	static std::unordered_map<void*, uint64_t> table;
	return table;
}
} // namespace

extern "C" {

/// Returns the page-rounded byte length registered for a unified buffer base
/// pointer. Called from generated Metal host code (address embedded as a
/// literal). Throws if the pointer was not produced by allocUnified/copy/wrap.
uint64_t nautilus_gpu_buffer_bytes(void* ptr) {
	std::lock_guard<std::mutex> lock(g_buffer_table_mutex);
	auto& table = bufferTable();
	auto it = table.find(ptr);
	if (it == table.end()) {
		throw std::runtime_error("nautilus_gpu_buffer_bytes: pointer was not allocated via "
		                         "gpu::allocUnified/copy/wrap");
	}
	return it->second;
}

} // extern "C"

namespace nautilus::gpu::detail {

size_t pageRoundUp(size_t bytes) {
#ifdef NAUTILUS_GPU_USE_CUDA_RUNTIME
	// CUDA managed memory has no page-alignment requirement for the kernel ABI;
	// keep the logical size.
	return bytes;
#else
	auto pg = static_cast<size_t>(getpagesize());
	return ((bytes + pg - 1) / pg) * pg;
#endif
}

void* allocUnifiedBytes(size_t bytes) {
	auto rounded = pageRoundUp(bytes == 0 ? 1 : bytes);
#ifdef NAUTILUS_GPU_USE_CUDA_RUNTIME
	void* p = nullptr;
	if (cudaMallocManaged(&p, rounded) != cudaSuccess) {
		throw std::runtime_error("gpu::allocUnified: cudaMallocManaged failed");
	}
	return p;
#else
	void* p = nullptr;
	if (posix_memalign(&p, static_cast<size_t>(getpagesize()), rounded) != 0) {
		throw std::runtime_error("gpu::allocUnified: posix_memalign failed");
	}
	return p;
#endif
}

void freeUnifiedBytes(void* ptr) {
#ifdef NAUTILUS_GPU_USE_CUDA_RUNTIME
	cudaFree(ptr);
#else
	free(ptr);
#endif
}

void registerBuffer(void* ptr, uint64_t allocBytes) {
	std::lock_guard<std::mutex> lock(g_buffer_table_mutex);
	bufferTable()[ptr] = allocBytes;
}

void unregisterBuffer(void* ptr) {
	std::lock_guard<std::mutex> lock(g_buffer_table_mutex);
	bufferTable().erase(ptr);
}

} // namespace nautilus::gpu::detail

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
