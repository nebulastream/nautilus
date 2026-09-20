#include "nautilus/profiling/measurement.hpp"
#include <catch2/catch_test_macros.hpp>
#include <string>

using namespace nautilus;

// Pure formatting and lookup over values someone else collected: no hardware,
// no skips.

TEST_CASE("a default-constructed measurement is empty", "[profiling][measurement]") {
	const profiling::Measurement measurement;
	CHECK(measurement.empty());
	CHECK(measurement.entries().empty());
	CHECK_FALSE(measurement.get("cycles").has_value());
	CHECK(measurement.table().empty());
}

TEST_CASE("entries are looked up by event name", "[profiling][measurement]") {
	const profiling::Measurement measurement({{"cycles", 1000.0}, {"instructions", 2000.0}});

	CHECK(measurement.get("cycles") == 1000.0);
	CHECK(measurement.get("instructions") == 2000.0);
	CHECK_FALSE(measurement.get("cache-misses").has_value());
	CHECK_FALSE(measurement.empty());
}

TEST_CASE("the table lists every event", "[profiling][measurement]") {
	const profiling::Measurement measurement({{"cycles", 300.0}, {"seconds", 1.5}});

	const auto table = measurement.table();
	CHECK(table.find("cycles") != std::string::npos);
	CHECK(table.find("300") != std::string::npos);
	// A ratio keeps its decimals where a raw count does not.
	CHECK(table.find("1.500") != std::string::npos);
}

TEST_CASE("csv carries one row per event", "[profiling][measurement]") {
	const profiling::Measurement measurement({{"cycles", 300.0}});

	const auto csv = measurement.csv();
	CHECK(csv.find("event,value\n") == 0);
	CHECK(csv.find("cycles,300\n") != std::string::npos);
}
