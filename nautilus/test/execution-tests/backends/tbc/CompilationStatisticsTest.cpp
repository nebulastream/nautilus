#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_all.hpp>

#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)

namespace nautilus::engine {

namespace {

val<int32_t> statsAddOne(val<int32_t> x) {
	return x + 1;
}

} // namespace

TEST_CASE("CompilationStatistics: tbc backend reports code size") {
	Options options;
	options.setOption("engine.backend", std::string {"tbc"});

	NautilusEngine engine(options);
	auto fn = engine.registerFunction(statsAddOne);
	REQUIRE(fn(5) == 6);

	auto stats = fn.getStatistics();
	REQUIRE(stats != nullptr);
	REQUIRE(stats->contains("tbc.instructions"));
	REQUIRE(stats->contains("tbc.codeSize.bytes"));
	REQUIRE(stats->contains("tbc.registers.max"));
	REQUIRE(stats->contains("tbc.dispatch"));
	REQUIRE(std::get<int64_t>(*stats->find("tbc.instructions")) > 0);
	REQUIRE(std::get<int64_t>(*stats->find("tbc.codeSize.bytes")) > 0);
	REQUIRE(std::get<int64_t>(*stats->find("tbc.registers.max")) > 0);
	// The recorded dispatch mode is the one actually selected after clamping
	// to what this build supports.
	const auto dispatch = std::get<std::string>(*stats->find("tbc.dispatch"));
	REQUIRE((dispatch == "tailcall" || dispatch == "goto" || dispatch == "switch"));
}

} // namespace nautilus::engine

#endif
