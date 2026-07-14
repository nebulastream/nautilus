// Benchmarks for the Apple Metal GPU backend, with a CPU baseline.
//
// The kernels use a grid-stride loop, so the SAME launch function runs both
// ways from one definition:
//   * Metal  — threads stride across the data in parallel on the GPU.
//   * CPU     — on a non-Metal backend the fallback intrinsics report
//               gridDim = blockDim = 1, so the stride is 1 and the kernel
//               degrades to a full serial loop over all N on the CPU.
// This makes "CPU vs Metal" an apples-to-apples comparison of the same total
// work over the same unified buffers (sizes the old NAUTILUS_BUFFER_SIZE
// ceiling could not express), instead of the single-element CPU fallback a
// thread-index kernel would produce.
//
// Compilation happens once in registerFunction (outside the measured loop). On
// Metal the device/queue/library/pipeline are cached in the generated host
// code, so the measured per-call cost is encode + dispatch + waitUntilCompleted;
// the one-time compile cost is measured separately below.

#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/gpu/config.hpp"
#include "nautilus/gpu/gpu.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <string>
#include <vector>

namespace nautilus::engine {

// ============================================================================
// Grid-stride kernels (parallel on GPU, full serial loop on the CPU fallback).
// ============================================================================

// SAXPY: y[i] = a * x[i] + y[i]
static auto benchSaxpy = gpu::NautilusKernelFunction {
    "benchSaxpy", [](gpu::Array<float> x, gpu::Array<float> y, val<float> a, val<uint32_t> n) {
	    auto stride = gpu::gridDim_x() * gpu::blockDim_x();
	    for (val<uint32_t> i = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x(); i < n; i = i + stride) {
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
	    auto stride = gpu::gridDim_x() * gpu::blockDim_x();
	    for (val<uint32_t> i = gpu::blockIdx_x() * gpu::blockDim_x() + gpu::threadIdx_x(); i < n; i = i + stride) {
		    c[i] = a[i] + b[i];
	    }
    }};

void launchVecAdd(val<float*> a, val<float*> b, val<float*> c, val<uint32_t> n) {
	auto blocks = (n + val<uint32_t>((uint32_t) 255)) / val<uint32_t>((uint32_t) 256);
	gpu::launch(benchVecAdd, gpu::GridDim {blocks}, gpu::BlockDim {(uint32_t) 256}, a, b, c, n);
}

// Shared-memory block reduction (Metal-only: the CPU fallback's single thread
// would compute only a per-block partial of one element, so it is not part of
// the CPU-vs-GPU comparison).
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

static engine::NautilusEngine makeEngine(const std::string& backend) {
	engine::Options options;
	options.setOption("engine.backend", backend);
	return engine::NautilusEngine(options);
}

// Element counts spanning the range the old 4096-byte buffer ceiling forbade.
static constexpr uint32_t SIZES[] = {1u << 20, 1u << 22, 1u << 24}; // 1M, 4M, 16M floats

// CPU baseline backends to compare against Metal (full serial loop via the
// grid-stride fallback). MLIR is the optimized native path.
static std::vector<std::string> comparisonBackends() {
	std::vector<std::string> backends;
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
	backends.emplace_back("metal");
	return backends;
}

TEST_CASE("GPU vs CPU Execution Benchmark") {
	for (const auto& backend : comparisonBackends()) {
		for (uint32_t n : SIZES) {
			auto tag = "_" + std::to_string(n / (1u << 20)) + "M_" + backend;

			Catch::Benchmark::Benchmark("saxpy" + tag).operator=([n, backend](Catch::Benchmark::Chronometer meter) {
				auto engine = makeEngine(backend);
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

			Catch::Benchmark::Benchmark("vecadd" + tag).operator=([n, backend](Catch::Benchmark::Chronometer meter) {
				auto engine = makeEngine(backend);
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
		}
	}
}

TEST_CASE("Metal Shared-Memory Benchmark") {
	auto engine = makeEngine("metal");
	auto fn = engine.registerFunction(launchBlockReduce);
	for (uint32_t n : SIZES) {
		auto tag = std::to_string(n / (1u << 20)) + "M";
		Catch::Benchmark::Benchmark("metal_blockreduce_" + tag)
		    .operator=([&fn, n](Catch::Benchmark::Chronometer meter) {
			    auto in = gpu::allocUnified<float>(n);
			    auto out = gpu::allocUnified<float>((n + 255) / 256);
			    for (uint32_t i = 0; i < n; ++i) {
				    in.data()[i] = 1.0f;
			    }
			    meter.measure([&] { return fn(in, out, n), 0; });
			    gpu::freeUnified(in);
			    gpu::freeUnified(out);
		    });
	}
}

TEST_CASE("Metal GPU Compilation Benchmark") {
	// One-time cost: trace -> MSL -> metallib (metal/metallib) -> ObjC++ host
	// dylib (cc) -> dlopen. Dominated by the external toolchain invocations.
	Catch::Benchmark::Benchmark("metal_compile_saxpy").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([&] {
			auto engine = makeEngine("metal");
			auto fn = engine.registerFunction(launchSaxpy);
			(void) fn;
			return 0;
		});
	});
}

#endif // ENABLE_METAL_EXECUTION

} // namespace nautilus::engine
