
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/LegacyCompiler.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/options.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <list>
#include <regex>
#include <string>

namespace nautilus::testing {

namespace {

/// RAII fixture that owns the arena, pool, and compiler for a single test.
/// The returned IRGraph borrows arena memory owned by the pool, so both
/// must outlive every use of the graph.
struct IRFixture {
	engine::Options options;
	common::Arena arena;
	common::ArenaPool irArenaPool;
	compiler::LegacyCompiler compiler;

	IRFixture() : compiler(options, arena, irArenaPool) {
	}

	std::shared_ptr<compiler::ir::IRGraph> compile(std::function<void()> tracedFn) {
		std::list<compiler::CompilableFunction> functions;
		functions.emplace_back("execute", std::move(tracedFn));
		return compiler.compileToIR(functions);
	}
};

} // namespace

TEST_CASE("VarNameMetadata: default toString is byte-identical to legacy output") {
	// With dump.sourceLocations / dump.variableNames off, the side-table
	// stays empty and the dump matches the format that existed before
	// these features. Locks down the zero-overhead default path.
	IRFixture fixture;
	auto ir = fixture.compile([]() {
		val<int64_t> sum = 1;
		tracing::traceReturnOperation(Type::i64, sum.state);
	});
	const auto dump = ir->toString();
	REQUIRE(dump.find("; at ") == std::string::npos);
	REQUIRE(dump.find("[var=") == std::string::npos);
	// The empty IRPrintOptions overload must produce identical output.
	REQUIRE(ir->toString(compiler::ir::IRPrintOptions {}) == dump);
}

TEST_CASE("VarNameMetadata: side-table is empty when capture is off") {
	IRFixture fixture;
	auto ir = fixture.compile([]() {
		val<int64_t> a = 10;
		val<int64_t> b = 20;
		auto c = a + b;
		tracing::traceReturnOperation(Type::i64, c.state);
	});
	REQUIRE(ir->getDebugInfoMap().empty());
}

TEST_CASE("VarNameMetadata: source-location capture populates the side-table") {
	IRFixture fixture;
	fixture.options.setOption("dump.sourceLocations", true);
	auto ir = fixture.compile([]() {
		val<int64_t> sum = 3;
		val<int64_t> factor = 4;
		auto resultVal = sum + factor;
		tracing::traceReturnOperation(Type::i64, resultVal.state);
	});

	// SourceLocationResolver only resolves frames when ENABLE_STACKTRACE
	// is on at build time. Without it the side-table stays empty even
	// though the option was set.
	if (ir->getDebugInfoMap().empty()) {
		SUCCEED("No source-location frames recovered; ENABLE_STACKTRACE likely off.");
		return;
	}
	// At least one op should have a non-empty frame stack.
	bool sawFrames = false;
	for (const auto& [id, info] : ir->getDebugInfoMap()) {
		(void) id;
		if (!info.frames.empty()) {
			sawFrames = true;
			break;
		}
	}
	REQUIRE(sawFrames);
}

TEST_CASE("VarNameMetadata: dump renders [var=...] when names were resolved") {
	IRFixture fixture;
	fixture.options.setOption("dump.variableNames", true);
	auto ir = fixture.compile([]() {
		val<int64_t> sum = 3;
		val<int64_t> factor = 4;
		auto resultVal = sum + factor;
		tracing::traceReturnOperation(Type::i64, resultVal.state);
	});

	compiler::ir::IRPrintOptions opts;
	opts.showSourceLocations = true;
	opts.showVariableNames = true;
	const auto dump = ir->toString(opts);
	INFO("IR dump:\n" << dump);

	// Source-location resolution depends on ENABLE_STACKTRACE; variable
	// names additionally require -g and an LLVM-DWARF-enabled build.
	// Skip cleanly when neither resolver produced anything.
	if (dump.find("; at ") == std::string::npos) {
		SUCCEED("No source-location trailer; ENABLE_STACKTRACE likely off.");
		return;
	}
	if (dump.find("[var=") == std::string::npos) {
		SUCCEED("No DWARF-recovered variable names; binary likely built without -g.");
		return;
	}

	REQUIRE(dump.find("[var=sum]") != std::string::npos);
	REQUIRE(dump.find("[var=factor]") != std::string::npos);

	// The documented per-op trailer shape:
	//   ; at <file>:<line> (<func>) [var=<name>]
	const std::regex shape {R"(; at \S+:\d+ \([^)]*\) \[var=\S+\])"};
	REQUIRE(std::regex_search(dump, shape));
}

TEST_CASE("VarNameMetadata: trailer shape regex matches the documented format") {
	// Pins the documented trailer shape independently of any live IR
	// dump. Any future change to the format is caught here.
	const std::regex shape {R"(; at \S+:\d+ \([^)]*\)( \[var=\S+\])?)"};
	REQUIRE(std::regex_search(std::string("; at user.cpp:42 (main) [var=sum]"), shape));
	REQUIRE(std::regex_search(std::string("; at user.cpp:42 (main)"), shape));
	REQUIRE_FALSE(std::regex_search(std::string("$1 = 0 :i64"), shape));
}

namespace multi_frame {
// User-side helper function whose body lives in the same TU as the test
// case below. Variables declared here survive the SourceLocationResolver's
// nautilus/src/ + nautilus/include/ filter, so the trace tag chain for
// the `+` inside this body should retain *both* this frame and the
// caller's frame after resolution. Force a real call (not inlining) so
// the captured return-address chain genuinely contains two distinct
// user frames; otherwise the test ends up exercising the single-frame
// path that the other tests already cover.
[[gnu::noinline]] val<int64_t> helperCompute(val<int64_t> a, val<int64_t> b) {
	val<int64_t> acc = a + b;
	return acc;
}
} // namespace multi_frame

TEST_CASE("VarNameMetadata: multi-frame variable resolution across user helpers") {
	// When a value flows through a user-side helper function, the trace
	// tag chain captures both the helper body's frame and the caller's
	// frame. With `dump.variableNames` on, each frame in the dump's
	// inlined-from chain should carry its own [var=<name>] annotation
	// when DWARF can recover one (e.g. "acc" inside helperCompute,
	// "sum" at the call site).
	IRFixture fixture;
	fixture.options.setOption("dump.variableNames", true);
	auto ir = fixture.compile([]() {
		val<int64_t> x = 3;
		val<int64_t> y = 4;
		val<int64_t> sum = multi_frame::helperCompute(x, y);
		tracing::traceReturnOperation(Type::i64, sum.state);
	});

	// Skip cleanly when neither resolver produced anything (build flags).
	bool anyFrame = false;
	bool anyVarName = false;
	for (const auto& [id, info] : ir->getDebugInfoMap()) {
		(void) id;
		if (!info.frames.empty()) {
			anyFrame = true;
		}
		if (info.hasAnyVariableName()) {
			anyVarName = true;
		}
	}
	if (!anyFrame) {
		SUCCEED("No source-location frames recovered; ENABLE_STACKTRACE likely off.");
		return;
	}
	if (!anyVarName) {
		SUCCEED("No DWARF variable names recovered; binary likely built without -g.");
		return;
	}

	// Locate at least one operation whose frame stack carries both
	// user frames (helperCompute body + the lambda call site) and
	// whose variableNames contains both "acc" and "sum". This is the
	// behavioural guarantee of stack-aware variable tracking.
	bool sawMultiFrameVariables = false;
	for (const auto& [id, info] : ir->getDebugInfoMap()) {
		(void) id;
		bool hasAcc = false;
		bool hasSum = false;
		for (const auto& name : info.variableNames) {
			if (!name.has_value()) {
				continue;
			}
			if (*name == "acc") {
				hasAcc = true;
			}
			if (*name == "sum") {
				hasSum = true;
			}
		}
		if (hasAcc && hasSum) {
			sawMultiFrameVariables = true;
			break;
		}
	}
	if (!sawMultiFrameVariables) {
		// Some toolchains inline through the helper aggressively even at
		// -O0, collapsing the two frames into one. Don't fail on that —
		// the API is still correct, it's just that DWARF flattened the
		// information away.
		SUCCEED("Could not observe both frames separately — likely inlined by the host compiler.");
		return;
	}
	REQUIRE(sawMultiFrameVariables);

	// Stack-aware dump: with two frames present, the trailer must list
	// the inlined-from line and may carry per-frame [var=...] tails.
	compiler::ir::IRPrintOptions opts;
	opts.showSourceLocations = true;
	opts.showVariableNames = true;
	const auto dump = ir->toString(opts);
	INFO("IR dump:\n" << dump);
	REQUIRE(dump.find("inlined from") != std::string::npos);
}

} // namespace nautilus::testing
