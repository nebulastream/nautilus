
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>
#ifdef ENABLE_TBC_JIT
#include "nautilus/function.hpp"
#include "nautilus/nautilus_function.hpp"

namespace nautilus::compiler::tbc::jit {
// Defined in libnautilus; gates the copy-and-patch benchmarks on builds
// that can execute stitched code.
bool jitRuntimeAvailable();
} // namespace nautilus::compiler::tbc::jit
#endif

namespace nautilus::engine {

val<int8_t> addExpression(val<int8_t> x) {
	val<int8_t> y = (int8_t) 2;
	return y + x;
}

val<int32_t> fib(val<int32_t> n) {
	val<int32_t> a = 0, b = 1, c;
	for (val<int> i = 2; i <= n; i = i + 1) {
		c = a + b;
		a = b;
		b = c;
	}
	return b;
}
#ifdef ENABLE_TBC_JIT
// Call-heavy kernels for the copy-and-patch A/B: the loop kernels above never
// leave one function, so they cannot show the cost of the JIT's frame-push
// helper (internal CALL) or the dyncall bridge (external CALL_EXT).
val<int64_t> tbcJitCalleeBody(val<int64_t> x, val<int64_t> y) {
	return x * 2 + y;
}
static auto tbcJitBenchCallee = NautilusFunction {"tbcJitBenchCallee", tbcJitCalleeBody};

val<int64_t> internalCallLoop(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = tbcJitBenchCallee(acc, val<int64_t>(1));
	}
	return acc;
}

int64_t tbcJitNativeAdd(int64_t a, int64_t b) noexcept {
	return a + b;
}

val<int64_t> externalCallLoop(val<int32_t> n) {
	val<int64_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		acc = invoke(tbcJitNativeAdd, acc, val<int64_t>(1));
	}
	return acc;
}
#endif

val<int32_t> sum(val<int32_t*> array, val<int32_t> length) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < length; i = i + 1) {
		val<int32_t> value = array[i];
		sum = sum + value;
	}
	return sum;
}

void runAddBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(addExpression);
	meter.measure([&] { return func(42); });
}

void runFibBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(fib);
	meter.measure([&] { return func(15000); });
}

void runArraySumBenchmark(Catch::Benchmark::Chronometer& meter, Options& options) {
	auto engine = engine::NautilusEngine(options);
	auto func = engine.registerFunction(sum);
	auto size = 1024 * 1024 * 10;
	auto buffer = (int32_t*) malloc(size);
	meter.measure([&] { return func(buffer, size / sizeof(int32_t)); });
}

static auto benchmarks =
    std::vector<std::tuple<std::string, std::function<void(Catch::Benchmark::Chronometer& meter, Options& options)>>> {
        {"add", runAddBenchmark},
        {"fibonacci", runFibBenchmark},
        {"sum", runArraySumBenchmark},
    };

