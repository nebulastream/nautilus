#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include <catch2/catch_all.hpp>
#include <filesystem>
#include <fstream>
#include <sstream>

#if defined(ENABLE_MLIR_BACKEND) && !defined(__APPLE__)
#include "nautilus/specialization/specialize.hpp"
#include "nautilus/specialization/specialized_function.hpp"

namespace nautilus::engine {

// ---------------------------------------------------------------------------
// ValueProfile<T> — runtime counter that SpecializedNautilusFunction reads
// at trace time to decide which dispatcher variant to emit.
// ---------------------------------------------------------------------------

TEST_CASE("ValueProfile::profile updates hits and total") {
	ValueProfile<int64_t> p;
	REQUIRE_FALSE(p.isStable(0.5, 4));
	ValueProfile<int64_t>::profile(&p, 5);
	ValueProfile<int64_t>::profile(&p, 5);
	ValueProfile<int64_t>::profile(&p, 5);
	ValueProfile<int64_t>::profile(&p, 5);
	REQUIRE(p.isStable(0.5, 4));
	REQUIRE(p.dominant() == 5);

	// A different value resets the hit streak but keeps total growing.
	ValueProfile<int64_t>::profile(&p, 7);
	REQUIRE(p.dominant() == 7);
}

TEST_CASE("ValueProfile: non-dominant values keep profile unstable") {
	ValueProfile<int64_t> p;
	for (int i = 0; i < 2048; ++i) {
		ValueProfile<int64_t>::profile(&p, i % 2);
	}
	REQUIRE_FALSE(p.isStable());
	REQUIRE_FALSE(p.isStable(0.01, 1024));
}

TEST_CASE("ValueProfile: min_samples gate") {
	ValueProfile<int32_t> p;
	for (int i = 0; i < 16; ++i) {
		ValueProfile<int32_t>::profile(&p, 42);
	}
	REQUIRE_FALSE(p.isStable());   // default 1024 samples required
	REQUIRE(p.isStable(0.95, 16)); // lowered gate -> stable
	REQUIRE(p.dominant() == 42);
}

// ---------------------------------------------------------------------------
// IR-dump helper. We deliberately do not use the reference-file comparison
// harness because the dispatcher bakes profile pointers into the IR as
// `inttoptr (i64 N to ptr)` literals, and `N` changes across runs.
// ---------------------------------------------------------------------------

struct CompiledDumps {
	std::string mlir;
	std::string llvm;
};

static CompiledDumps readCompiledIR(const std::string& tag, auto kernel) {
	auto dumpPath = (std::filesystem::temp_directory_path() / ("nautilus-specialize-ir-" + tag)).string();
	std::filesystem::remove_all(dumpPath);
	std::filesystem::create_directories(dumpPath);

	engine::Options options;
	options.setOption("engine.backend", "mlir");
	options.setOption("dump.after_llvm_generation", true);
	options.setOption("dump.after_mlir_generation", true);
	options.setOption("dump.all", true);
	options.setOption("dump.path", dumpPath);
	options.setOption("engine.normalizeFunctionNames", true);
	options.setOption("mlir.enableIntrinsics", true);
	options.setOption("mlir.enableMultithreading", false);

	auto engine = engine::NautilusEngine(options);
	auto function = engine.registerFunction(kernel);

	auto slurp = [](std::string_view path) {
		std::ifstream in {std::string {path}};
		std::stringstream buf;
		buf << in.rdbuf();
		return buf.str();
	};

	auto mlirFile = function.getExecutable()->getGeneratedFile("after_mlir_generation");
	auto llvmFile = function.getExecutable()->getGeneratedFile("after_llvm_generation");
	REQUIRE(!mlirFile.empty());
	REQUIRE(!llvmFile.empty());
	return {slurp(mlirFile), slurp(llvmFile)};
}

// ---------------------------------------------------------------------------
// SpecializedNautilusFunction: wraps a user kernel and exposes itself as a
// regular NautilusFunction. The dispatch logic (profile-update vs traced
// if/else into __specialized / __generic) lives inside the dispatcher
// symbol's own traced body, so a parent trace sees exactly one
// `traceNautilusCall` to the dispatcher.
// ---------------------------------------------------------------------------

static val<int64_t> specNF_pow(val<int64_t> base, val<int64_t> exp) {
	val<int64_t> r = 1;
	for (val<int64_t> i = 0; i < exp; i = i + 1) {
		r = r * base;
	}
	return r;
}

TEST_CASE("SpecializedNautilusFunction: profile empty -> generic path + profile update") {
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"specnf_pow", specNF_pow};

	auto engine = nautilus::testing::makeEngine("mlir", [&](engine::Options& options) {
		options.setOption("mlir.enableIntrinsics", true);
		options.setOption("mlir.enableMultithreading", false);
	});
	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};
	auto fn = engine.registerFunction(wrapper);

	REQUIRE(fn(2, 5) == 32);
	REQUIRE(fn(3, 4) == 81);
	REQUIRE(fn(7, 0) == 1);
	for (int i = 0; i < 2048; ++i) {
		REQUIRE(fn(2, 5) == 32);
	}
	REQUIRE(spec.profile<0>().isStable());
	REQUIRE(spec.profile<0>().dominant() == 5);
}

