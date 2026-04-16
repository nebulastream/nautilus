#include "nautilus/CompilationStatistics.hpp"
#include <catch2/catch_all.hpp>
#include <chrono>
#include <exception>
#include <thread>

using namespace nautilus::compiler;

TEST_CASE("CompilationStatistics: set and lookup round-trip") {
	CompilationStatistics stats;
	REQUIRE(stats.empty());
	stats.set("tracing.ms", 1.5);
	stats.set("ir.operations", int64_t {42});
	stats.set("backend.name", std::string {"mlir"});

	REQUIRE(stats.size() == 3);
	REQUIRE(stats.contains("tracing.ms"));
	REQUIRE(stats.contains("ir.operations"));
	REQUIRE_FALSE(stats.contains("missing"));

	const auto* traceValue = stats.find("tracing.ms");
	REQUIRE(traceValue != nullptr);
	REQUIRE(std::holds_alternative<double>(*traceValue));
	REQUIRE(std::get<double>(*traceValue) == Catch::Approx(1.5));

	const auto* backendValue = stats.find("backend.name");
	REQUIRE(backendValue != nullptr);
	REQUIRE(std::get<std::string>(*backendValue) == "mlir");
}

TEST_CASE("CompilationStatistics: set overwrites preserving order") {
	CompilationStatistics stats;
	stats.set("a", int64_t {1});
	stats.set("b", int64_t {2});
	stats.set("a", int64_t {10});

	// Expect insertion order: a, b (updating a should not reorder).
	std::vector<std::string> keys;
	for (const auto& [k, _] : stats) {
		keys.push_back(k);
	}
	REQUIRE(keys == std::vector<std::string> {"a", "b"});
	REQUIRE(std::get<int64_t>(*stats.find("a")) == 10);
}

TEST_CASE("CompilationStatistics: add increments counter and creates when missing") {
	CompilationStatistics stats;
	stats.add("ops", 3);
	stats.add("ops", 4);
	REQUIRE(std::get<int64_t>(*stats.find("ops")) == 7);

	stats.set("duration.ms", 1.0);
	REQUIRE_THROWS_AS(stats.add("duration.ms", 1), std::exception);
}

TEST_CASE("CompilationStatistics: recordTimingMs captures non-negative duration") {
	CompilationStatistics stats;
	const auto start = std::chrono::steady_clock::now();
	// Ensure the delta is greater than zero without flaking on fast CI
	// hardware — a 1 ms sleep is the minimum resolution that reliably
	// produces a positive duration across platforms.
	std::this_thread::sleep_for(std::chrono::milliseconds(1));
	stats.recordTimingMs("phase.ms", start);

	const auto* val = stats.find("phase.ms");
	REQUIRE(val != nullptr);
	REQUIRE(std::holds_alternative<double>(*val));
	REQUIRE(std::get<double>(*val) > 0.0);
}

TEST_CASE("CompilationStatistics: merge namespaces nested entries") {
	CompilationStatistics inner;
	inner.set("ms", 2.5);
	inner.set("codeSize.bytes", int64_t {1024});

	CompilationStatistics outer;
	outer.merge("bc", inner);

	REQUIRE(outer.contains("bc.ms"));
	REQUIRE(outer.contains("bc.codeSize.bytes"));
	REQUIRE(std::get<double>(*outer.find("bc.ms")) == Catch::Approx(2.5));
	REQUIRE(std::get<int64_t>(*outer.find("bc.codeSize.bytes")) == 1024);
}

TEST_CASE("CompilationStatistics: formatValue renders units") {
	CompilationStatistics stats;
	// Duration keys render milliseconds with "ms" suffix.
	REQUIRE(CompilationStatistics::formatValue("tracing.ms", 12.25) == "12.25 ms");
	REQUIRE(CompilationStatistics::formatValue("irPasses.totalMs", 7.0) == "7.00 ms");

	// Byte keys render with human-friendly units.
	REQUIRE(CompilationStatistics::formatValue("bc.codeSize.bytes", int64_t {512}) == "512 B");
	const auto kib = CompilationStatistics::formatValue("asmjit.codeSize.bytes", int64_t {2048});
	REQUIRE(kib == "2.00 KiB");

	// Integer counters get thousands separators.
	REQUIRE(CompilationStatistics::formatValue("ir.operations", int64_t {12345}) == "12,345");
}

TEST_CASE("CompilationStatistics: formatReport groups entries and renders header") {
	CompilationStatistics stats;
	stats.set("tracing.ms", 1.0);
	stats.set("ir.functions", int64_t {1});
	stats.set("ir.operations", int64_t {42});
	stats.set("irPasses.totalMs", 0.5);
	stats.set("irPasses.EmptyBlockEliminationPass.ms", 0.3);
	stats.set("backend.totalMs", 12.5);

	const auto report = stats.formatReport("2026-04-16_00-00-00_#ABCDEFG", "mlir");
	// Header shows the compilation id and backend.
	REQUIRE(report.find("2026-04-16_00-00-00_#ABCDEFG") != std::string::npos);
	REQUIRE(report.find("backend: mlir") != std::string::npos);
	// Group labels are rendered.
	REQUIRE(report.find("tracing") != std::string::npos);
	REQUIRE(report.find("irPasses") != std::string::npos);
	// Total lines appear first within irPasses group — they show up before
	// the EmptyBlockEliminationPass sub-entry in the output string.
	const auto totalPos = report.find("totalMs");
	const auto passPos = report.find("EmptyBlockEliminationPass");
	REQUIRE(totalPos != std::string::npos);
	REQUIRE(passPos != std::string::npos);
	REQUIRE(totalPos < passPos);
}

TEST_CASE("ScopedTimer: records elapsed time into stats") {
	CompilationStatistics stats;
	{
		ScopedTimer timer(&stats, "scope.ms");
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	REQUIRE(stats.contains("scope.ms"));
	REQUIRE(std::get<double>(*stats.find("scope.ms")) > 0.0);
}

TEST_CASE("ScopedTimer: null stats pointer is a no-op") {
	// Must not dereference the null pointer on destruction.
	ScopedTimer timer(nullptr, "ignored");
	(void) timer;
}
