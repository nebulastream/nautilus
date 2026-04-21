#pragma once

#include <cstdint>

// Extern "C" GPU runtime function declarations.
// These serve as stable function pointer addresses for intrinsic matching
// in GPU backends. The backends look up ProxyCallOperations by function
// pointer and replace them with GPU-specific intrinsics (e.g. threadIdx.x).
//
// CPU fallback semantics:
//   threadIdx = 0, blockIdx = 0, blockDim = 1, gridDim = 1, syncThreads = no-op

extern "C" {

uint32_t nautilus_gpu_thread_idx_x();
uint32_t nautilus_gpu_thread_idx_y();
uint32_t nautilus_gpu_thread_idx_z();

uint32_t nautilus_gpu_block_idx_x();
uint32_t nautilus_gpu_block_idx_y();
uint32_t nautilus_gpu_block_idx_z();

uint32_t nautilus_gpu_block_dim_x();
uint32_t nautilus_gpu_block_dim_y();
uint32_t nautilus_gpu_block_dim_z();

uint32_t nautilus_gpu_grid_dim_x();
uint32_t nautilus_gpu_grid_dim_y();
uint32_t nautilus_gpu_grid_dim_z();

void nautilus_gpu_sync_threads();

// Launch configuration intrinsics.
// The backend consumes these to configure the next kernel launch.
// CPU fallback: no-op (ignored, kernel runs as single thread).
void nautilus_gpu_set_grid(uint32_t x, uint32_t y, uint32_t z);
void nautilus_gpu_set_block(uint32_t x, uint32_t y, uint32_t z);

} // extern "C"
