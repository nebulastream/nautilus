
#include "BenchmarkUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/std/bit.h"
#include "nautilus/std/cmath.h"
#include "nautilus/std/cstring.h"
#include <catch2/catch_all.hpp>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace nautilus::engine {

// ============================================================================
// Math intrinsic benchmark functions (val<T>, will be JIT-compiled)
// ============================================================================

static val<float> benchSqrt(val<float> x) {
	return sqrt(x);
}

static val<float> benchSin(val<float> x) {
	return sin(x);
}

static val<float> benchCos(val<float> x) {
	return cos(x);
}

static val<float> benchExp(val<float> x) {
	return exp(x);
}

static val<float> benchLog(val<float> x) {
	return log(x);
}

static val<float> benchPow(val<float> x, val<float> y) {
	return pow(x, y);
}

static val<float> benchFma(val<float> x, val<float> y, val<float> z) {
	return fma(x, y, z);
}

static val<float> benchFloor(val<float> x) {
	return floor(x);
}

static val<float> benchCeil(val<float> x) {
	return ceil(x);
}

// Composite: a small math expression using multiple intrinsics
static val<float> benchMathExpr(val<float> x) {
	return sqrt(x * x + sin(x) * cos(x));
}

// ============================================================================
// Bit intrinsic benchmark functions
// ============================================================================

static val<uint32_t> benchPopcount(val<uint32_t> x) {
	return popcount(x);
}

static val<uint32_t> benchCountlZero(val<uint32_t> x) {
	return countl_zero(x);
}

static val<uint32_t> benchCountrZero(val<uint32_t> x) {
	return countr_zero(x);
}

static val<uint32_t> benchByteswap(val<uint32_t> x) {
	return byteswap(x);
}

static val<uint32_t> benchRotl(val<uint32_t> x, val<uint32_t> s) {
	return rotl(x, s);
}

// Composite: hash-like bit mixing
static val<uint32_t> benchBitMix(val<uint32_t> x) {
	x = x ^ byteswap(x);
	return popcount(x) + countl_zero(x);
}

// ============================================================================
// Memory intrinsic benchmark functions
// ============================================================================

static val<void*> benchMemcpy(val<void*> dest, val<const void*> src, val<size_t> n) {
	return memcpy(dest, src, n);
}

static val<void*> benchMemset(val<void*> dest, val<int> value, val<size_t> n) {
	return memset(dest, value, n);
}

// ============================================================================
// Native C++ baselines
// ============================================================================

static float nativeSqrt(float x) {
	return std::sqrt(x);
}

static float nativeSin(float x) {
	return std::sin(x);
}

static float nativeExp(float x) {
	return std::exp(x);
}

static float nativeLog(float x) {
	return std::log(x);
}

static float nativeMathExpr(float x) {
	return std::sqrt(x * x + std::sin(x) * std::cos(x));
}

static uint32_t nativePopcount(uint32_t x) {
	return __builtin_popcount(x);
}

static uint32_t nativeCountlZero(uint32_t x) {
	return x == 0 ? 32 : __builtin_clz(x);
}

static uint32_t nativeByteswap(uint32_t x) {
	return __builtin_bswap32(x);
}

static void nativeMemcpy(void* dest, const void* src, size_t n) {
	std::memcpy(dest, src, n);
}

static void nativeMemset(void* dest, int value, size_t n) {
	std::memset(dest, value, n);
}

// ============================================================================
// Helper: compile a unary float function once and return a shared callable.
// ============================================================================
using FloatFn = std::function<float(float)>;

static FloatFn compileUnaryFloat(val<float> (*func)(val<float>), const std::string& backend) {
	auto op = engine::Options();
	op.setOption("mlir.eager_compilation", true);
	op.setOption("engine.backend", backend);
	op.setOption("engine.compilationStrategy", std::string("legacy"));
	auto engine = engine::NautilusEngine(op);
	auto module = engine.createModule();
	module.registerFunction<val<float>(val<float>)>("f", func);
	auto compiled = std::make_shared<CompiledModule>(module.compile());
	auto fn = compiled->getFunction<float(float)>("f");
	return [compiled, fn](float x) {
		return fn(x);
	};
}

using UintFn = std::function<uint32_t(uint32_t)>;

