#pragma once

// The embedder-facing part of the engine's chunk pool (docs/plans/nautilus-0.4/02-nir.md §4.1): a provider of chunk
// memory and the pool's counters.

#include <cstddef>
#include <cstdint>

namespace nautilus::common {

/// A source of chunk memory supplied by the embedder (02 §4.1.6), so that compiler memory can be charged to the
/// embedder's own budget. `allocate` returns nullptr to refuse. Chunks are at least 4 KiB aligned.
class ChunkProvider {
public:
	virtual ~ChunkProvider() = default;
	virtual void* allocate(std::size_t size) = 0;
	virtual void deallocate(void* data, std::size_t size) noexcept = 0;
};

struct ChunkPoolStats {
	/// Bytes handed out to arenas right now.
	uint64_t inUseBytes = 0;
	/// Bytes idle in the pool and the per-thread caches.
	uint64_t retainedBytes = 0;
	/// inUseBytes + retainedBytes: what the pool holds from the OS or the provider.
	uint64_t heldBytes = 0;
	/// The most bytes in use at once since the last `resetPeak`.
	uint64_t peakInUseBytes = 0;
	/// Chunks given back to the OS or the provider: over budget, idle too long, or trimmed.
	uint64_t chunksTrimmed = 0;
	/// Idle chunks whose pages were released while the address range stays in the pool.
	uint64_t chunksPagesReleased = 0;
	/// Chunks taken from the OS or the provider.
	uint64_t chunksAllocated = 0;
	/// Dedicated allocations made for large requests.
	uint64_t dedicatedAllocations = 0;
};

} // namespace nautilus::common
