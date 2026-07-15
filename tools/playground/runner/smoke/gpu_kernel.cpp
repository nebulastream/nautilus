// Smoke fixture: GPU kernel launch — codegen-only CUDA/Metal backends must
// dump the generated device (and host) sources.
#include <nautilus/Engine.hpp>
#include <nautilus/gpu/gpu.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

static auto vecAddKernel =
    gpu::NautilusKernelFunction {"vecAdd", [](val<float*> a, val<float*> b, val<float*> c) {
	                                 auto tid = gpu::threadIdx_x();
	                                 c[tid] = a[tid] + b[tid];
                                 }};

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<void(val<float*>, val<float*>, val<float*>)>(
	    "launchVecAdd", [](val<float*> a, val<float*> b, val<float*> c) {
		    gpu::launch(vecAddKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, a, b, c);
	    });
}
