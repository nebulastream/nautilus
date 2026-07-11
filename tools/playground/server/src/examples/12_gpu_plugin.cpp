// title: GPU plugin: SAXPY kernel (CUDA / Metal)
// description: A gpu::NautilusKernelFunction plus its launch wrapper — pick the CUDA or Metal backend to see the generated device and host code.
#include <nautilus/Engine.hpp>
#include <nautilus/gpu/gpu.hpp> // nautilus-gpu plugin
#include <nautilus/val.hpp>

using namespace nautilus;

// SAXPY (Single-precision A*X Plus Y): y[i] = a * x[i] + y[i].
//
// The kernel body is ordinary traced Nautilus code; gpu::threadIdx_x() and
// friends become the backend's thread-index intrinsics. Select the **CUDA**
// backend to get the generated `.cu` translation unit, or **Metal** for the
// shader plus its Objective-C++ host wrapper. The playground runner has no
// GPU toolchain, so it stops after code generation (gpu.codegenOnly) — on a
// machine with nvcc or Xcode the same module compiles and runs.
static auto saxpyKernel =
    gpu::NautilusKernelFunction {"saxpy", [](val<float*> x, val<float*> y, val<float> a) {
	                                 auto tid = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	                                 y[tid] = a * x[tid] + y[tid];
                                 }};

void playground_register(engine::NautilusModule& m) {
	// The host-side launch wrapper is traced too: grid dimensions computed
	// from `n` end up as regular IR feeding the launch call.
	m.registerFunction<void(val<float*>, val<float*>, val<float>, val<uint32_t>)>(
	    "launchSaxpy", [](val<float*> x, val<float*> y, val<float> a, val<uint32_t> n) {
		    auto blocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
		    gpu::launch(saxpyKernel, gpu::GridDim {blocks}, gpu::BlockDim {(uint32_t) 256}, x, y, a);
	    });
}