static UintFn compileUnaryUint(val<uint32_t> (*func)(val<uint32_t>), const std::string& backend) {
	auto op = engine::Options();
	op.setOption("mlir.eager_compilation", true);
	op.setOption("engine.backend", backend);
	op.setOption("engine.compilationStrategy", std::string("legacy"));
	auto engine = engine::NautilusEngine(op);
	auto module = engine.createModule();
	module.registerFunction<val<uint32_t>(val<uint32_t>)>("f", func);
	auto compiled = std::make_shared<CompiledModule>(module.compile());
	auto fn = compiled->getFunction<uint32_t(uint32_t)>("f");
	return [compiled, fn](uint32_t x) {
		return fn(x);
	};
}

// ============================================================================
// Math intrinsic execution benchmarks
// ============================================================================

TEST_CASE("Math Intrinsic Execution Benchmark", "[plugins]") {

	auto backends = benchmark::getEnabledBackends();

	struct UnaryBench {
		std::string name;
		val<float> (*func)(val<float>);
		float arg;
	};
	auto unaryBenches = std::vector<UnaryBench> {
	    {"sqrt", benchSqrt, 42.0f}, {"sin", benchSin, 1.5f},
	    {"cos", benchCos, 1.5f},    {"exp", benchExp, 2.0f},
	    {"log", benchLog, 42.0f},   {"floor", benchFloor, 3.7f},
	    {"ceil", benchCeil, 3.2f},  {"mathExpr", benchMathExpr, 42.0f},
	};

	// Pre-compile all unary math functions and benchmark only execution.
	for (auto& backend : backends) {
		for (auto& [name, func, arg] : unaryBenches) {
			auto fn = compileUnaryFloat(func, backend);
			Catch::Benchmark::Benchmark("plugins/math/" + backend + "/" + name)
			    .operator=([fn, arg](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return fn(arg); }); });
		}

		// pow (binary)
		{
			auto op = engine::Options();
			op.setOption("mlir.eager_compilation", true);
			op.setOption("engine.backend", backend);
			op.setOption("engine.compilationStrategy", std::string("legacy"));
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<float>(val<float>, val<float>)>("f", benchPow);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<float(float, float)>("f");
			Catch::Benchmark::Benchmark("plugins/math/" + backend + "/pow")
			    .operator=([compiled, fn](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return fn(2.0f, 10.0f); });
			    });
		}

		// fma (ternary)
		{
			auto op = engine::Options();
			op.setOption("mlir.eager_compilation", true);
			op.setOption("engine.backend", backend);
			op.setOption("engine.compilationStrategy", std::string("legacy"));
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<float>(val<float>, val<float>, val<float>)>("f", benchFma);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<float(float, float, float)>("f");
			Catch::Benchmark::Benchmark("plugins/math/" + backend + "/fma")
			    .operator=([compiled, fn](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return fn(2.0f, 3.0f, 4.0f); });
			    });
		}
	}

	// Native baselines
	Catch::Benchmark::Benchmark("plugins/math/native/sqrt").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeSqrt(42.0f); });
	});
	Catch::Benchmark::Benchmark("plugins/math/native/sin").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeSin(1.5f); });
	});
	Catch::Benchmark::Benchmark("plugins/math/native/exp").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeExp(2.0f); });
	});
	Catch::Benchmark::Benchmark("plugins/math/native/log").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeLog(42.0f); });
	});
	Catch::Benchmark::Benchmark("plugins/math/native/mathExpr").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeMathExpr(42.0f); });
	});
}

// ============================================================================
// Bit intrinsic execution benchmarks
// ============================================================================

