#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/gpu/gpu.hpp>
#include <nautilus/nautilus_function.hpp>

namespace nautilus::engine {

// ============================================================================
// GPU Intrinsic Test Functions
// ============================================================================

// Simple thread index queries
val<uint32_t> gpuThreadIdxX(val<uint32_t> x) {
	return gpu::threadIdx_x() + x;
}

val<uint32_t> gpuBlockIdxX(val<uint32_t> x) {
	return gpu::blockIdx_x() + x;
}

val<uint32_t> gpuBlockDimX(val<uint32_t> x) {
	return gpu::blockDim_x() + x;
}

val<uint32_t> gpuGridDimX(val<uint32_t> x) {
	return gpu::gridDim_x() + x;
}

// Global thread index computation: blockIdx.x * blockDim.x + threadIdx.x
val<uint32_t> gpuGlobalThreadIdx() {
	return gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
}

// Multi-dimensional thread index
val<uint32_t> gpuThreadIdx3D() {
	return gpu::threadIdx_x() + gpu::threadIdx_y() + gpu::threadIdx_z();
}

// Using syncThreads (void return)
void gpuSyncThreadsOnly() {
	gpu::syncThreads();
}

// Kernel-like pattern: global index computation + bounds check
val<uint32_t> gpuBoundedIndex(val<uint32_t> n) {
	auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	if (tid < n) {
		return tid;
	}
	return val<uint32_t>((uint32_t) 0);
}

// Pointer-based kernel pattern: write to output[threadIdx.x]
void gpuVectorAdd(val<uint32_t*> a, val<uint32_t*> b, val<uint32_t*> c) {
	auto tid = gpu::threadIdx_x();
	c[tid] = a[tid] + b[tid];
	gpu::syncThreads();
}

// ============================================================================
// Kernel launch pattern: gpu::NautilusKernelFunction as kernel, host function launches it
// ============================================================================

// Define a kernel as a gpu::NautilusKernelFunction
static auto vecAddKernel =
    gpu::NautilusKernelFunction {"vecAdd", [](val<uint32_t*> a, val<uint32_t*> b, val<uint32_t*> c) {
	                                 auto tid = gpu::threadIdx_x();
	                                 c[tid] = a[tid] + b[tid];
                                 }};

// Host function that launches the kernel via gpu::launch with explicit grid/block dims
void gpuLaunchVecAdd(val<uint32_t*> a, val<uint32_t*> b, val<uint32_t*> c) {
	gpu::launch(vecAddKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, a, b, c);
}

// Host function that computes launch dims dynamically from data
void gpuLaunchVecAddDynamic(val<uint32_t*> a, val<uint32_t*> b, val<uint32_t*> c, val<uint32_t> n) {
	auto numBlocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(vecAddKernel, gpu::GridDim {numBlocks}, gpu::BlockDim {(uint32_t) 256}, a, b, c);
}

// ============================================================================
// Realistic GPU use-case kernels
// ============================================================================

// SAXPY kernel: y[i] = a * x[i] + y[i]
// A classic GPU computing benchmark (Single-precision A*X Plus Y).
static auto saxpyKernel =
    gpu::NautilusKernelFunction {"saxpy", [](val<float*> x, val<float*> y, val<float> a) {
	                                 auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	                                 y[tid] = a * x[tid] + y[tid];
                                 }};

// Host function that launches SAXPY
void gpuLaunchSaxpy(val<float*> x, val<float*> y, val<float> a) {
	gpu::launch(saxpyKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, x, y, a);
}

// Vector scale kernel: out[i] = in[i] * scalar
static auto vecScaleKernel =
    gpu::NautilusKernelFunction {"vecScale", [](val<uint32_t*> in, val<uint32_t*> out, val<uint32_t> scalar) {
	                                 auto tid = gpu::threadIdx_x();
	                                 out[tid] = in[tid] * scalar;
                                 }};

// Host function that launches vector scale
void gpuLaunchVecScale(val<uint32_t*> in, val<uint32_t*> out, val<uint32_t> scalar) {
	gpu::launch(vecScaleKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, in, out, scalar);
}

// Bounded vector add kernel: c[i] = a[i] + b[i] with bounds check (control flow on GPU)
static auto vecAddBoundedKernel = gpu::NautilusKernelFunction {
    "vecAddBounded", [](val<uint32_t*> a, val<uint32_t*> b, val<uint32_t*> c, val<uint32_t> n) {
	    auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	    if (tid < n) {
		    c[tid] = a[tid] + b[tid];
	    }
    }};

// Host function that launches the bounded vector add
void gpuLaunchVecAddBounded(val<uint32_t*> a, val<uint32_t*> b, val<uint32_t*> c, val<uint32_t> n) {
	auto numBlocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(vecAddBoundedKernel, gpu::GridDim {numBlocks}, gpu::BlockDim {(uint32_t) 256}, a, b, c, n);
}

// Nested if kernel: classify values into buckets and write to output
// out[tid] = 3 if a[tid] > 200, 2 if > 100, 1 if > 0, else 0
static auto classifyKernel =
    gpu::NautilusKernelFunction {"classify", [](val<uint32_t*> a, val<uint32_t*> out, val<uint32_t> n) {
	                                 auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	                                 if (tid < n) {
		                                 if (a[tid] > val<uint32_t>((uint32_t) 200)) {
			                                 out[tid] = val<uint32_t>((uint32_t) 3);
		                                 } else {
			                                 if (a[tid] > val<uint32_t>((uint32_t) 100)) {
				                                 out[tid] = val<uint32_t>((uint32_t) 2);
			                                 } else {
				                                 if (a[tid] > val<uint32_t>((uint32_t) 0)) {
					                                 out[tid] = val<uint32_t>((uint32_t) 1);
				                                 } else {
					                                 out[tid] = val<uint32_t>((uint32_t) 0);
				                                 }
			                                 }
		                                 }
	                                 }
                                 }};

void gpuLaunchClassify(val<uint32_t*> a, val<uint32_t*> out, val<uint32_t> n) {
	gpu::launch(classifyKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, a, out, n);
}

// Loop kernel: compute prefix sum per thread (each thread sums a[0..tid])
// out[tid] = sum(a[0..tid])
static auto prefixSumKernel = gpu::NautilusKernelFunction {
    "prefixSum", [](val<uint32_t*> a, val<uint32_t*> out, val<uint32_t> n) {
	    auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	    if (tid < n) {
		    val<uint32_t> sum = val<uint32_t>((uint32_t) 0);
		    for (val<uint32_t> i = val<uint32_t>((uint32_t) 0); i <= tid; i = i + val<uint32_t>((uint32_t) 1)) {
			    sum = sum + a[i];
		    }
		    out[tid] = sum;
	    }
    }};

void gpuLaunchPrefixSum(val<uint32_t*> a, val<uint32_t*> out, val<uint32_t> n) {
	gpu::launch(prefixSumKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, a, out, n);
}

} // namespace nautilus::engine
