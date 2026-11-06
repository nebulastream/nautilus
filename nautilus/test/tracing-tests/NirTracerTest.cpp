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