TEST_CASE("Bit Intrinsic Execution Benchmark", "[plugins]") {

	auto backends = benchmark::getEnabledBackends();

	struct UnaryBitBench {
		std::string name;
		val<uint32_t> (*func)(val<uint32_t>);
		uint32_t arg;
	};
	auto unaryBenches = std::vector<UnaryBitBench> {
	    {"popcount", benchPopcount, 0xDEADBEEF},     {"countlZero", benchCountlZero, 0x000F0000},
	    {"countrZero", benchCountrZero, 0x000F0000}, {"byteswap", benchByteswap, 0x12345678},
	    {"bitMix", benchBitMix, 0xDEADBEEF},
	};

	// Pre-compile all unary bit functions.
	for (auto& backend : backends) {
		for (auto& [name, func, arg] : unaryBenches) {
			auto fn = compileUnaryUint(func, backend);
			Catch::Benchmark::Benchmark("plugins/bit/" + backend + "/" + name)
			    .operator=([fn, arg](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return fn(arg); }); });
		}

		// rotl (binary)
		{
			auto op = engine::Options();
			op.setOption("mlir.eager_compilation", true);
			op.setOption("engine.backend", backend);
			op.setOption("engine.compilationStrategy", std::string("legacy"));
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<uint32_t>(val<uint32_t>, val<uint32_t>)>("f", benchRotl);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<uint32_t(uint32_t, uint32_t)>("f");
			Catch::Benchmark::Benchmark("plugins/bit/" + backend + "/rotl")
			    .operator=([compiled, fn](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return fn(0xDEADBEEF, 13); });
			    });
		}
	}

	// Native baselines
	Catch::Benchmark::Benchmark("plugins/bit/native/popcount").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativePopcount(0xDEADBEEF); });
	});
	Catch::Benchmark::Benchmark("plugins/bit/native/countlZero").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeCountlZero(0x000F0000); });
	});
	Catch::Benchmark::Benchmark("plugins/bit/native/byteswap").operator=([](Catch::Benchmark::Chronometer meter) {
		meter.measure([] { return nativeByteswap(0x12345678); });
	});
}

// ============================================================================
// Memory intrinsic execution benchmarks
// ============================================================================

TEST_CASE("Memory Intrinsic Execution Benchmark", "[plugins]") {

	auto backends = benchmark::getEnabledBackends();

	auto sizes = std::vector<std::pair<std::string, size_t>> {
	    {"64B", 64},
	    {"4KB", 4096},
	    {"1MB", 1024 * 1024},
	};

	// Pre-compile memcpy and memset for each backend.
	for (auto& backend : backends) {
		for (auto& [sizeName, sz] : sizes) {
			// memcpy
			{
				auto op = engine::Options();
				op.setOption("mlir.eager_compilation", true);
				op.setOption("engine.backend", backend);
				op.setOption("engine.compilationStrategy", std::string("legacy"));
				auto engine = engine::NautilusEngine(op);
				auto module = engine.createModule();
				module.registerFunction<val<void*>(val<void*>, val<const void*>, val<size_t>)>("f", benchMemcpy);
				auto compiled = std::make_shared<CompiledModule>(module.compile());
				auto fn = compiled->getFunction<void*(void*, const void*, size_t)>("f");
				auto* src = (char*) malloc(sz);
				auto* dst = (char*) malloc(sz);
				std::memset(src, 0xAB, sz);
				Catch::Benchmark::Benchmark("plugins/memory/" + backend + "/memcpy_" + sizeName)
				    .operator=([compiled, fn, src, dst, sz](Catch::Benchmark::Chronometer meter) {
					    meter.measure([&] { return fn(dst, src, sz); });
				    });
				// Note: buffers intentionally leaked to keep them alive for benchmark.
			}

			// memset
			{
				auto op = engine::Options();
				op.setOption("mlir.eager_compilation", true);
				op.setOption("engine.backend", backend);
				op.setOption("engine.compilationStrategy", std::string("legacy"));
				auto engine = engine::NautilusEngine(op);
				auto module = engine.createModule();
				module.registerFunction<val<void*>(val<void*>, val<int>, val<size_t>)>("f", benchMemset);
				auto compiled = std::make_shared<CompiledModule>(module.compile());
				auto fn = compiled->getFunction<void*(void*, int, size_t)>("f");
				auto* buf = (char*) malloc(sz);
				Catch::Benchmark::Benchmark("plugins/memory/" + backend + "/memset_" + sizeName)
				    .operator=([compiled, fn, buf, sz](Catch::Benchmark::Chronometer meter) {
					    meter.measure([&] { return fn(buf, 0x42, sz); });
				    });
			}
		}
	}

	// Native baselines
	for (auto& [sizeName, sz] : sizes) {
		{
			auto* src = (char*) malloc(sz);
			auto* dst = (char*) malloc(sz);
			std::memset(src, 0xAB, sz);
			Catch::Benchmark::Benchmark("plugins/memory/native/memcpy_" + sizeName)
			    .operator=([src, dst, sz](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { nativeMemcpy(dst, src, sz); });
			    });
		}
		{
			auto* buf = (char*) malloc(sz);
			Catch::Benchmark::Benchmark("plugins/memory/native/memset_" + sizeName)
			    .operator=([buf, sz](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { nativeMemset(buf, 0x42, sz); });
			    });
		}
	}
}

} // namespace nautilus::engine
