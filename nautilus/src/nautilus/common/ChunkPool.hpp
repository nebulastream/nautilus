#pragma once

// The bounded chunk pool of docs/plans/nautilus-0.4/02-nir.md §4.1: arenas take their memory in chunks of fixed size
// classes (64 KiB to 1 MiB) from one pool per engine. A request above half the largest chunk gets a dedicated
// allocation that is never pooled. Idle chunks stay under a byte budget, lose their pages after a short idle time and
// go back to the OS after a longer one, and an embedder can supply the memory itself.

#include "nautilus/common/ChunkProvider.hpp"
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>

namespace nautilus::common {

struct Chunk {
	std::byte* data = nullptr;
	std::size_t size = 0;
	/// A dedicated allocation for one large request: freed when released, never pooled.
	bool dedicated = false;
};

struct ChunkPoolOptions {
	/// At most this many bytes sit idle in the pool (and in per-thread caches); a chunk beyond it goes back to the OS
	/// immediately. `engine.arena.retainedBytes`.
	std::size_t retainedBytes = 8u * 1024 * 1024;
	/// An idle chunk loses its pages after this long (`madvise`), and goes back to the OS after `unmapAfterMs`.
	uint64_t releasePagesAfterMs = 200;
	uint64_t unmapAfterMs = 5000;
	/// Optional embedder memory. Without it chunks are page-backed (`mmap`).
	std::shared_ptr<ChunkProvider> provider;
	/// Milliseconds on a monotonic clock; tests inject a controllable one.
	std::function<uint64_t()> clock;
};

/// A handle to a chunk pool; copies share it, and it stays alive while an arena holds a copy. A default-constructed
/// handle is empty (`valid()` is false).
class ChunkPool {
public:
	static constexpr std::size_t MIN_CHUNK = 64 * 1024;
	static constexpr std::size_t MAX_CHUNK = 1024 * 1024;
	static constexpr std::size_t PAGE = 4096;

	ChunkPool() = default;
	/// Creates a new pool.
	explicit ChunkPool(ChunkPoolOptions options);
	static ChunkPool create(ChunkPoolOptions options = {}) {
		return ChunkPool(std::move(options));
	}

	bool valid() const noexcept {
		return state_ != nullptr;
	}

	/// A chunk of at least `minSize` bytes. A request up to half of MAX_CHUNK gets a chunk of the size class that is
	/// at least `preferredSize` (the arena's growth step) and `minSize`; a larger one gets a dedicated allocation.
	/// Throws std::bad_alloc if the memory is refused.
	Chunk acquire(std::size_t minSize, std::size_t preferredSize = MIN_CHUNK);
	/// Returns a chunk obtained from `acquire` of this pool.
	void release(Chunk chunk) noexcept;

	/// Gives every idle chunk back to the OS: the reaction to memory pressure (`NautilusEngine::trimMemory`).
	void trim() noexcept;
	void setRetainedBytes(std::size_t bytes) noexcept;

	ChunkPoolStats stats() const noexcept;
	void resetPeak() noexcept;

	bool operator==(const ChunkPool& other) const noexcept {
		return state_ == other.state_;
	}

	struct State;

private:
	std::shared_ptr<State> state_;
};

} // namespace nautilus::common
