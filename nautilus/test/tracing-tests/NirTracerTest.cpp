// The direct NIR tracer (tracing/nir/NirTracer): golden pre-SSA dumps of a small corpus and agreement with the 0.3
// tracer after conversion.
#include "NirTracerCorpus.hpp"
#include <catch2/catch_all.hpp>
#include <cstdlib>
#include <fstream>

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