TEST_CASE("SpecializedNautilusFunction: stable profile -> dispatcher picks specialized/generic") {
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"specnf_pow_stable", specNF_pow};
	for (int i = 0; i < 2048; ++i) {
		ValueProfile<int64_t>::profile(&spec.profile<0>(), 5);
	}
	REQUIRE(spec.profile<0>().isStable());

	auto engine = nautilus::testing::makeEngine("mlir", [&](engine::Options& options) {
		options.setOption("mlir.enableIntrinsics", true);
		options.setOption("mlir.enableMultithreading", false);
	});
	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};
	auto fn = engine.registerFunction(wrapper);

	// Specialized branch (exp == 5).
	REQUIRE(fn(2, 5) == 32);
	REQUIRE(fn(3, 5) == 243);
	REQUIRE(fn(-2, 5) == -32);
	REQUIRE(fn(1, 5) == 1);
	// Generic branch (exp != 5).
	REQUIRE(fn(2, 10) == 1024);
	REQUIRE(fn(3, 4) == 81);
	REQUIRE(fn(5, 0) == 1);
	REQUIRE(fn(7, 1) == 7);
}

TEST_CASE("SpecializedNautilusFunction IR: dispatcher hides cond_br + variant calls") {
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"specnf_ir", specNF_pow};
	for (int i = 0; i < 2048; ++i) {
		ValueProfile<int64_t>::profile(&spec.profile<0>(), 5);
	}
	REQUIRE(spec.profile<0>().isStable());

	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};
	const auto dumps = readCompiledIR("specnf", wrapper);
	INFO("== Pre-opt MLIR for SpecializedNautilusFunction (stable):\n" << dumps.mlir);

	// Externally a SpecializedNautilusFunction looks like a regular
	// NautilusFunction: the parent `execute` contains exactly one call
	// to the dispatcher symbol `@specnf_ir`. The dispatch logic
	// (cond_br + nested calls into the two variants) lives in the
	// dispatcher's own `func.func @specnf_ir` body, not in `execute`.
	REQUIRE(dumps.mlir.find("func.func @execute") != std::string::npos);
	REQUIRE(dumps.mlir.find("call @specnf_ir(") != std::string::npos);
	REQUIRE(dumps.mlir.find("func.func @specnf_ir(") != std::string::npos);
	REQUIRE(dumps.mlir.find("func.func @specnf_ir__generic(") != std::string::npos);
	REQUIRE(dumps.mlir.find("func.func @specnf_ir__specialized(") != std::string::npos);
	REQUIRE(dumps.mlir.find("call @specnf_ir__generic(") != std::string::npos);
	REQUIRE(dumps.mlir.find("call @specnf_ir__specialized(") != std::string::npos);
	REQUIRE(dumps.mlir.find("cf.cond_br") != std::string::npos);

	// The specialized variant must have a *reduced* signature: the
	// specialized index (1 = `exp`) is dropped, so only `base` (one
	// i64) remains. The dominant value 5 becomes a local constant
	// inside the specialized body, not a dead function parameter.
	REQUIRE(dumps.mlir.find("func.func @specnf_ir__specialized(%arg0: i64) ->") != std::string::npos);
	REQUIRE(dumps.mlir.find("func.func @specnf_ir__generic(%arg0: i64, %arg1: i64) ->") != std::string::npos);

	// The cond_br and the two variant calls must live inside the
	// dispatcher function, not inside `execute`.
	const auto executeStart = dumps.mlir.find("func.func @execute");
	const auto executeEnd = dumps.mlir.find("func.func @", executeStart + 1);
	REQUIRE(executeStart != std::string::npos);
	const std::string executeBody = dumps.mlir.substr(executeStart, executeEnd - executeStart);
	REQUIRE(executeBody.find("cf.cond_br") == std::string::npos);
	REQUIRE(executeBody.find("call @specnf_ir__generic") == std::string::npos);
	REQUIRE(executeBody.find("call @specnf_ir__specialized") == std::string::npos);
	REQUIRE(executeBody.find("call @specnf_ir(") != std::string::npos);
}

TEST_CASE("SpecializedNautilusFunction: multi-index specialization over (base, exp)") {
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 0, 1> spec {"specnf_multi", specNF_pow};
	for (int i = 0; i < 2048; ++i) {
		ValueProfile<int64_t>::profile(&spec.profile<0>(), 2);
		ValueProfile<int64_t>::profile(&spec.profile<1>(), 5);
	}
	REQUIRE(spec.profile<0>().isStable());
	REQUIRE(spec.profile<1>().isStable());

	auto engine = nautilus::testing::makeEngine("mlir", [&](engine::Options& options) {
		options.setOption("mlir.enableIntrinsics", true);
		options.setOption("mlir.enableMultithreading", false);
	});
	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};
	auto fn = engine.registerFunction(wrapper);

	// Fully-specialized path: both args match.
	REQUIRE(fn(2, 5) == 32);
	// Only base matches -> generic.
	REQUIRE(fn(2, 4) == 16);
	// Only exp matches -> generic.
	REQUIRE(fn(3, 5) == 243);
	// Neither matches -> generic.
	REQUIRE(fn(4, 3) == 64);
	REQUIRE(fn(7, 0) == 1);
}

