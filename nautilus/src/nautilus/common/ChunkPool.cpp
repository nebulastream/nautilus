#include "nautilus/common/ChunkPool.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <chrono>
#include <cstdlib>
#include <mutex>
#include <new>
#include <vector>

#if defined(__unix__) || defined(__APPLE__)
#include <sys/mman.h>
#define NAUTILUS_CHUNK_MMAP 1
#endif

namespace nautilus::common {

namespace {

constexpr std::size_t NUM_CLASSES = 5; // 64 KiB, 128 KiB, 256 KiB, 512 KiB, 1 MiB

std::size_t classIndex(std::size_t size) {
	return static_cast<std::size_t>(std::countr_zero(size / ChunkPool::MIN_CHUNK));
}

std::size_t roundUp(std::size_t value, std::size_t multiple) {
	return (value + multiple - 1) / multiple * multiple;
}

uint64_t steadyMs() {
	return static_cast<uint64_t>(
	    std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch())
	        .count());
}

} // namespace

struct ChunkPool::State {
	struct Idle {
		Chunk chunk;
		uint64_t since;
		bool pagesReleased;
	};

	explicit State(ChunkPoolOptions o)
	    : options(std::move(o)), budget(options.retainedBytes), id(nextId().fetch_add(1) + 1) {
	}

	~State() {
		for (auto& list : idle) {
			for (auto& e : list) {
				osFree(e.chunk);
			}
		}
	}

	static std::atomic<uint64_t>& nextId() {
		static std::atomic<uint64_t> id {0};
		return id;
	}

	uint64_t now() const {
		return options.clock ? options.clock() : steadyMs();
	}

	std::byte* osAllocate(std::size_t size) {
		void* p = nullptr;
		if (options.provider) {
			p = options.provider->allocate(size);
		} else {
#ifdef NAUTILUS_CHUNK_MMAP
			p = ::mmap(nullptr, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
			if (p == MAP_FAILED) {
				p = nullptr;
			}
#else
			p = std::aligned_alloc(PAGE, size);
#endif
		}
		if (p == nullptr) {
			throw std::bad_alloc();
		}
		fromOs.fetch_add(1, std::memory_order_relaxed);
		return static_cast<std::byte*>(p);
	}

	void osFree(const Chunk& chunk) noexcept {
		if (options.provider) {
			options.provider->deallocate(chunk.data, chunk.size);
			return;
		}
#ifdef NAUTILUS_CHUNK_MMAP
		::munmap(chunk.data, chunk.size);
#else
		std::free(chunk.data);
#endif
	}

	void dropPages(const Chunk& chunk) noexcept {
#ifdef NAUTILUS_CHUNK_MMAP
		if (!options.provider) {
#if defined(__APPLE__)
			::madvise(chunk.data, chunk.size, MADV_FREE_REUSABLE);
#else
			::madvise(chunk.data, chunk.size, MADV_DONTNEED);
#endif
		}
#else
		(void) chunk;
#endif
	}

	/// Idle chunks that waited long enough lose their pages; ones that waited longer go back to the OS. Locked.
	void decay(uint64_t at) noexcept {
		for (auto& list : idle) {
			std::size_t keep = 0;
			for (std::size_t i = 0; i < list.size(); ++i) {
				auto& e = list[i];
				const uint64_t age = at - e.since;
				if (age >= options.unmapAfterMs) {
					retained.fetch_sub(e.chunk.size, std::memory_order_relaxed);
					trimmed.fetch_add(1, std::memory_order_relaxed);
					osFree(e.chunk);
					continue;
				}
				if (!e.pagesReleased && age >= options.releasePagesAfterMs) {
					dropPages(e.chunk);
					e.pagesReleased = true;
					pagesReleased.fetch_add(1, std::memory_order_relaxed);
				}
				list[keep++] = e;
			}
			list.resize(keep);
		}
	}

	void noteInUse(std::size_t size) noexcept {
		auto now = inUse.fetch_add(size, std::memory_order_relaxed) + size;
		auto seen = peak.load(std::memory_order_relaxed);
		while (now > seen && !peak.compare_exchange_weak(seen, now, std::memory_order_relaxed)) {
		}
	}

	ChunkPoolOptions options;
	std::atomic<std::size_t> budget;
	std::mutex mutex;
	std::array<std::vector<Idle>, NUM_CLASSES> idle; // oldest first
	std::atomic<uint64_t> inUse {0}, retained {0}, peak {0}, trimmed {0}, pagesReleased {0}, fromOs {0}, dedicated {0};
	/// Bumped by `trim`: per-thread caches older than this are dropped on their thread's next use.
	std::atomic<uint64_t> epoch {0};
	/// Tells a pool apart from a later one at the same address (the per-thread caches compare ids).
	const uint64_t id;
};

namespace {

/// A few chunks a thread keeps for itself, so that the steady state takes no lock. A cache does not keep its pool
/// alive: when the pool is gone, its chunks (plain page-backed memory; caches are not used with a provider) are
/// unmapped without accounting.
struct ThreadCache {
	static constexpr std::size_t CAPACITY = 4;

	std::weak_ptr<ChunkPool::State> pool;
	uint64_t poolId = 0;
	uint64_t epoch = 0;
	std::array<Chunk, CAPACITY> items {};
	std::size_t count = 0;

	~ThreadCache() {
		flush();
	}

