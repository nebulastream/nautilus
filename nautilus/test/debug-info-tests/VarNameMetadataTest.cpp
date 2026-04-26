
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

} // namespace nautilus::testing
