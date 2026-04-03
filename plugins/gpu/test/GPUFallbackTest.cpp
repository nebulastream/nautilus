#include "nautilus/Engine.hpp"
#include "nautilus/gpu/gpu.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/val.hpp"
#include "nautilus/val_ptr.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <vector>

namespace nautilus::gpu::test {

// ============================================================================
// Test kernel functions
// ============================================================================

// Simple kernel: out[tid] = in[tid] * scalar
static auto vecScaleKernel =
    NautilusFunction {"vecScale", [](val<uint32_t*> in, val<uint32_t*> out, val<uint32_t> scalar) {
	                      auto tid = gpu::threadIdx_x();
	                      out[tid] = in[tid] * scalar;
                      }};

// Kernel using multiple indexing intrinsics
static auto globalIdKernel =
    NautilusFunction {"globalId", [](val<uint32_t*> out) {
	                      auto globalId = gpu::threadIdx_x() + gpu::blockIdx_x() * gpu::blockDim_x();
	                      out[globalId] = globalId;
                      }};

// Kernel with control flow
static auto clampKernel =
    NautilusFunction {"clamp", [](val<uint32_t*> data, val<uint32_t> minVal, val<uint32_t> maxVal) {
	                      auto tid = gpu::threadIdx_x();
	                      auto v = data[tid];
	                      if (v < minVal) {
		                      data[tid] = minVal;
	                      } else if (v > maxVal) {
		                      data[tid] = maxVal;
	                      }
                      }};

// ============================================================================
// Host functions that launch kernels
// ============================================================================

void hostVecScale(val<uint32_t*> in, val<uint32_t*> out, val<uint32_t> scalar) {
	gpu::launch(vecScaleKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 256}, in, out, scalar);
}

void hostGlobalId(val<uint32_t*> out) {
	gpu::launch(globalIdKernel, gpu::GridDim {(uint32_t) 4}, gpu::BlockDim {(uint32_t) 64}, out);
}

void hostClamp(val<uint32_t*> data, val<uint32_t> minVal, val<uint32_t> maxVal) {
	gpu::launch(clampKernel, gpu::GridDim {(uint32_t) 1}, gpu::BlockDim {(uint32_t) 128}, data, minVal, maxVal);
}

// ============================================================================
// CPU Fallback Tests (no GPU required)
// ============================================================================

TEST_CASE("GPU CPU Fallback - Intrinsic Default Values") {
	// Without tracing, GPU intrinsic stubs return CPU fallback values
	REQUIRE(stubs::gpu_threadIdx_x() == 0);
	REQUIRE(stubs::gpu_threadIdx_y() == 0);
	REQUIRE(stubs::gpu_threadIdx_z() == 0);
	REQUIRE(stubs::gpu_blockIdx_x() == 0);
	REQUIRE(stubs::gpu_blockIdx_y() == 0);
	REQUIRE(stubs::gpu_blockIdx_z() == 0);
	REQUIRE(stubs::gpu_blockDim_x() == 1);
	REQUIRE(stubs::gpu_blockDim_y() == 1);
	REQUIRE(stubs::gpu_blockDim_z() == 1);
	REQUIRE(stubs::gpu_gridDim_x() == 1);
	REQUIRE(stubs::gpu_gridDim_y() == 1);
	REQUIRE(stubs::gpu_gridDim_z() == 1);
}

TEST_CASE("GPU CPU Fallback - VecScale Interpreter") {
	// Test the vector scale kernel in interpreter mode (no compilation).
	// CPU fallback: threadIdx_x() = 0, so only element [0] is processed.
	engine::Options options;
	options.setOption("engine.Compilation", false);
	auto engine = engine::NautilusEngine(options);

	std::vector<uint32_t> in = {10, 20, 30, 40};
	std::vector<uint32_t> out = {0, 0, 0, 0};
	uint32_t scalar = 3;

	auto f = engine.registerFunction(hostVecScale);
	f(in.data(), out.data(), scalar);

	// Only out[0] should be written (threadIdx_x = 0)
	REQUIRE(out[0] == 30); // 10 * 3
	REQUIRE(out[1] == 0);  // untouched
	REQUIRE(out[2] == 0);  // untouched
	REQUIRE(out[3] == 0);  // untouched
}

#ifdef ENABLE_TRACING
TEST_CASE("GPU CPU Fallback - VecScale Compiled") {
	// Test the vector scale kernel compiled via MLIR backend.
	// CPU fallback: threadIdx_x() = 0, so only element [0] is processed.
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif

	for (auto& backend : backends) {
		DYNAMIC_SECTION("Backend: " + backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			options.setOption("engine.Compilation", true);
			if (backend == "mlir") {
				options.setOption("mlir.enableMultithreading", false);
			}
			auto engine = engine::NautilusEngine(options);

			std::vector<uint32_t> in = {10, 20, 30, 40};
			std::vector<uint32_t> out = {0, 0, 0, 0};
			uint32_t scalar = 3;

			auto f = engine.registerFunction(hostVecScale);
			f(in.data(), out.data(), scalar);

			// Only out[0] should be written (threadIdx_x = 0)
			REQUIRE(out[0] == 30);
			REQUIRE(out[1] == 0);
			REQUIRE(out[2] == 0);
			REQUIRE(out[3] == 0);
		}
	}
}

TEST_CASE("GPU CPU Fallback - Clamp Compiled") {
	// Test the clamp kernel with control flow compiled via MLIR backend.
	std::vector<std::string> backends = {};
#ifdef ENABLE_MLIR_BACKEND
	backends.emplace_back("mlir");
#endif
#ifdef ENABLE_BC_BACKEND
	backends.emplace_back("bc");
#endif

	for (auto& backend : backends) {
		DYNAMIC_SECTION("Backend: " + backend) {
			engine::Options options;
			options.setOption("engine.backend", backend);
			options.setOption("engine.Compilation", true);
			if (backend == "mlir") {
				options.setOption("mlir.enableMultithreading", false);
			}
			auto engine = engine::NautilusEngine(options);

			std::vector<uint32_t> data = {5, 20, 30, 40};
			uint32_t minVal = 10;
			uint32_t maxVal = 25;

			auto f = engine.registerFunction(hostClamp);
			f(data.data(), minVal, maxVal);

			// Only data[0] is accessed (threadIdx_x = 0), 5 < 10 so clamped to 10
			REQUIRE(data[0] == 10);
			REQUIRE(data[1] == 20); // untouched
			REQUIRE(data[2] == 30); // untouched
			REQUIRE(data[3] == 40); // untouched
		}
	}
}
#endif

} // namespace nautilus::gpu::test
