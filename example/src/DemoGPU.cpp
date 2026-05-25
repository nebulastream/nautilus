#include <cstdint>
#include <iostream>
#include <vector>

#include <nautilus/Engine.hpp>
#include <nautilus/gpu/gpu.hpp>

#ifdef GPU_DEMO_USE_CUDA
#include <cuda_runtime.h>
#endif

using namespace nautilus;

// vecAdd kernel: c[tid] = a[tid] + b[tid]
// One thread per element, single block of 256 threads.
static auto vecAddKernel = gpu::NautilusKernelFunction {
    "vecAdd", [](val<float*> a, val<float*> b, val<float*> c) {
	    auto tid = gpu::threadIdx_x();
	    c[tid] = a[tid] + b[tid];
    }};

// Host function traced into a launch wrapper that dispatches the kernel.
void launchVecAdd(val<float*> a, val<float*> b, val<float*> c) {
	gpu::launch(vecAddKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, a, b, c);
}

int main() {
	// N is bounded by the generator's NAUTILUS_BUFFER_SIZE (4096 bytes -> 1024 floats).
	constexpr uint32_t N = 256;

	engine::Options options;
	options.setOption("engine.backend", GPU_DEMO_BACKEND);
	engine::NautilusEngine engine(options);

	auto fn = engine.registerFunction(launchVecAdd);

#ifdef GPU_DEMO_USE_CUDA
	// CUDA: unified memory accessible from both host and device.
	float *a = nullptr, *b = nullptr, *c = nullptr;
	cudaMallocManaged(&a, N * sizeof(float));
	cudaMallocManaged(&b, N * sizeof(float));
	cudaMallocManaged(&c, N * sizeof(float));
	for (uint32_t i = 0; i < N; ++i) {
		a[i] = float(i);
		b[i] = float(2 * i);
		c[i] = 0.f;
	}
#else
	// Metal: the generated host wrapper allocates MTLBuffers backed by raw
	// host memory (MTLResourceStorageModeShared) and memcpy's results back.
	std::vector<float> a_vec(N), b_vec(N), c_vec(N, 0.f);
	for (uint32_t i = 0; i < N; ++i) {
		a_vec[i] = float(i);
		b_vec[i] = float(2 * i);
	}
	float *a = a_vec.data(), *b = b_vec.data(), *c = c_vec.data();
#endif

	fn(a, b, c);

	std::cout << "Backend: " << GPU_DEMO_BACKEND << "\n";
	std::cout << "vecAdd over " << N << " floats: a[i] + b[i] = i + 2*i = 3*i\n";
	std::cout << "  c[0]   = " << c[0] << "  (expected 0)\n";
	std::cout << "  c[1]   = " << c[1] << "  (expected 3)\n";
	std::cout << "  c[42]  = " << c[42] << "  (expected 126)\n";
	std::cout << "  c[" << (N - 1) << "] = " << c[N - 1] << "  (expected " << (3 * (N - 1)) << ")\n";

	bool ok = true;
	for (uint32_t i = 0; i < N; ++i) {
		if (c[i] != float(3 * i)) {
			ok = false;
			break;
		}
	}
	std::cout << (ok ? "OK" : "MISMATCH") << "\n";

#ifdef GPU_DEMO_USE_CUDA
	cudaFree(a);
	cudaFree(b);
	cudaFree(c);
#endif

	return ok ? 0 : 1;
}