TEST_CASE("Execution Benchmark") {
	// One configuration per backend, with every optimization this suite
	// knows about switched on: this tracks steady-state production
	// performance rather than the marginal effect of any single flag (that
	// per-flag A/B coverage lives in CompilationStatisticsTest and the
	// *ModeTest execution tests). TBC is the exception -- it gets two
	// configs, interpreted and copy-and-patch JIT, since that split is the
	// point of having the backend.
	std::vector<std::tuple<std::string, Options>> configs;

#ifdef ENABLE_MLIR_BACKEND
	{
		Options op;
		op.setOption("engine.backend", std::string("mlir"));
		// force synchronous compilation for the MLIR backend.
		op.setOption("mlir.eager_compilation", true);
		configs.emplace_back("mlir", op);
	}
#endif
#ifdef ENABLE_C_BACKEND
	{
		Options op;
		op.setOption("engine.backend", std::string("cpp"));
		configs.emplace_back("cpp", op);
	}
#endif
#ifdef ENABLE_BC_BACKEND
	{
		Options op;
		op.setOption("engine.backend", std::string("bc"));
		op.setOption("ir.enableLICM", true);
		op.setOption("ir.enableLocalCSE", true);
		op.setOption("bc.registerAllocator", true);
		op.setOption("bc.dispatch", std::string("threaded"));
		op.setOption("bc.regfileReuse", true);
		op.setOption("bc.superinstructions", true);
		op.setOption("bc.immediates", true);
		configs.emplace_back("bc", op);
	}
#endif
#ifdef ENABLE_ASMJIT_BACKEND
	{
		Options op;
		op.setOption("engine.backend", std::string("asmjit"));
		op.setOption("ir.enableLICM", true);
		op.setOption("ir.enableLocalCSE", true);
		op.setOption("asmjit.enableBranchFusion", true);
		op.setOption("asmjit.enableConstFolding", true);
		op.setOption("asmjit.enableSelectCmov", true);
		configs.emplace_back("asmjit", op);
	}
#endif
#ifdef ENABLE_TBC_BACKEND
	{
		Options op;
		op.setOption("engine.backend", std::string("tbc"));
		op.setOption("ir.enableLICM", true);
		op.setOption("ir.enableLocalCSE", true);
		op.setOption("tbc.mode", std::string("interp"));
		configs.emplace_back("tbc_interp", op);
	}
#ifdef ENABLE_TBC_JIT
	if (compiler::tbc::jit::jitRuntimeAvailable()) {
		Options op;
		op.setOption("engine.backend", std::string("tbc"));
		op.setOption("ir.enableLICM", true);
		op.setOption("ir.enableLocalCSE", true);
		op.setOption("tbc.mode", std::string("jit"));
		configs.emplace_back("tbc_jit", op);
	}
#endif
#endif

	for (auto& [name, op] : configs) {
		for (auto& test : benchmarks) {
			auto func = std::get<1>(test);
			auto testName = std::get<0>(test);
			Catch::Benchmark::Benchmark("exec_" + name + "_" + testName)
			    .operator=([&func, &op](Catch::Benchmark::Chronometer meter) { func(meter, op); });
		}
	}

#ifdef ENABLE_TBC_JIT
	// Call-heavy A/B between the two TBC configs above: the loop kernels
	// never leave one function, so they cannot show the cost of the JIT's
	// frame-push helper (internal CALL) or the dyncall bridge (external
	// CALL_EXT).
	auto findConfig = [&configs](const std::string& configName) -> const Options* {
		for (auto& [name, op] : configs) {
			if (name == configName) {
				return &op;
			}
		}
		return nullptr;
	};
	if (const auto* jitOptions = findConfig("tbc_jit")) {
		const auto* interpOptions = findConfig("tbc_interp");
		auto callBenchmarks =
		    std::vector<std::tuple<std::string, std::function<void(Catch::Benchmark::Chronometer&, Options&)>>> {
		        {"internalCall",
		         [](Catch::Benchmark::Chronometer& meter, Options& options) {
			         auto engine = engine::NautilusEngine(options);
			         auto func = engine.registerFunction(internalCallLoop);
			         meter.measure([&] { return func(10000); });
		         }},
		        {"externalCall",
		         [](Catch::Benchmark::Chronometer& meter, Options& options) {
			         auto engine = engine::NautilusEngine(options);
			         auto func = engine.registerFunction(externalCallLoop);
			         meter.measure([&] { return func(10000); });
		         }},
		    };
		std::vector<std::tuple<std::string, Options>> tbcCallConfigs = {
		    {"interp", *interpOptions},
		    {"jit", *jitOptions},
		};
		for (auto& [tbcName, op] : tbcCallConfigs) {
			for (auto& test : callBenchmarks) {
				auto func = std::get<1>(test);
				auto testName = std::get<0>(test);
				Catch::Benchmark::Benchmark("exec_tbc_" + testName + "_" + tbcName)
				    .operator=([&func, &op](Catch::Benchmark::Chronometer meter) { func(meter, op); });
			}
		}
	}
#endif
}

} // namespace nautilus::engine
