#include "ExecutionTest.hpp"
#include "nautilus/Engine.hpp"
#include "nautilus/prefetch/plugin.hpp"
#include "nautilus/prefetch/prefetch.hpp"
#include <catch2/catch_all.hpp>
#include <cstdint>

namespace nautilus::engine {

// ============================================================================
// Test Functions
//
// prefetch() is purely a performance hint: it never affects program
// correctness. Every test below checks that the surrounding computation
// produces exactly the same result whether or not -- and however -- the
// prefetch hint is issued.
// ============================================================================

/// Sums an array while issuing a prefetch hint for each element just before
/// reading it.
template <PrefetchRW RW, PrefetchLocality Locality>
val<int64_t> sumWithPrefetch(val<int64_t*> data, val<int64_t> count) {
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < count; i = i + 1) {
		prefetch(data + i, RW, Locality);
		sum = sum + data[i];
	}
	return sum;
}

/// Same access pattern, relying on prefetch()'s default arguments (Read, High).
val<int64_t> sumWithDefaultPrefetch(val<int64_t*> data, val<int64_t> count) {
	val<int64_t> sum = 0;
	for (val<int64_t> i = 0; i < count; i = i + 1) {
		prefetch(data + i);
		sum = sum + data[i];
	}
	return sum;
}

/// Writes to an array while issuing a write-intent prefetch just before each
/// store; the stored values must be unaffected by the prefetch hint.
val<int64_t> fillWithWritePrefetch(val<int64_t*> data, val<int64_t> count) {
	for (val<int64_t> i = 0; i < count; i = i + 1) {
		prefetch(data + i, PrefetchRW::Write, PrefetchLocality::Moderate);
		data[i] = i * 2;
	}
	return count;
}

// ============================================================================
// Test Body
// ============================================================================

void prefetchTests(engine::NautilusEngine& engine) {
	constexpr int64_t N = 64;

	SECTION("read prefetch, all localities") {
		int64_t data[N];
		int64_t expected = 0;
		for (int64_t i = 0; i < N; i++) {
			data[i] = i + 1;
			expected += data[i];
		}

		auto none = engine.registerFunction(sumWithPrefetch<PrefetchRW::Read, PrefetchLocality::None>);
		REQUIRE(none(data, N) == expected);

		auto low = engine.registerFunction(sumWithPrefetch<PrefetchRW::Read, PrefetchLocality::Low>);
		REQUIRE(low(data, N) == expected);

		auto moderate = engine.registerFunction(sumWithPrefetch<PrefetchRW::Read, PrefetchLocality::Moderate>);
		REQUIRE(moderate(data, N) == expected);

		auto high = engine.registerFunction(sumWithPrefetch<PrefetchRW::Read, PrefetchLocality::High>);
		REQUIRE(high(data, N) == expected);
	}

	SECTION("write-hinted prefetch, all localities") {
		int64_t data[N] = {};

		auto none = engine.registerFunction(sumWithPrefetch<PrefetchRW::Write, PrefetchLocality::None>);
		REQUIRE(none(data, N) == 0);

		auto low = engine.registerFunction(sumWithPrefetch<PrefetchRW::Write, PrefetchLocality::Low>);
		REQUIRE(low(data, N) == 0);

		auto moderate = engine.registerFunction(sumWithPrefetch<PrefetchRW::Write, PrefetchLocality::Moderate>);
		REQUIRE(moderate(data, N) == 0);

		auto high = engine.registerFunction(sumWithPrefetch<PrefetchRW::Write, PrefetchLocality::High>);
		REQUIRE(high(data, N) == 0);
	}

	SECTION("default prefetch arguments") {
		int64_t data[N];
		int64_t expected = 0;
		for (int64_t i = 0; i < N; i++) {
			data[i] = i * 3;
			expected += data[i];
		}
		auto f = engine.registerFunction(sumWithDefaultPrefetch);
		REQUIRE(f(data, N) == expected);
	}

	SECTION("write prefetch alongside actual stores") {
		int64_t data[N] = {};
		auto f = engine.registerFunction(fillWithWritePrefetch);
		REQUIRE(f(data, N) == N);
		for (int64_t i = 0; i < N; i++) {
			REQUIRE(data[i] == i * 2);
		}
	}
}

TEST_CASE("Prefetch Interpreter Test") {
	auto engine = nautilus::testing::makeEngine("interpreter");
	prefetchTests(engine);
}

#ifdef ENABLE_TRACING
TEST_CASE("Prefetch Compiler Test") {
	nautilus::testing::forEachBackend([](engine::NautilusEngine& engine) { prefetchTests(engine); },
	                                  /*include_interpreter=*/false);
}
#endif

} // namespace nautilus::engine
