// The direct NIR tracer (tracing/nir/NirTracer): golden pre-SSA dumps of a small corpus and agreement with the 0.3
// tracer after conversion.
#include "NirTracerCorpus.hpp"
#include "nautilus/region.hpp"
#include <algorithm>
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <fstream>
#include <set>

using namespace nautilus;
using namespace nautilus::testing::nirtracer;

TEST_CASE("the direct tracer's pre-SSA output verifies, converts and matches the golden dump", "[nir-tracer][golden]") {
	for (const auto& entry : corpus()) {
		DYNAMIC_SECTION(entry.name) {
			common::Arena arena;
			auto module = traceDirect(entry.wrapper, arena);
			REQUIRE(diagnostics(module).empty());
			const auto text = nir::print(module);
			INFO(text);
			if (!entry.directOnly) { // the conversion does not handle extension instructions yet
				REQUIRE_NOTHROW(conversion::convert(module));
				REQUIRE(diagnostics(conversion::convert(module)).empty());
			}
			if (std::getenv("NAUTILUS_UPDATE_GOLDEN") != nullptr) {
				std::ofstream(goldenPath(entry.name)) << text;
			} else {
#ifdef __clang__
				// the order in which a compiler evaluates the operands of `a + b * c` is its own, and the dumps follow
				// it (as the reference dumps of nautilus-tracing-tests, which are Clang only)
				REQUIRE(text == readFile(goldenPath(entry.name)));
#endif
			}
		}
	}
}

TEST_CASE("the direct tracer and the 0.3 tracer converge to the same SSA", "[nir-tracer][equivalence]") {
	for (const auto& entry : corpus()) {
		if (entry.directOnly) {
			continue;
		}
		DYNAMIC_SECTION(entry.name) {
			common::Arena arena03, arenaDirect;
			auto direct = conversion::convert(traceDirect(entry.wrapper, arenaDirect));
			auto reference = conversion::convert(trace03(entry.wrapper, arena03));
			REQUIRE(nir::print(direct) == nir::print(reference));
		}
	}
}

TEST_CASE("runtime calls share their callee, signature and pointer entries", "[nir-tracer][calls]") {
	// five calls of two functions: the callee cache of the tracer must hand out the ids interning would
	common::Arena arena, arena03;
	auto module = traceDirect(engine::details::createFunctionWrapper(runtimeCalls), arena);
	REQUIRE(diagnostics(module).empty());
	REQUIRE(module.callees.size() == 2);
	REQUIRE(module.pointers.size() == 2);
	REQUIRE(module.signatures.size() == 3); // the two callees and the function itself
	REQUIRE(nir::print(conversion::convert(module)) ==
	        nir::print(conversion::convert(trace03(engine::details::createFunctionWrapper(runtimeCalls), arena03))));
}

TEST_CASE("a function that needs a hoisted declaration keeps the arguments of its calls", "[nir-tracer][calls]") {
	// the tracer writes the blocks in one pass and starts over, hoisting, when a declaration comes after a use; the
	// calls renumbered by the first attempt must be back as they were
	common::Arena arena, arena03;
	auto module = traceDirect(engine::details::createFunctionWrapper(guardedCalls), arena);
	REQUIRE(diagnostics(module).empty());
	REQUIRE(nir::print(conversion::convert(module)) ==
	        nir::print(conversion::convert(trace03(engine::details::createFunctionWrapper(guardedCalls), arena03))));
}

// ---- debug sections (D14, 13 §4.9)
// -----------------------------------------------------------------------------------

namespace {

val<int32_t> nestedRegions(val<int32_t> n) {
	val<int32_t> acc = 0;
	region("outer", [&]() {
		for (val<int32_t> i = 0; i < n; i = i + 1) {
			region("inner", [&]() { acc = acc + i; });
		}
	});
	return acc;
}

nir::Module traceWith(const engine::Options& options, common::Arena& arena) {
	std::list<compiler::CompilableFunction> functions;
	functions.emplace_back("execute", engine::details::createFunctionWrapper(nestedRegions));
	return std::move(tracing::nirtrace::NirTracer::Trace(functions, options, arena).module);
}

std::string regionName(const nir::Module& m, uint32_t region) {
	return region == nir::NONE || m.debugRegions[region].nameId == nir::NONE
	           ? std::string()
	           : std::string(m.debugName(m.debugRegions[region].nameId));
}

} // namespace

TEST_CASE("a trace records no debug data unless a debug option asks for it", "[nir-tracer][debug]") {
	common::Arena arena;
	const auto module = traceWith(engine::Options {}, arena);
	CHECK_FALSE(module.hasDebugData());
	CHECK(module.debugRegions.empty());
	CHECK(module.debugInstRegions.empty());
	CHECK(module.debugFunctions.empty());
}

TEST_CASE("the regions of a trace reach the SSA module", "[nir-tracer][debug]") {
	for (const auto* option : {"debug", "perf", "perf.sample"}) {
		DYNAMIC_SECTION(option) {
			engine::Options options;
			options.setOption(option, true);
			common::Arena arena;
			const auto pre = traceWith(options, arena);
			REQUIRE(diagnostics(pre).empty());

			// the region table: two regions, the inner one nested in the outer one, and the function
			REQUIRE(pre.debugRegions.size() == 2);
			REQUIRE(pre.debugFunctions.size() == 1);
			REQUIRE(pre.debugInstRegions.size() == pre.insts.size());
			REQUIRE(pre.debugInstLocations.size() == pre.insts.size());
			uint32_t outer = nir::NONE, inner = nir::NONE;
			for (uint32_t r = 0; r < pre.debugRegions.size(); ++r) {
				(regionName(pre, r) == "outer" ? outer : inner) = r;
			}
			REQUIRE(outer != nir::NONE);
			REQUIRE(regionName(pre, inner) == "inner");
			CHECK(pre.debugRegions[outer].parent == nir::NONE);
			CHECK(pre.debugRegions[inner].parent == outer);
			// the call sites are in this file
			REQUIRE(pre.debugRegions[inner].location != nir::NONE);
			const auto file = pre.debugName(pre.debugLocations[pre.debugRegions[inner].location].file);
			CHECK(std::string(file).find("NirTracerTest") != std::string::npos);

			// the conversion writes its own per-instruction sections, parallel to its instructions
			const auto ssa = conversion::convert(pre);
			REQUIRE(diagnostics(ssa).empty());
			REQUIRE(ssa.debugInstRegions.size() == ssa.insts.size());
			REQUIRE(ssa.debugInstLocations.size() == ssa.insts.size());
			std::set<std::string> seen;
			for (uint32_t id = 0; id < ssa.functions[0].instCount; ++id) {
				seen.insert(regionName(ssa, ssa.regionOf(0, id)));
			}
			CHECK(seen.contains("")); // the code outside the regions
			CHECK(seen.contains("outer"));
			CHECK(seen.contains("inner"));

			// the printer says where every instruction is in the text
			nir::PrintLines lines;
			const auto text = nir::print(ssa, lines);
			CHECK(text == nir::print(ssa));
			REQUIRE(lines.instruction.size() == ssa.insts.size());
			const auto textLines = static_cast<uint32_t>(std::count(text.begin(), text.end(), '\n'));
			for (uint32_t id = 0; id < ssa.insts.size(); ++id) {
				CHECK(lines.instruction[id] >= 1);
				CHECK(lines.instruction[id] <= textLines);
			}
			CHECK(lines.function[0] < lines.block[0]);
			CHECK(lines.block[0] <= lines.instruction[ssa.blocks[0].first + ssa.blocks[0].argCount]);
		}
	}
}