	void flush() noexcept {
		auto state = pool.lock();
		for (std::size_t i = 0; i < count; ++i) {
			if (state) {
				state->retained.fetch_sub(items[i].size, std::memory_order_relaxed);
				state->trimmed.fetch_add(1, std::memory_order_relaxed);
				state->osFree(items[i]);
			} else {
#ifdef NAUTILUS_CHUNK_MMAP
				::munmap(items[i].data, items[i].size);
#else
				std::free(items[i].data);
#endif
			}
		}
		count = 0;
	}

	/// Binds the cache to `s`, dropping what it held for another pool or before a trim.
	void attach(const std::shared_ptr<ChunkPool::State>& s) noexcept {
		if (poolId != s->id) {
			flush();
			pool = s;
			poolId = s->id;
			epoch = s->epoch.load(std::memory_order_acquire);
		} else if (epoch != s->epoch.load(std::memory_order_acquire)) {
			flush();
			epoch = s->epoch.load(std::memory_order_acquire);
		}
	}
};

ThreadCache& threadCache() {
	thread_local ThreadCache cache;
	return cache;
}

} // namespace

ChunkPool::ChunkPool(ChunkPoolOptions options) : state_(std::make_shared<State>(std::move(options))) {
}

Chunk ChunkPool::acquire(std::size_t minSize, std::size_t preferredSize) {
	auto& s = *state_;
	if (minSize > MAX_CHUNK / 2) {
		Chunk chunk;
		chunk.size = roundUp(minSize, PAGE);
		chunk.data = s.osAllocate(chunk.size);
		chunk.dedicated = true;
		s.dedicated.fetch_add(1, std::memory_order_relaxed);
		s.noteInUse(chunk.size);
		return chunk;
	}
	std::size_t size = std::max({preferredSize, std::bit_ceil(std::max(minSize, MIN_CHUNK)), MIN_CHUNK});
	size = std::min(std::bit_ceil(size), MAX_CHUNK);
	const auto cls = classIndex(size);

	if (!s.options.provider) {
		auto& cache = threadCache();
		cache.attach(state_);
		for (std::size_t i = cache.count; i > 0; --i) {
			if (cache.items[i - 1].size == size) {
				Chunk chunk = cache.items[i - 1];
				cache.items[i - 1] = cache.items[--cache.count];
				s.retained.fetch_sub(size, std::memory_order_relaxed);
				s.noteInUse(size);
				return chunk;
			}
		}
	}
	{
		std::lock_guard<std::mutex> lock(s.mutex);
		s.decay(s.now());
		auto& list = s.idle[cls];
		if (!list.empty()) {
			Chunk chunk = list.back().chunk;
			list.pop_back();
			s.retained.fetch_sub(size, std::memory_order_relaxed);
			s.noteInUse(size);
			return chunk;
		}
	}
	Chunk chunk;
	chunk.size = size;
	chunk.data = s.osAllocate(size);
	s.noteInUse(size);
	return chunk;
}

void ChunkPool::release(Chunk chunk) noexcept {
	if (chunk.data == nullptr) {
		return;
	}
	auto& s = *state_;
	s.inUse.fetch_sub(chunk.size, std::memory_order_relaxed);
	if (chunk.dedicated) {
		s.osFree(chunk);
		return;
	}
	if (s.retained.load(std::memory_order_relaxed) + chunk.size > s.budget.load(std::memory_order_relaxed)) {
		s.trimmed.fetch_add(1, std::memory_order_relaxed);
		s.osFree(chunk);
		return;
	}
	if (!s.options.provider) {
		auto& cache = threadCache();
		cache.attach(state_);
		if (cache.count < ThreadCache::CAPACITY) {
			s.retained.fetch_add(chunk.size, std::memory_order_relaxed);
			cache.items[cache.count++] = chunk;
			return;
		}
	}
	std::lock_guard<std::mutex> lock(s.mutex);
	const auto at = s.now();
	s.decay(at);
	s.retained.fetch_add(chunk.size, std::memory_order_relaxed);
	s.idle[classIndex(chunk.size)].push_back({chunk, at, false});
}

void ChunkPool::trim() noexcept {
	auto& s = *state_;
	s.epoch.fetch_add(1, std::memory_order_release);
	if (!s.options.provider) {
		threadCache().attach(state_); // drops this thread's cache now, other threads' on their next use
	}
	std::lock_guard<std::mutex> lock(s.mutex);
	for (auto& list : s.idle) {
		for (auto& e : list) {
			s.retained.fetch_sub(e.chunk.size, std::memory_order_relaxed);
			s.trimmed.fetch_add(1, std::memory_order_relaxed);
			s.osFree(e.chunk);
		}
		list.clear();
	}
}

void ChunkPool::setRetainedBytes(std::size_t bytes) noexcept {
	state_->budget.store(bytes, std::memory_order_relaxed);
}

ChunkPoolStats ChunkPool::stats() const noexcept {
	auto& s = *state_;
	ChunkPoolStats out;
	out.inUseBytes = s.inUse.load(std::memory_order_relaxed);
	out.retainedBytes = s.retained.load(std::memory_order_relaxed);
	out.heldBytes = out.inUseBytes + out.retainedBytes;
	out.peakInUseBytes = s.peak.load(std::memory_order_relaxed);
	out.chunksTrimmed = s.trimmed.load(std::memory_order_relaxed);
	out.chunksPagesReleased = s.pagesReleased.load(std::memory_order_relaxed);
	out.chunksAllocated = s.fromOs.load(std::memory_order_relaxed);
	out.dedicatedAllocations = s.dedicated.load(std::memory_order_relaxed);
	return out;
}

void ChunkPool::resetPeak() noexcept {
	state_->peak.store(state_->inUse.load(std::memory_order_relaxed), std::memory_order_relaxed);
}

} // namespace nautilus::common
