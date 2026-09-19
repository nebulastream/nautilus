#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>

#if defined(ENABLE_TRACING) && defined(ENABLE_BC_BACKEND)

namespace nautilus::engine {

namespace {

val<int32_t> statsAddOne(val<int32_t> x) {
	return x + 1;
}

// A larger workload than statsAddOne so that register reuse has
// something visible to do; without enough live ranges the allocator's
// effect on the register count would be invisible (a single arithmetic
// op already maps 1:1 to its register).
val<int32_t> statsLoopSum(val<int32_t> n) {
	val<int32_t> sum = val<int32_t>(0);
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = sum + i;
	}
	return sum;
}

} // namespace

TEST_CASE("CompilationStatistics: bytecode backend reports code size") {
	Options options;
	options.setOption("engine.backend", std::string {"bc"});

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(5) == 6);

	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(stats->contains("bc.instructions"));
	REQUIRE(stats->contains("bc.codeSize.bytes"));
	REQUIRE(std::get<int64_t>(*stats->find("bc.instructions")) > 0);
	REQUIRE(std::get<int64_t>(*stats->find("bc.codeSize.bytes")) > 0);
	// Register-allocator stats are always emitted by the BC backend,
	// regardless of whether the allocator itself is enabled.
	REQUIRE(stats->contains("bc.registers.total"));
	REQUIRE(stats->contains("bc.registers.max"));
	REQUIRE(stats->contains("bc.registerAllocator.enabled"));
	REQUIRE(std::get<int64_t>(*stats->find("bc.registers.total")) > 0);
	REQUIRE(std::get<int64_t>(*stats->find("bc.registers.max")) > 0);
}

TEST_CASE("CompilationStatistics: bc.registerAllocator option shrinks the register file") {
	auto compileWithAllocator = [](bool enabled) {
		Options options;
		options.setOption("engine.backend", std::string {"bc"});
		options.setOption("bc.registerAllocator", enabled);
		auto engine = std::make_unique<NautilusEngine>(options);
		auto fn = engine->registerFunction(statsLoopSum);
		// Run the function so we know the produced bytecode is correct
		// even with the allocator enabled.
		REQUIRE(fn(10) == 45);
		auto stats = fn.getStatistics();
		REQUIRE(stats != nullptr);
		REQUIRE(std::get<std::string>(*stats->find("bc.registerAllocator.enabled")) ==
		        std::string(enabled ? "true" : "false"));
		auto maxRegs = std::get<int64_t>(*stats->find("bc.registers.max"));
		// Hold the engine alive until after the stats are read.
		(void) engine;
		return maxRegs;
	};

	auto withAlloc = compileWithAllocator(true);
	auto withoutAlloc = compileWithAllocator(false);

	INFO("registers with allocator=" << withAlloc << " without=" << withoutAlloc);
	REQUIRE(withAlloc <= withoutAlloc);
	REQUIRE(withAlloc < withoutAlloc); // for this workload reuse is always possible
}

} // namespace nautilus::engine

#endif
