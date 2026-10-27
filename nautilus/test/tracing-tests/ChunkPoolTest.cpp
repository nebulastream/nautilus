// The bounded chunk pool (docs/plans/nautilus-0.4/02-nir.md §4.1; M1.8): size classes, dedicated allocations, the byte
// budget, decay with an injected clock, trim, the embedder's provider, and arenas on top of it.
#include "nautilus/common/Arena.hpp"
#include "nautilus/common/ChunkPool.hpp"
#include <atomic>
#include <catch2/catch_test_macros.hpp>
#include <cstring>
#include <map>
#include <random>
#include <thread>
#include <vector>

#if defined(__linux__)
#include <sys/mman.h>
#endif

namespace nautilus::common {

namespace {

constexpr std::size_t KiB = 1024;
constexpr std::size_t MiB = 1024 * KiB;

/// A clock the tests advance by hand.
struct FakeClock {
	std::shared_ptr<std::atomic<uint64_t>> now = std::make_shared<std::atomic<uint64_t>>(1000);
	std::function<uint64_t()> fn() const {
		auto n = now;
		return [n] {
			return n->load();
		};
	}
	void advance(uint64_t ms) {
		now->fetch_add(ms);
	}
};

ChunkPoolOptions options(std::size_t retained, FakeClock& clock) {
	ChunkPoolOptions o;
	o.retainedBytes = retained;
	o.clock = clock.fn();
	return o;
}

/// Counts what an embedder would be charged.
class CountingProvider : public ChunkProvider {
public:
	void* allocate(std::size_t size) override {
		if (limit != 0 && live + size > limit) {
			return nullptr;
		}
		void* p = std::aligned_alloc(4096, size);
		sizes[p] = size;
		live += size;
		++allocations;
		return p;
	}
	void deallocate(void* data, std::size_t size) noexcept override {
		REQUIRE(sizes.at(data) == size);
		sizes.erase(data);
		live -= size;
		std::free(data);
	}
	std::size_t limit = 0;
	std::size_t live = 0;
	std::size_t allocations = 0;
	std::map<void*, std::size_t> sizes;
};

} // namespace

TEST_CASE("chunks come in size classes and a large request gets a dedicated allocation", "[chunkpool]") {
	FakeClock clock;
	auto pool = ChunkPool::create(options(8 * MiB, clock));
	auto small = pool.acquire(1);
	CHECK(small.size == 64 * KiB);
	CHECK_FALSE(small.dedicated);
	auto grown = pool.acquire(1, 256 * KiB);
	CHECK(grown.size == 256 * KiB);
	// the request decides when it does not fit the preferred class
	auto bigger = pool.acquire(300 * KiB, 64 * KiB);
	CHECK(bigger.size == 512 * KiB);
	CHECK_FALSE(bigger.dedicated);
	auto capped = pool.acquire(10, 4 * MiB);
	CHECK(capped.size == ChunkPool::MAX_CHUNK);
	// above half a chunk: exactly the request, rounded to pages, never pooled
	auto huge = pool.acquire(600 * KiB + 1);
	CHECK(huge.dedicated);
	CHECK(huge.size == 600 * KiB + ChunkPool::PAGE);
	std::memset(huge.data, 1, huge.size);
	auto before = pool.stats();
	CHECK(before.dedicatedAllocations == 1);
	CHECK(before.inUseBytes == small.size + grown.size + bigger.size + capped.size + huge.size);
	for (auto c : {small, grown, bigger, capped, huge}) {
		pool.release(c);
	}
	auto after = pool.stats();
	CHECK(after.inUseBytes == 0);
	// the dedicated chunk went straight back; the others are idle
	CHECK(after.retainedBytes == small.size + grown.size + bigger.size + capped.size);
	CHECK(after.peakInUseBytes == before.inUseBytes);
}

TEST_CASE("a released chunk is reused without asking the OS again", "[chunkpool]") {
	FakeClock clock;
	auto pool = ChunkPool::create(options(8 * MiB, clock));
	auto first = pool.acquire(1, 128 * KiB);
	auto* data = first.data;
	pool.release(first);
	auto again = pool.acquire(1, 128 * KiB);
	CHECK(again.data == data);
	CHECK(pool.stats().chunksAllocated == 1);
	pool.release(again);
}

TEST_CASE("idle chunks stay under the byte budget", "[chunkpool]") {
	FakeClock clock;
	auto pool = ChunkPool::create(options(256 * KiB, clock));
	std::vector<Chunk> chunks;
	for (int i = 0; i < 16; ++i) {
		chunks.push_back(pool.acquire(1));
	}
	CHECK(pool.stats().heldBytes == 16 * 64 * KiB);
	for (auto c : chunks) {
		pool.release(c);
	}
	auto stats = pool.stats();
	CHECK(stats.retainedBytes <= 256 * KiB);
	CHECK(stats.chunksTrimmed == 16 - stats.retainedBytes / (64 * KiB));
	CHECK(stats.heldBytes == stats.retainedBytes);
	pool.setRetainedBytes(0);
	pool.release(pool.acquire(1));
	CHECK(pool.stats().retainedBytes <= 256 * KiB);
}

TEST_CASE("idle chunks lose their pages and then go back to the OS", "[chunkpool]") {
	FakeClock clock;
	auto o = options(8 * MiB, clock);
	o.releasePagesAfterMs = 200;
	o.unmapAfterMs = 5000;
	auto pool = ChunkPool::create(o);
	// more chunks than a thread cache holds, so that some reach the shared pool, where the clock decides
	std::vector<Chunk> chunks;
	for (int i = 0; i < 10; ++i) {
		chunks.push_back(pool.acquire(1));
		std::memset(chunks.back().data, 0x5a, chunks.back().size);
	}
	for (auto c : chunks) {
		pool.release(c);
	}
	CHECK(pool.stats().retainedBytes == 10 * 64 * KiB);
	clock.advance(300);
	pool.release(pool.acquire(1, 128 * KiB)); // an acquire or release in the shared pool runs the decay
	auto pages = pool.stats();
	CHECK(pages.chunksPagesReleased > 0);
	CHECK(pages.chunksTrimmed == 0);
#if defined(__linux__)
	// at least one idle chunk is no longer resident (the page release is a madvise, not a free)
	std::size_t gone = 0;
	for (auto c : chunks) {
		unsigned char vec[16];
		if (::mincore(c.data, c.size, vec) == 0 && (vec[0] & 1) == 0) {
			++gone;
		}
	}
	CHECK(gone > 0);
#endif
	clock.advance(6000);
	pool.release(pool.acquire(1, 128 * KiB));
	auto unmapped = pool.stats();
	CHECK(unmapped.chunksTrimmed > 0);
	CHECK(unmapped.retainedBytes < pages.retainedBytes);
}

TEST_CASE("trim gives every idle chunk back", "[chunkpool]") {
	FakeClock clock;
	auto pool = ChunkPool::create(options(8 * MiB, clock));
	std::vector<Chunk> chunks;
	for (int i = 0; i < 10; ++i) {
		chunks.push_back(pool.acquire(1, 256 * KiB));
	}
	auto held = pool.acquire(1);
	for (auto c : chunks) {
		pool.release(c);
	}
	REQUIRE(pool.stats().retainedBytes > 0);
	pool.trim();
	auto stats = pool.stats();
	CHECK(stats.retainedBytes == 0);
	CHECK(stats.heldBytes == held.size);
	CHECK(stats.chunksTrimmed == 10);
	pool.release(held);
}

TEST_CASE("an embedder's provider supplies and takes back every chunk", "[chunkpool]") {
	auto provider = std::make_shared<CountingProvider>();
	provider->limit = 3 * MiB;
	{
		FakeClock clock;
		auto o = options(1 * MiB, clock);
		o.provider = provider;
		auto pool = ChunkPool::create(o);
		auto a = pool.acquire(1, 1 * MiB);
		auto b = pool.acquire(1, 1 * MiB);
		CHECK(provider->live == 2 * MiB);
		auto huge = pool.acquire(700 * KiB);
		CHECK(provider->live == 2 * MiB + huge.size);
		// the provider refuses: the request fails instead of exceeding the embedder's limit
		CHECK_THROWS_AS(pool.acquire(1, 1 * MiB), std::bad_alloc);
		pool.release(huge);
		CHECK(provider->live == 2 * MiB);
		pool.release(a);
		pool.release(b); // over the 1 MiB budget: given back
		CHECK(provider->live == 1 * MiB);
		pool.trim();
		CHECK(provider->live == 0);
		pool.release(pool.acquire(1));
	}
	// destroying the pool returns what it still held
	CHECK(provider->live == 0);
	CHECK(provider->sizes.empty());
}

TEST_CASE("threads share a pool and leave nothing in use", "[chunkpool]") {
	auto pool = ChunkPool::create([] {
		ChunkPoolOptions o;
		o.retainedBytes = 4 * MiB;
		return o;
	}());
	std::vector<std::thread> threads;
	for (int t = 0; t < 8; ++t) {
		threads.emplace_back([&pool, t] {
			std::mt19937 rng(t);
			std::vector<Chunk> held;
			for (int i = 0; i < 20000; ++i) {
				if (held.size() < 6 && (rng() & 1)) {
					std::size_t request = (rng() % 8 == 0) ? 600 * KiB : 1 + rng() % (400 * KiB);
					held.push_back(pool.acquire(request, 64 * KiB << (rng() % 5)));
					held.back().data[0] = std::byte {1};
				} else if (!held.empty()) {
					pool.release(held.back());
					held.pop_back();
				}
			}
			for (auto c : held) {
				pool.release(c);
			}
		});
	}
	for (auto& t : threads) {
		t.join();
	}
	auto stats = pool.stats();
	CHECK(stats.inUseBytes == 0);
	// threads' caches and a racing budget check overshoot by a few chunks at most
	CHECK(stats.retainedBytes <= 4 * MiB + 8 * 4 * MiB);
}

TEST_CASE("an arena on a pool takes size classes and gives its chunks back", "[chunkpool][arena]") {
	FakeClock clock;
	auto pool = ChunkPool::create(options(8 * MiB, clock));
	{
		Arena arena(pool);
		for (int i = 0; i < 100; ++i) {
			arena.allocate(32 * KiB, 16);
		}
		// 3.2 MiB through chunks that grow from 64 KiB to 1 MiB
		CHECK(arena.capacity() >= 3 * MiB);
		CHECK(pool.stats().inUseBytes == arena.capacity());
		auto* big = arena.allocate(2 * MiB, 64);
		std::memset(big, 0, 2 * MiB);
		CHECK(pool.stats().dedicatedAllocations == 1);
	}
	CHECK(pool.stats().inUseBytes == 0);
	CHECK(pool.stats().retainedBytes <= 8 * MiB);
	// the arena pool recycles the arena objects, the chunk pool the memory
	ArenaPool arenas(pool);
	{
		auto handle = arenas.acquire();
		handle->allocate(1 * MiB, 16);
		CHECK(pool.stats().inUseBytes > 0);
	}
	CHECK(pool.stats().inUseBytes == 0);
}

TEST_CASE("after a large compile the held bytes return to the budget", "[chunkpool][arena]") {
	FakeClock clock;
	const std::size_t budget = 4 * MiB;
	auto pool = ChunkPool::create(options(budget, clock));
	ArenaPool arenas(pool);
	{
		// about the 22 MB that 0.3 kept for a 20,000-operation compile
		auto handle = arenas.acquire();
		for (int i = 0; i < 22 * 1024; ++i) {
			handle->allocate(1024, 16);
		}
		CHECK(pool.stats().heldBytes >= 22 * MiB);
	}
	auto stats = pool.stats();
	CHECK(stats.inUseBytes == 0);
	CHECK(stats.heldBytes <= budget);
	CHECK(stats.chunksTrimmed > 0);
}

TEST_CASE("forty thousand modules in a row hold memory steady", "[chunkpool][arena]") {
	FakeClock clock;
	const std::size_t budget = 4 * MiB;
	auto pool = ChunkPool::create(options(budget, clock));
	ArenaPool arenas(pool);
	std::mt19937 rng(7);
	for (int module = 0; module < 40000; ++module) {
		auto handle = arenas.acquire();
		// mostly small compiles, now and then one that needs megabytes
		std::size_t total = (module % 500 == 0) ? 3 * MiB + rng() % MiB : rng() % (200 * KiB);
		for (std::size_t done = 0; done < total; done += 4 * KiB) {
			handle->allocate(4 * KiB, 16);
		}
		if (module % 1000 == 0) {
			clock.advance(100);
		}
	}
	auto stats = pool.stats();
	CHECK(stats.inUseBytes == 0);
	CHECK(stats.heldBytes <= budget);
	// the steady state asks the OS for memory only a bounded number of times, not once per module
	CHECK(stats.chunksAllocated < 400);
	CHECK(stats.peakInUseBytes < 8 * MiB);
}

} // namespace nautilus::common
