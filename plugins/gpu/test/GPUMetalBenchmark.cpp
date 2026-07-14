// Benchmarks for the Apple Metal GPU backend.
//
// Measures steady-state kernel execution over large unified buffers (the data
// sizes that the old fixed NAUTILUS_BUFFER_SIZE ceiling could not express), plus
// the one-time compilation cost (trace -> MSL -> metallib -> ObjC++ host dylib).
//
// Compilation happens once in registerFunction (outside the measured loop); the
// device/queue/library/pipeline are cached in the generated host code, so the
// measured per-call cost is command-buffer encode + dispatch + waitUntilCompleted.
//
// Only meaningful on Apple with the Metal backend; the launch wrappers fall back
// to a single CPU thread on other backends, so cross-backend comparison here
// would be apples-to-oranges and is intentionally omitted.

#include "nautilus/Engine.hpp"
#include "nautilus/gpu/config.hpp"
#include "nautilus/gpu/gpu.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>

namespace nautilus::engine {

// ============================================================================
// Benchmark kernels (global thread index + bounds check => arbitrary N).
// ============================================================================

// SAXPY: y[i] = a * x[i] + y[i]
static auto benchSaxpy = gpu::NautilusKernelFunction {
    "benchSaxpy", [](gpu::Array<float> x, gpu::Array<float> y, val<float> a, val<uint32_t> n) {
	    auto i = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	    if (i < n) {
		    y[i] = a * x[i] + y[i];
	    }
    }};

void launchSaxpy(val<float*> x, val<float*> y, val<float> a, val<uint32_t> n) {
	auto blocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(benchSaxpy, gpu::GridDim {blocks}, gpu::BlockDim {(uint32_t) 256}, x, y, a, n);
}

// Vector add: c[i] = a[i] + b[i]
static auto benchVecAdd = gpu::NautilusKernelFunction {
    "benchVecAdd", [](gpu::Array<float> a, gpu::Array<float> b, gpu::Array<float> c, val<uint32_t> n) {
	    auto i = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x();
	    if (i < n) {
		    c[i] = a[i] + b[i];
	    }
    }};

void launchVecAdd(val<float*> a, val<float*> b, val<float*> c, val<uint32_t> n) {
	auto blocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(benchVecAdd, gpu::GridDim {blocks}, gpu::BlockDim {(uint32_t) 256}, a, b, c, n);
}

// Shared-memory block reduction: out[blockIdx] = sum of this block's 256 inputs.
static auto benchBlockReduce =
    gpu::NautilusKernelFunction {"benchBlockReduce", [](gpu::Array<float> in, gpu::Array<float> out, val<uint32_t> n) {
	                                 auto tile = gpu::sharedArray<float, 256>();
	                                 auto tid = gpu::threadIdx_x();
	                                 auto gid = gpu::blockIdx_x() * gpu::blockDim_x() + tid;
	                                 val<float> v = val<float>(0.0f);
	                                 if (gid < n) {
		                                 v = in[gid];
	                                 }
	                                 tile[tid] = v;
	                                 gpu::syncThreads();
	                                 for (val<uint32_t> s = gpu::blockDim_x() / val<uint32_t>((uint32_t) 2);
	                                      s > val<uint32_t>((uint32_t) 0); s = s / val<uint32_t>((uint32_t) 2)) {
		                                 if (tid < s) {
			                                 tile[tid] = tile[tid] + tile[tid + s];
		                                 }
		                                 gpu::syncThreads();
	                                 }
	                                 if (tid == val<uint32_t>((uint32_t) 0)) {
		                                 val<float> total = tile[(uint32_t) 0];
		                                 out[gpu::blockIdx_x()] = total;
	                                 }
                                 }};

void launchBlockReduce(val<float*> in, val<float*> out, val<uint32_t> n) {
	auto blocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(benchBlockReduce, gpu::GridDim {blocks}, gpu::BlockDim {(uint32_t) 256}, in, out, n);
}

#ifdef ENABLE_METAL_EXECUTION

static engine::NautilusEngine makeMetalEngine() {
	engine::Options options;
	options.setOption("engine.backend", std::string("metal"));
	return engine::NautilusEngine(options);
}

// Element counts spanning the range the old 4096-byte buffer ceiling forbade.
static constexpr uint32_t SIZES[] = {1u << 20, 1u << 22, 1u << 24}; // 1M, 4M, 16M floats

TEST_CASE("Metal GPU Execution Benchmark") {
	for (uint32_t n : SIZES) {
		auto mb = (n * sizeof(float)) / (1024 * 1024);
		auto tag = std::to_string(n / (1u << 20)) + "M";

		// SAXPY: 2 buffers (x read, y read+write).
		Catch::Benchmark::Benchmark("metal_saxpy_" + tag).operator=([n](Catch::Benchmark::Chronometer meter) {
			auto engine = makeMetalEngine();
			auto fn = engine.registerFunction(launchSaxpy);
			auto x = gpu::allocUnified<float>(n);
			auto y = gpu::allocUnified<float>(n);
			for (uint32_t i = 0; i < n; ++i) {
				x.data()[i] = 1.0f;
				y.data()[i] = 2.0f;
			}
			meter.measure([&] { return fn(x, y, 3.0f, n), 0; });
			gpu::freeUnified(x);
			gpu::freeUnified(y);
		});

		// Vector add: 3 buffers.
		Catch::Benchmark::Benchmark("metal_vecadd_" + tag).operator=([n](Catch::Benchmark::Chronometer meter) {
			auto engine = makeMetalEngine();
			auto fn = engine.registerFunction(launchVecAdd);
			auto a = gpu::allocUnified<float>(n);
			auto b = gpu::allocUnified<float>(n);
			auto c = gpu::allocUnified<float>(n);
			for (uint32_t i = 0; i < n; ++i) {
				a.data()[i] = 1.0f;
				b.data()[i] = 2.0f;
			}
			meter.measure([&] { return fn(a, b, c, n), 0; });
			gpu::freeUnified(a);
			gpu::freeUnified(b);
			gpu::freeUnified(c);
		});

		// Shared-memory block reduction.
		Catch::Benchmark::Benchmark("metal_blockreduce_" + tag).operator=([n](Catch::Benchmark::Chronometer meter) {
			auto engine = makeMetalEngine();
			auto fn = engine.registerFunction(launchBlockReduce);
			auto in = gpu::allocUnified<float>(n);
			auto out = gpu::allocUnified<float>((n + 255) / 256);
			for (uint32_t i = 0; i < n; ++i) {
				in.data()[i] = 1.0f;
			}
			meter.measure([&] { return fn(in, out, n), 0; });
			gpu::freeUnified(in);
			gpu::freeUnified(out);
		});

		(void) mb;
	}
}

TEST_CASE("Metal GPU Compilation Benchmark") {
	// One-time cost: trace -> MSL -> metallib (metal/metallib) -> ObjC++ host
	// dylib (cc) -> dlopen. Dominated by the external toolchain invocations.
	Catch::Benchmark::Benchmark("metal_compile_saxpy").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([&] {
			auto engine = makeMetalEngine();
			auto fn = engine.registerFunction(launchSaxpy);
			(void) fn;
			return 0;
		});
	});
}

#endif // ENABLE_METAL_EXECUTION

} // namespace nautilus::engine