TEST_CASE("SpecializedNautilusFunction IR: multi-index drops all specialized args from signature") {
	// Fully specialized over both arguments: the specialized variant
	// should have a zero-parameter signature because every argument is
	// replaced by a local constant inside the body.
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 0, 1> spec {"specnf_full", specNF_pow};
	for (int i = 0; i < 2048; ++i) {
		ValueProfile<int64_t>::profile(&spec.profile<0>(), 2);
		ValueProfile<int64_t>::profile(&spec.profile<1>(), 5);
	}
	REQUIRE(spec.profile<0>().isStable());
	REQUIRE(spec.profile<1>().isStable());

	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};
	const auto dumps = readCompiledIR("specnf_full", wrapper);
	INFO("== Pre-opt MLIR for fully-specialized SpecializedNautilusFunction:\n" << dumps.mlir);

	// The specialized variant now takes *no* arguments.
	REQUIRE(dumps.mlir.find("func.func @specnf_full__specialized() ->") != std::string::npos);
	// Dispatcher calls it with an empty arg list.
	REQUIRE(dumps.mlir.find("call @specnf_full__specialized() :") != std::string::npos);
	// Generic variant still has the original two-parameter signature.
	REQUIRE(dumps.mlir.find("func.func @specnf_full__generic(%arg0: i64, %arg1: i64) ->") != std::string::npos);
}

TEST_CASE("SpecializedNautilusFunction: multi-index unstable profile emits two profile updates") {
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 0, 1> spec {"specnf_multi_empty", specNF_pow};

	auto engine = nautilus::testing::makeEngine("mlir", [&](engine::Options& options) {
		options.setOption("mlir.enableIntrinsics", true);
		options.setOption("mlir.enableMultithreading", false);
	});
	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};
	auto fn = engine.registerFunction(wrapper);

	for (int i = 0; i < 2048; ++i) {
		REQUIRE(fn(2, 5) == 32);
	}
	REQUIRE(spec.profile<0>().isStable());
	REQUIRE(spec.profile<0>().dominant() == 2);
	REQUIRE(spec.profile<1>().isStable());
	REQUIRE(spec.profile<1>().dominant() == 5);
}

TEST_CASE("SpecializedNautilusFunction: re-register after profile stabilizes emits specialized variant") {
	SpecializedNautilusFunction<val<int64_t>(val<int64_t>, val<int64_t>), 1> spec {"specnf_reg", specNF_pow};

	auto engine = nautilus::testing::makeEngine("mlir", [&](engine::Options& options) {
		options.setOption("mlir.enableIntrinsics", true);
		options.setOption("mlir.enableMultithreading", false);
	});
	std::function<val<int64_t>(val<int64_t>, val<int64_t>)> wrapper = [&](val<int64_t> b, val<int64_t> e) {
		return spec(b, e);
	};

	// First registration: profile is empty, the dispatcher body goes
	// through the profile-update + generic path. Driving the kernel with
	// a dominant `exp` value makes the profile stable.
	auto fn1 = engine.registerFunction(wrapper);
	REQUIRE_FALSE(spec.profile<0>().isStable());
	for (int i = 0; i < 4096; ++i) {
		REQUIRE(fn1(2, 5) == 32);
	}
	REQUIRE(spec.profile<0>().isStable());
	REQUIRE(spec.profile<0>().dominant() == 5);

	// Second registration: the profile is now stable, so re-tracing the
	// dispatcher picks up the if/else into the specialized variant. The
	// compiled module should expose a `__specialized` symbol that the
	// first compilation did not have.
	const auto dumps = readCompiledIR("specnf_reg", wrapper);
	INFO("== Pre-opt MLIR for re-registered SpecializedNautilusFunction:\n" << dumps.mlir);
	REQUIRE(dumps.mlir.find("specnf_reg__generic") != std::string::npos);
	REQUIRE(dumps.mlir.find("specnf_reg__specialized") != std::string::npos);
	REQUIRE(dumps.mlir.find("call @specnf_reg__specialized") != std::string::npos);
	REQUIRE(dumps.mlir.find("call @specnf_reg__generic") != std::string::npos);
	REQUIRE(dumps.mlir.find("cf.cond_br") != std::string::npos);

	auto fn2 = engine.registerFunction(wrapper);
	REQUIRE(fn2(2, 5) == 32);  // specialized branch
	REQUIRE(fn2(3, 5) == 243); // specialized branch
	REQUIRE(fn2(3, 4) == 81);  // generic branch
	REQUIRE(fn2(7, 0) == 1);   // generic branch
}

} // namespace nautilus::engine
#endif
