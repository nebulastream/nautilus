// The tracing diagnostics of M2.4 (docs/diagnostics.md): every code has a test that triggers it.

#include "ExecutionTest.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/config.hpp"
#include "nautilus/control_flow.hpp"
#include "nautilus/region.hpp"
#include "nautilus/static.hpp"
#include "nautilus/tracing/TracingDiagnostics.hpp"
#include "nautilus/val.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>
#include <string>

namespace nautilus::engine {

#ifdef ENABLE_TRACING

TEST_CASE("Diagnostics: the text and the JSON form", "[diagnostics]") {
	tracing::TracingDiagnostic d {
	    "NAUT-T001", "constant loop", "the loop \"x\" is constant", "use static_val", "Query.cpp", 42, "f"};
	CHECK(d.toText() == "NAUT-T001 constant loop at Query.cpp:42: the loop \"x\" is constant; use static_val");
	CHECK(d.toJson() ==
	      "{\"code\":\"NAUT-T001\",\"title\":\"constant loop\",\"message\":\"the loop \\\"x\\\" is "
	      "constant\",\"hint\":\"use static_val\",\"file\":\"Query.cpp\",\"line\":42,\"function\":\"f\"}");
	tracing::TracingDiagnostic unlocated {"NAUT-T002", "path explosion", "m", "", {}, 0, {}};
	CHECK(unlocated.toText() == "NAUT-T002 path explosion at <unknown location>: m");
}

namespace {

[[maybe_unused]] std::string nirBackend() {
	for (const auto* name : {"tbc", "asmjit", "cpp", "mlir"}) {
		for (const auto& available : testing::availableBackends()) {
			if (available == name) {
				return available;
			}
		}
	}
	return {};
}

// a bound the compiler cannot see through, so that it does not unroll the loop into distinct call sites
int constantLoopCount = 64;

[[maybe_unused]] val<int64_t> constantLoop(val<int64_t> x) {
	val<int64_t> r = x;
#pragma GCC unroll 1
	for (int i = 0; i < constantLoopCount; i++) {
		// a plain C++ loop that repeats an operation whose result dies at once: the call stack and the alive values are
		// the same on every pass, so the tracer takes the repetition for a loop
		val<int64_t> unused = r + 1;
	}
	return r;
}

std::vector<val<int64_t>> escapedStorage;

[[maybe_unused]] val<int64_t> escapingValueKept(val<int64_t> x) {
	region([&] { escapedStorage.push_back(x + 1); });
	return x;
}

[[maybe_unused]] val<int64_t> breakOutsideLoop(val<int64_t> x) {
	If(x > 0, [&] { Break(); });
	return x;
}

[[maybe_unused]] val<int64_t> breakThroughRegion(val<int64_t> n) {
	val<int64_t> i = 0;
	While([&] { return i < n; },
	      [&] {
		      region([&] { Break(); });
		      i = i + 1;
	      });
	return i;
}

[[maybe_unused]] val<int64_t> manyBranches(val<int64_t> x) {
	val<int64_t> r = 0;
	for (static_val<int> i = 0; i < 40; i++) {
		if (x > i) {
			r = r + 1;
		}
	}
	return r;
}

template <typename F>
std::string compileError(F function, const std::function<void(Options&)>& tweak = {}) {
	auto engine = testing::makeEngine(nirBackend(), tweak);
	try {
		auto compiled = engine.registerFunction(function);
		compiled(1);
	} catch (const std::exception& e) {
		return e.what();
	}
	return {};
}

} // namespace

TEST_CASE("Diagnostics: NAUT-T001 constant loop", "[diagnostics]") {
	if (nirBackend().empty()) {
		SKIP("No compilation backend available");
	}
	const auto message = compileError(constantLoop);
	INFO(message);
	CHECK(message.find("NAUT-T001 constant loop at ") == 0);
	CHECK(message.find(".cpp:") != std::string::npos);
	CHECK(message.find("static_val") != std::string::npos);
}

TEST_CASE("Diagnostics: NAUT-T002 path explosion and NAUT-T003 replay share", "[diagnostics]") {
	if (nirBackend().empty()) {
		SKIP("No compilation backend available");
	}
	auto engine = testing::makeEngine(nirBackend(), [](Options& options) {
		options.setOption("tracing.diagnostics.maxPasses", 10);
		options.setOption("tracing.diagnostics.replayedOperations", 50);
	});
	auto compiled = engine.registerFunction(manyBranches);
	REQUIRE(compiled(5) == 5);
	auto stats = compiled.getStatistics();
	REQUIRE(stats != nullptr);
	const auto* count = stats->find("diagnostics.count");
	REQUIRE(count != nullptr);
	CHECK(std::get<int64_t>(*count) >= 1);
	std::string codes;
	for (int64_t i = 0; i < std::get<int64_t>(*count); ++i) {
		codes += std::get<std::string>(*stats->find("diagnostics." + std::to_string(i) + ".code")) + " ";
	}
	INFO(codes);
	CHECK(codes.find("NAUT-T002") != std::string::npos);
	// 40 chained branches are one first run and 80 re-runs, which take most of the tracing time
	CHECK(codes.find("NAUT-T003") != std::string::npos);
	const auto* message = stats->find("diagnostics.0.message");
	REQUIRE(message != nullptr);
	CHECK(std::get<std::string>(*message).find("passes") != std::string::npos);
}

TEST_CASE("Diagnostics: no warning below the thresholds", "[diagnostics]") {
	if (nirBackend().empty()) {
		SKIP("No compilation backend available");
	}
	auto engine = testing::makeEngine(nirBackend());
	auto compiled = engine.registerFunction(manyBranches);
	REQUIRE(compiled(5) == 5);
	auto stats = compiled.getStatistics();
	REQUIRE(stats != nullptr);
	CHECK(std::get<int64_t>(*stats->find("diagnostics.count")) == 0);
}

TEST_CASE("Diagnostics: NAUT-T004 a value escaping a region", "[diagnostics]") {
	if (nirBackend().empty()) {
		SKIP("No compilation backend available");
	}
	const auto message = compileError(escapingValueKept);
	INFO(message);
	CHECK(message.find("NAUT-T004 ") == 0);
	CHECK(message.find("TracingDiagnosticsTest.cpp:") != std::string::npos); // the region() call
	escapedStorage.clear();
}

TEST_CASE("Diagnostics: NAUT-T005 an exit that does not fit", "[diagnostics]") {
	if (nirBackend().empty()) {
		SKIP("No compilation backend available");
	}
	SECTION("Break outside a loop") {
		const auto message = compileError(breakOutsideLoop);
		INFO(message);
		CHECK(message.find("NAUT-T005 ") == 0);
	}
	SECTION("Break through a region") {
		const auto message = compileError(breakThroughRegion);
		INFO(message);
		CHECK(message.find("NAUT-T005 ") == 0);
		CHECK(message.find("region()") != std::string::npos);
	}
}

TEST_CASE("Diagnostics: engine.diagnostics=json writes the error as it is raised", "[diagnostics]") {
	if (nirBackend().empty()) {
		SKIP("No compilation backend available");
	}
	for (const auto* mode : {"off", "text", "json"}) {
		DYNAMIC_SECTION(mode) {
			const auto message = compileError(breakOutsideLoop, [&](Options& options) {
				options.setOption("engine.diagnostics", std::string(mode));
			});
			CHECK(message.find("NAUT-T005 ") == 0);
		}
	}
}

#endif // ENABLE_TRACING

} // namespace nautilus::engine
