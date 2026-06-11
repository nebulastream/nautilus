#include "nautilus/tracing/ExceptionBasedTraceContext.hpp"
#include <catch2/catch_test_macros.hpp>

namespace nautilus::tracing {

TEST_CASE("AliveVariableHash erases entries that reach a zero count", "[AliveVariableHash]") {
	AliveVariableHash avh;
	REQUIRE(avh.size() == 0);
	REQUIRE(avh.hash() == 0);

	avh.increment(1);
	avh.increment(2);
	avh.increment(3);
	REQUIRE(avh.size() == 3);

	avh.decrement(2);
	REQUIRE(avh.size() == 2);

	avh.decrement(1);
	avh.decrement(3);
	REQUIRE(avh.size() == 0);
	REQUIRE(avh.hash() == 0);
}

TEST_CASE("AliveVariableHash erasure is hash-neutral", "[AliveVariableHash]") {
	// A variable that was seen and fully released must leave the hash identical
	// to one that was never seen. Snapshots key on this hash, so any difference
	// would change trace deduplication and the generated IR.
	AliveVariableHash seenAndReleased;
	seenAndReleased.increment(7);
	seenAndReleased.increment(42);
	seenAndReleased.decrement(42);

	AliveVariableHash neverSeen;
	neverSeen.increment(7);

	REQUIRE(seenAndReleased.hash() == neverSeen.hash());
	REQUIRE(seenAndReleased.size() == neverSeen.size());
}

TEST_CASE("AliveVariableHash re-incrementing a released id reproduces the original hash", "[AliveVariableHash]") {
	AliveVariableHash avh;
	avh.increment(5);
	const uint64_t hashWithFive = avh.hash();

	avh.decrement(5);
	avh.increment(5);
	REQUIRE(avh.hash() == hashWithFive);
	REQUIRE(avh.size() == 1);
}

TEST_CASE("AliveVariableHash hash reflects reference counts", "[AliveVariableHash]") {
	AliveVariableHash once;
	once.increment(9);

	AliveVariableHash twice;
	twice.increment(9);
	twice.increment(9);

	REQUIRE(once.hash() != twice.hash());
	REQUIRE(twice.size() == 1);

	twice.decrement(9);
	REQUIRE(twice.hash() == once.hash());
}

TEST_CASE("AliveVariableHash reset clears live entries", "[AliveVariableHash]") {
	AliveVariableHash avh;
	avh.increment(1);
	avh.increment(1);
	avh.increment(2);

	avh.reset();
	REQUIRE(avh.size() == 0);
	REQUIRE(avh.hash() == 0);
}

} // namespace nautilus::tracing
