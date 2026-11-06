// The allocation counter of docs/plans/nautilus-0.4/12-validation.md: no heap allocation per recorded operation in a
// steady-state trace of the direct NIR tracer.
#include "AllocationCounter.hpp"
#include "NirTracerCorpus.hpp"
#include <catch2/catch_all.hpp>

using namespace nautilus;
using namespace nautilus::engine;
using namespace nautilus::testing::nirtracer;

TEST_CASE("a trace in steady state makes no heap allocation per recorded operation", "[nir-tracer][allocations]") {
	struct Run {
		uint64_t operations;
		uint64_t allocations;
	};
	auto measure = [](const std::function<void()>& wrapper) {
		common::Arena arena;
		traceDirect(wrapper, arena); // the first trace warms the caches the tracer keeps
		arena.softReset();
		uint64_t operations = 0;
		const auto before = testing::allocations();
		auto module = traceDirect(wrapper, arena, &operations);
		return Run {operations, testing::allocations() - before};
	};
	// 1024 distinct call sites against a function of 4 operations: the allocations the longer trace makes beyond the
	// short one's are the per-operation cost
	const auto small = measure(engine::details::createFunctionWrapper(linearSum));
	const auto large = measure(engine::details::createFunctionWrapper(distinctSites1024));
	INFO("small: " << small.operations << " operations, " << small.allocations << " allocations");
	INFO("large: " << large.operations << " operations, " << large.allocations << " allocations");
	REQUIRE(large.operations > small.operations + 1000);
	const double perOperation =
	    static_cast<double>(large.allocations - std::min(large.allocations, small.allocations)) /
	    static_cast<double>(large.operations - small.operations);
	INFO("allocations per operation: " << perOperation);
	REQUIRE(perOperation < 0.1);
}
