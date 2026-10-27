// The engine's chunk pool (docs/plans/nautilus-0.4/02-nir.md §4.1): the budget option, trimMemory, the counters and an
// embedder's provider, observed through a real engine.
#include "nautilus/Engine.hpp"
#include "nautilus/common/ChunkProvider.hpp"
#include "nautilus/config.hpp"
#include <catch2/catch_test_macros.hpp>
#include <cstdlib>
#include <map>
#include <mutex>

#if defined(ENABLE_TRACING) && defined(ENABLE_TBC_BACKEND)

namespace nautilus::engine {

namespace {

val<int32_t> arenaAddOne(val<int32_t> x) {
	return x + 1;
}

val<int32_t> arenaLoop(val<int32_t> n) {
	val<int32_t> sum = 0;
	for (val<int32_t> i = 0; i < n; i = i + 1) {
		sum = sum + i * 3;
	}
	return sum;
}

class CountingProvider : public common::ChunkProvider {
public:
	void* allocate(std::size_t size) override {
		std::lock_guard<std::mutex> lock(mutex);
		void* p = std::aligned_alloc(4096, size);
		sizes[p] = size;
		live += size;
		return p;
	}
	void deallocate(void* data, std::size_t size) noexcept override {
		std::lock_guard<std::mutex> lock(mutex);
		sizes.erase(data);
		live -= size;
		(void) size;
		std::free(data);
	}
	std::mutex mutex;
	std::size_t live = 0;
	std::map<void*, std::size_t> sizes;
};

Options singleTier(int retainedBytes) {
	Options options;
	options.setOption("engine.backend", std::string("tbc"));
	options.setOption("engine.arena.retainedBytes", retainedBytes);
	return options;
}

} // namespace

TEST_CASE("the engine keeps at most engine.arena.retainedBytes idle and trimMemory gives it back", "[arena][engine]") {
	const int budget = 256 * 1024;
	NautilusEngine engine(singleTier(budget));
	for (int i = 0; i < 20; ++i) {
		auto f = engine.registerFunction(arenaLoop);
		REQUIRE(f(10) == 135);
	}
	auto stats = engine.arenaStatistics();
	CHECK(stats.chunksAllocated > 0);
	// the per-thread caches and a racing budget check overshoot by a few chunks at most
	CHECK(stats.retainedBytes <= static_cast<uint64_t>(budget) + 4 * 1024 * 1024);
	engine.trimMemory();
	CHECK(engine.arenaStatistics().retainedBytes == 0);
	// the engine keeps working after a trim
	auto f = engine.registerFunction(arenaAddOne);
	REQUIRE(f(41) == 42);
}

TEST_CASE("an embedder's provider backs the engine's arenas", "[arena][engine]") {
	auto provider = std::make_shared<CountingProvider>();
	{
		NautilusEngine engine(singleTier(0), provider);
		auto f = engine.registerFunction(arenaLoop);
		REQUIRE(f(10) == 135);
		CHECK(engine.arenaStatistics().chunksAllocated > 0);
		// the budget is 0: whatever the compile used was handed back as soon as it finished
		engine.trimMemory();
		CHECK(engine.arenaStatistics().retainedBytes == 0);
	}
	CHECK(provider->sizes.empty());
	CHECK(provider->live == 0);
}

} // namespace nautilus::engine

#endif
