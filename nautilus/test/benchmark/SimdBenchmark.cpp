
#include "BenchmarkUtil.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/vector.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <cstring>
#include <memory>

namespace nautilus::engine {

// ============================================================================
// SIMD benchmark functions (val<vec<T>>)
// ============================================================================

static void benchVectorAddFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) + val<vec<float>>::Load(b)).Store(c);
}

static void benchVectorMulFloat(val<const float*> a, val<const float*> b, val<float*> c) {
	(val<vec<float>>::Load(a) * val<vec<float>>::Load(b)).Store(c);
}

static void benchVectorFmaFloat(val<const float*> a, val<const float*> b, val<const float*> c, val<float*> d) {
	Fma(val<vec<float>>::Load(a), val<vec<float>>::Load(b), val<vec<float>>::Load(c)).Store(d);
}

static val<float> benchVectorDotProduct(val<const float*> a, val<const float*> b) {
	return (val<vec<float>>::Load(a) * val<vec<float>>::Load(b)).ReduceAdd();
}

static val<float> benchVectorReduceAdd(val<const float*> a) {
	return val<vec<float>>::Load(a).ReduceAdd();
}

static val<float> benchVectorDistSq(val<const float*> a, val<const float*> b) {
	auto diff = val<vec<float>>::Load(a) - val<vec<float>>::Load(b);
	return (diff * diff).ReduceAdd();
}

static void benchVectorAddInt(val<const int32_t*> a, val<const int32_t*> b, val<int32_t*> c) {
	(val<vec<int32_t>>::Load(a) + val<vec<int32_t>>::Load(b)).Store(c);
}

static val<int32_t> benchVectorReduceAddInt(val<const int32_t*> a) {
	return val<vec<int32_t>>::Load(a).ReduceAdd();
}

// ============================================================================
// Native C++ scalar baselines (process same number of elements as SIMD width)
// ============================================================================

static void nativeVectorAddFloat(const float* a, const float* b, float* c, size_t n) {
	for (size_t i = 0; i < n; i++) {
		c[i] = a[i] + b[i];
	}
}

static float nativeDotProduct(const float* a, const float* b, size_t n) {
	float sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += a[i] * b[i];
	}
	return sum;
}

static float nativeReduceAdd(const float* a, size_t n) {
	float sum = 0;
	for (size_t i = 0; i < n; i++) {
		sum += a[i];
	}
	return sum;
}

// ============================================================================
// SIMD execution benchmarks
// ============================================================================

