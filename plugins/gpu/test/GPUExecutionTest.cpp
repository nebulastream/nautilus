#include "common/GPUFunctions.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/gpu/config.hpp"
#include <catch2/catch_all.hpp>
#include <vector>

namespace nautilus::engine {

void gpuExecutionTest(engine::NautilusEngine& engine) {
	SECTION("threadIdx_x returns 0 on CPU") {
		auto f = engine.registerFunction(gpuThreadIdxX);
		// threadIdx_x() returns 0 on CPU, so result = 0 + 5 = 5
		REQUIRE(f((uint32_t) 5) == (uint32_t) 5);
		REQUIRE(f((uint32_t) 0) == (uint32_t) 0);
		REQUIRE(f((uint32_t) 42) == (uint32_t) 42);
	}

	SECTION("blockIdx_x returns 0 on CPU") {
		auto f = engine.registerFunction(gpuBlockIdxX);
		REQUIRE(f((uint32_t) 10) == (uint32_t) 10);
	}

	SECTION("blockDim_x returns 1 on CPU") {
		auto f = engine.registerFunction(gpuBlockDimX);
		// blockDim_x() returns 1 on CPU, so result = 1 + x
		REQUIRE(f((uint32_t) 0) == (uint32_t) 1);
		REQUIRE(f((uint32_t) 5) == (uint32_t) 6);
	}

	SECTION("gridDim_x returns 1 on CPU") {
		auto f = engine.registerFunction(gpuGridDimX);
		// gridDim_x() returns 1 on CPU, so result = 1 + x
		REQUIRE(f((uint32_t) 0) == (uint32_t) 1);
		REQUIRE(f((uint32_t) 9) == (uint32_t) 10);
	}

	SECTION("global thread index = blockIdx * blockDim + threadIdx = 0*1+0 = 0") {
		auto f = engine.registerFunction(gpuGlobalThreadIdx);
		REQUIRE(f() == (uint32_t) 0);
	}

	SECTION("3D thread index = 0+0+0 on CPU") {
		auto f = engine.registerFunction(gpuThreadIdx3D);
		REQUIRE(f() == (uint32_t) 0);
	}

	SECTION("bounded index: tid=0, n>0 returns 0") {
		auto f = engine.registerFunction(gpuBoundedIndex);
		REQUIRE(f((uint32_t) 10) == (uint32_t) 0);
	}

	SECTION("bounded index: tid=0, n=0 returns 0 (out of bounds path)") {
		auto f = engine.registerFunction(gpuBoundedIndex);
		REQUIRE(f((uint32_t) 0) == (uint32_t) 0);
	}

	SECTION("vector add at index 0") {
		uint32_t a[] = {10, 20, 30};
		uint32_t b[] = {1, 2, 3};
		uint32_t c[] = {0, 0, 0};
		auto f = engine.registerFunction(gpuVectorAdd);
		f(a, b, c);
		// On CPU, threadIdx_x() = 0, so only c[0] is written
		REQUIRE(c[0] == 11);
	}

	SECTION("kernel launch via gpu::launch") {
		uint32_t a[] = {100, 200, 300};
		uint32_t b[] = {1, 2, 3};
		uint32_t c[] = {0, 0, 0};
		auto f = engine.registerFunction(gpuLaunchVecAdd);
		f(a, b, c);
		// CPU fallback: kernel called once with threadIdx=0, writes c[0] only
		REQUIRE(c[0] == 101);
	}

	SECTION("kernel launch with dynamic grid size") {
		uint32_t a[] = {10, 20, 30};
		uint32_t b[] = {1, 2, 3};
		uint32_t c[] = {0, 0, 0};
		auto f = engine.registerFunction(gpuLaunchVecAddDynamic);
		f(a, b, c, (uint32_t) 3);
		// CPU fallback: kernel runs once at threadIdx=0
		REQUIRE(c[0] == 11);
	}
}

TEST_CASE("GPU Interpreter Test") {
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);
	gpuExecutionTest(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("GPU Compiler Test") {
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_C_BACKEND
	backends.emplace_back("cpp");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	backends.emplace_back("asmjit");
#endif
	for (auto& backend : backends) {
		DYNAMIC_SECTION(backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			auto engine = engine::NautilusEngine(options);
			gpuExecutionTest(engine);
		}
	}
}
#endif

#ifdef ENABLE_METAL_EXECUTION
TEST_CASE("GPU Metal Execution Test") {
	constexpr size_t N = 256;
	constexpr size_t bufSizeU32 = N * sizeof(uint32_t);
	constexpr size_t bufSizeF32 = N * sizeof(float);

	SECTION("vector add on GPU") {
		engine::Options options;
		options.setOption("engine.backend", std::string("metal"));
		options.setOption("gpu.metal.bufferSize", static_cast<int>(bufSizeU32));
		auto engine = engine::NautilusEngine(options);

		std::vector<uint32_t> a(N, 0);
		std::vector<uint32_t> b(N, 0);
		std::vector<uint32_t> c(N, 0);
		a[0] = 100;
		a[1] = 200;
		a[2] = 300;
		b[0] = 1;
		b[1] = 2;
		b[2] = 3;

		auto f = engine.registerFunction(gpuLaunchVecAdd);
		f(a.data(), b.data(), c.data());

		REQUIRE(c[0] == 101);
		REQUIRE(c[1] == 202);
		REQUIRE(c[2] == 303);
		REQUIRE(c[3] == 0);
	}

	SECTION("SAXPY: y = a*x + y on GPU") {
		engine::Options options;
		options.setOption("engine.backend", std::string("metal"));
		options.setOption("gpu.metal.bufferSize", static_cast<int>(bufSizeF32));
		auto engine = engine::NautilusEngine(options);

		std::vector<float> x(N, 0.0f);
		std::vector<float> y(N, 0.0f);
		// Initialize test data: x = [1, 2, 3, ...], y = [10, 20, 30, ...]
		for (size_t i = 0; i < N; i++) {
			x[i] = static_cast<float>(i + 1);
			y[i] = static_cast<float>((i + 1) * 10);
		}
		float a = 2.0f;

		auto f = engine.registerFunction(gpuLaunchSaxpy);
		f(x.data(), y.data(), a);

		// y[i] = a * x[i] + y[i] = 2 * (i+1) + (i+1)*10 = (i+1) * 12
		REQUIRE(y[0] == Catch::Approx(12.0f));
		REQUIRE(y[1] == Catch::Approx(24.0f));
		REQUIRE(y[2] == Catch::Approx(36.0f));
		REQUIRE(y[9] == Catch::Approx(120.0f));
		REQUIRE(y[255] == Catch::Approx(256.0f * 12.0f));
	}

	SECTION("vector scale: out = in * scalar on GPU") {
		engine::Options options;
		options.setOption("engine.backend", std::string("metal"));
		options.setOption("gpu.metal.bufferSize", static_cast<int>(bufSizeU32));
		auto engine = engine::NautilusEngine(options);

		std::vector<uint32_t> in(N, 0);
		std::vector<uint32_t> out(N, 0);
		for (size_t i = 0; i < N; i++) {
			in[i] = static_cast<uint32_t>(i + 1);
		}
		uint32_t scalar = 7;

		auto f = engine.registerFunction(gpuLaunchVecScale);
		f(in.data(), out.data(), scalar);

		REQUIRE(out[0] == 7);
		REQUIRE(out[1] == 14);
		REQUIRE(out[2] == 21);
		REQUIRE(out[255] == 256 * 7);
	}

	SECTION("bounded vector add with control flow on GPU") {
		engine::Options options;
		options.setOption("engine.backend", std::string("metal"));
		options.setOption("gpu.metal.bufferSize", static_cast<int>(bufSizeU32));
		auto engine = engine::NautilusEngine(options);

		std::vector<uint32_t> a(N, 0);
		std::vector<uint32_t> b(N, 0);
		std::vector<uint32_t> c(N, 0);
		// Only initialize first 10 elements
		for (size_t i = 0; i < 10; i++) {
			a[i] = static_cast<uint32_t>(i + 1);
			b[i] = static_cast<uint32_t>((i + 1) * 10);
		}

		auto f = engine.registerFunction(gpuLaunchVecAddBounded);
		// Launch with n=10: only threads 0-9 should write, others skip due to bounds check
		f(a.data(), b.data(), c.data(), (uint32_t) 10);

		// Threads 0-9 compute c[i] = a[i] + b[i]
		REQUIRE(c[0] == 11);
		REQUIRE(c[1] == 22);
		REQUIRE(c[9] == 110);
		// Threads 10-255 should NOT write (bounds check)
		REQUIRE(c[10] == 0);
		REQUIRE(c[255] == 0);
	}

	SECTION("nested ifs: classify values into buckets on GPU") {
		engine::Options options;
		options.setOption("engine.backend", std::string("metal"));
		options.setOption("gpu.metal.bufferSize", static_cast<int>(bufSizeU32));
		auto engine = engine::NautilusEngine(options);

		std::vector<uint32_t> a(N, 0);
		std::vector<uint32_t> out(N, 99);
		a[0] = 0;   // bucket 0
		a[1] = 50;  // bucket 1 (> 0)
		a[2] = 150; // bucket 2 (> 100)
		a[3] = 250; // bucket 3 (> 200)
		a[4] = 100; // bucket 1 (> 0, == 100 is not > 100)
		a[5] = 200; // bucket 2 (> 100, == 200 is not > 200)
		a[6] = 201; // bucket 3

		auto f = engine.registerFunction(gpuLaunchClassify);
		f(a.data(), out.data(), (uint32_t) 7);

		REQUIRE(out[0] == 0); // a=0, not > 0
		REQUIRE(out[1] == 1); // a=50, > 0
		REQUIRE(out[2] == 2); // a=150, > 100
		REQUIRE(out[3] == 3); // a=250, > 200
		REQUIRE(out[4] == 1); // a=100, > 0 but not > 100
		REQUIRE(out[5] == 2); // a=200, > 100 but not > 200
		REQUIRE(out[6] == 3); // a=201, > 200
		// Thread 7+ should not write (bounds check), original value 99 stays
		REQUIRE(out[7] == 99);
	}

	SECTION("loop: per-thread prefix sum on GPU") {
		engine::Options options;
		options.setOption("engine.backend", std::string("metal"));
		options.setOption("gpu.metal.bufferSize", static_cast<int>(bufSizeU32));
		auto engine = engine::NautilusEngine(options);

		std::vector<uint32_t> a(N, 0);
		std::vector<uint32_t> out(N, 0);
		// a = [1, 2, 3, 4, 5, ...]
		for (size_t i = 0; i < 10; i++) {
			a[i] = static_cast<uint32_t>(i + 1);
		}

		auto f = engine.registerFunction(gpuLaunchPrefixSum);
		f(a.data(), out.data(), (uint32_t) 10);

		// out[tid] = sum(a[0..tid])
		// out[0] = 1
		// out[1] = 1+2 = 3
		// out[2] = 1+2+3 = 6
		// out[3] = 1+2+3+4 = 10
		// out[9] = 1+2+...+10 = 55
		REQUIRE(out[0] == 1);
		REQUIRE(out[1] == 3);
		REQUIRE(out[2] == 6);
		REQUIRE(out[3] == 10);
		REQUIRE(out[9] == 55);
		// Thread 10+ should not write (bounds check)
		REQUIRE(out[10] == 0);
	}
}
#endif

} // namespace nautilus::engine
