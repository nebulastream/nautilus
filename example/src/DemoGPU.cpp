#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/gpu/gpu.hpp>

using namespace nautilus;

// vecAdd kernel: c[tid] = a[tid] + b[tid]. One thread per element.
// Pointer arguments are gpu::Array<T> — tracked unified buffers.
static auto vecAddKernel =
    gpu::NautilusKernelFunction {"vecAdd", [](gpu::Array<float> a, gpu::Array<float> b, gpu::Array<float> c) {
	                                 auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	                                 c[tid] = a[tid] + b[tid];
                                 }};

// Host function traced into a launch wrapper that dispatches the kernel.
// n drives the grid size and is independent of buffer allocation sizing.
void launchVecAdd(val<float*> a, val<float*> b, val<float*> c, val<uint32_t> n) {
	auto blocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(vecAddKernel, gpu::GridDim {blocks}, gpu::BlockDim {(uint32_t) 256}, a, b, c);
}

int main() {
	// No fixed buffer-size limit: allocate exactly what we need as unified memory.
	constexpr uint32_t N = 4096;

	engine::Options options;
	options.setOption("engine.backend", GPU_DEMO_BACKEND);
	engine::NautilusEngine engine(options);

	auto fn = engine.registerFunction(launchVecAdd);

	// Unified, GPU-accessible buffers — zero copy on both Metal and CUDA.
	auto a = gpu::allocUnified<float>(N);
	auto b = gpu::allocUnified<float>(N);
	auto c = gpu::allocUnified<float>(N);
	for (uint32_t i = 0; i < N; ++i) {
		a.data()[i] = float(i);
		b.data()[i] = float(2 * i);
		c.data()[i] = 0.f;
	}

	fn(a, b, c, N);

	std::cout << "Backend: " << GPU_DEMO_BACKEND << "\n";
	std::cout << "vecAdd over " << N << " floats: a[i] + b[i] = i + 2*i = 3*i\n";
	std::cout << "  c[0]    = " << c.data()[0] << "  (expected 0)\n";
	std::cout << "  c[1]    = " << c.data()[1] << "  (expected 3)\n";
	std::cout << "  c[42]   = " << c.data()[42] << "  (expected 126)\n";
	std::cout << "  c[" << (N - 1) << "] = " << c.data()[N - 1] << "  (expected " << (3 * (N - 1)) << ")\n";

	bool ok = true;
	for (uint32_t i = 0; i < N; ++i) {
		if (c.data()[i] != float(3 * i)) {
			ok = false;
			break;
		}
	}
	std::cout << (ok ? "OK" : "MISMATCH") << "\n";

	gpu::freeUnified(a);
	gpu::freeUnified(b);
	gpu::freeUnified(c);

	return ok ? 0 : 1;
}