TEST_CASE("SIMD Execution Benchmark", "[plugins]") {

	auto backends = benchmark::getEnabledBackends();
	auto lanes = vec<float>::Lanes();

	// Allocate aligned buffers for SIMD operations (max 64 lanes).
	constexpr size_t MAX_LANES = 64 / sizeof(float);
	alignas(64) float bufA[MAX_LANES], bufB[MAX_LANES], bufC[MAX_LANES], bufD[MAX_LANES];
	for (size_t i = 0; i < MAX_LANES; i++) {
		bufA[i] = (float) (i + 1);
		bufB[i] = (float) (i * 2 + 1);
		bufC[i] = 1.0f;
		bufD[i] = 0.0f;
	}

	alignas(64) int32_t ibufA[MAX_LANES], ibufB[MAX_LANES], ibufC[MAX_LANES];
	for (size_t i = 0; i < MAX_LANES; i++) {
		ibufA[i] = (int32_t) (i + 1);
		ibufB[i] = (int32_t) (i * 2 + 1);
		ibufC[i] = 0;
	}

	// Pre-compile all SIMD functions ONCE per backend, then benchmark only execution.
	for (auto& backend : backends) {
		auto op = engine::Options();
		op.setOption("mlir.eager_compilation", true);
		op.setOption("engine.backend", backend);
		op.setOption("engine.compilationStrategy", std::string("legacy"));

		// vectorAdd float
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<void(val<const float*>, val<const float*>, val<float*>)>("f", benchVectorAddFloat);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<void(const float*, const float*, float*)>("f");
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/vectorAdd")
			    .operator=([compiled, fn, &bufA, &bufB, &bufC](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { fn(bufA, bufB, bufC); });
			    });
		}

		// vectorMul float
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<void(val<const float*>, val<const float*>, val<float*>)>("f", benchVectorMulFloat);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<void(const float*, const float*, float*)>("f");
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/vectorMul")
			    .operator=([compiled, fn, &bufA, &bufB, &bufC](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { fn(bufA, bufB, bufC); });
			    });
		}

		// vectorFma
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<void(val<const float*>, val<const float*>, val<const float*>, val<float*>)>(
			    "f", benchVectorFmaFloat);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<void(const float*, const float*, const float*, float*)>("f");
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/vectorFma")
			    .operator=([compiled, fn, &bufA, &bufB, &bufC, &bufD](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { fn(bufA, bufB, bufC, bufD); });
			    });
		}

		// reduceAdd float
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchVectorReduceAdd))>(
			    engine.registerFunction(benchVectorReduceAdd));
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/reduceAdd")
			    .operator=(
			        [fn, &bufA](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(bufA); }); });
		}

		// dotProduct
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<float>(val<const float*>, val<const float*>)>("f", benchVectorDotProduct);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<float(const float*, const float*)>("f");
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/dotProduct")
			    .operator=([compiled, fn, &bufA, &bufB](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return fn(bufA, bufB); });
			    });
		}

		// distanceSquared
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<val<float>(val<const float*>, val<const float*>)>("f", benchVectorDistSq);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<float(const float*, const float*)>("f");
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/distanceSquared")
			    .operator=([compiled, fn, &bufA, &bufB](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { return fn(bufA, bufB); });
			    });
		}

		// vectorAdd int
		{
			auto engine = engine::NautilusEngine(op);
			auto module = engine.createModule();
			module.registerFunction<void(val<const int32_t*>, val<const int32_t*>, val<int32_t*>)>("f",
			                                                                                       benchVectorAddInt);
			auto compiled = std::make_shared<CompiledModule>(module.compile());
			auto fn = compiled->getFunction<void(const int32_t*, const int32_t*, int32_t*)>("f");
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/vectorAddInt")
			    .operator=([compiled, fn, &ibufA, &ibufB, &ibufC](Catch::Benchmark::Chronometer meter) {
				    meter.measure([&] { fn(ibufA, ibufB, ibufC); });
			    });
		}

		// reduceAdd int
		{
			auto engine = engine::NautilusEngine(op);
			auto fn = std::make_shared<decltype(engine.registerFunction(benchVectorReduceAddInt))>(
			    engine.registerFunction(benchVectorReduceAddInt));
			Catch::Benchmark::Benchmark("plugins/simd/" + backend + "/reduceAddInt")
			    .operator=(
			        [fn, &ibufA](Catch::Benchmark::Chronometer meter) { meter.measure([&] { return (*fn)(ibufA); }); });
		}
	}

	// Native scalar baselines (process same number of elements as one SIMD vector width)
	Catch::Benchmark::Benchmark("plugins/simd/native/vectorAdd")
	    .operator=([&bufA, &bufB, &bufC, lanes](Catch::Benchmark::Chronometer meter) {
		    meter.measure([&] { nativeVectorAddFloat(bufA, bufB, bufC, lanes); });
	    });
	Catch::Benchmark::Benchmark("plugins/simd/native/dotProduct")
	    .operator=([&bufA, &bufB, lanes](Catch::Benchmark::Chronometer meter) {
		    meter.measure([&] { return nativeDotProduct(bufA, bufB, lanes); });
	    });
	Catch::Benchmark::Benchmark("plugins/simd/native/reduceAdd")
	    .operator=([&bufA, lanes](Catch::Benchmark::Chronometer meter) {
		    meter.measure([&] { return nativeReduceAdd(bufA, lanes); });
	    });
}

} // namespace nautilus::engine
