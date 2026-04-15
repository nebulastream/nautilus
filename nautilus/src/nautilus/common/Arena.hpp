
#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <mutex>
#include <new>
#include <type_traits>
#include <utility>
#include <vector>

namespace nautilus::common {

/**
 * @brief A bump-pointer arena ("area") allocator.
 *
 * The Arena hands out memory from large, pre-allocated chunks using a
 * bump-pointer allocation scheme. Individual objects are never freed in
 * isolation; instead, every object allocated via the arena is destroyed and
 * every chunk released as part of destroying (or resetting) the Arena.
 *
 * This is a good fit for hot paths that produce a large number of short-lived,
 * similarly-typed objects (e.g. Block / TraceOperation during trace
 * construction) that are never freed until the owning structure is discarded.
 * Compared to calling ::operator new per object, the arena:
 *
 *   - amortises allocator overhead across many small allocations,
 *   - keeps logically related objects close together in memory, improving
 *     cache locality during downstream passes,
 *   - provides stable addresses for the lifetime of the arena: once an object
 *     is created it never moves, so references to it are never invalidated.
 *
 * To keep tiny consumers from paying for a heap chunk allocation, the Arena
 * carries a small inline buffer that is used first.  Only when the inline
 * buffer is exhausted is a heap chunk allocated on demand.
 *
 * After @ref softReset the arena keeps its previously allocated heap chunks
 * and walks back into them on subsequent overflows: the bump pointer first
 * resumes in the inline buffer, then in chunk[0], then chunk[1], ... and
 * only allocates a new chunk once the existing pool is exhausted.  This is
 * what makes the arena cheap to reuse across many cycles.
 *
 * The arena is non-copyable and non-movable.  Callers that need ownership
 * transfer should hold the Arena through a smart pointer.
 */
class Arena {
public:
	/// Size of the inline buffer embedded in every Arena instance.
	static constexpr std::size_t INLINE_BUFFER_SIZE = 4 * 1024;

	/// Size of the first heap chunk when the inline buffer is exhausted.
	static constexpr std::size_t INITIAL_CHUNK_SIZE = 16 * 1024;

	/// Maximum size an individual heap chunk is grown to.
	static constexpr std::size_t MAX_CHUNK_SIZE = 256 * 1024;

	Arena() noexcept {
		current = inlineBuffer;
		currentEnd = inlineBuffer + INLINE_BUFFER_SIZE;
	}

	~Arena() {
		release();
	}

	// The arena is non-copyable and non-movable.  The inline buffer's address
	// is part of the arena's identity, so moving would either leave pointers
	// into the source's buffer dangling or require copying the buffer
	// contents and fixing up pointers.  Neither is desirable.
	Arena(const Arena&) = delete;
	Arena& operator=(const Arena&) = delete;
	Arena(Arena&&) = delete;
	Arena& operator=(Arena&&) = delete;

	/**
	 * @brief Allocates @p size bytes with the requested @p align from the arena.
	 */
	void* allocate(std::size_t size, std::size_t align) {
		auto cur = reinterpret_cast<std::uintptr_t>(current);
		auto mask = static_cast<std::uintptr_t>(align) - 1;
		auto aligned = (cur + mask) & ~mask;
		if (aligned + size > reinterpret_cast<std::uintptr_t>(currentEnd)) {
			growChunk(size, align);
			cur = reinterpret_cast<std::uintptr_t>(current);
			aligned = (cur + mask) & ~mask;
		}
		current = reinterpret_cast<std::byte*>(aligned + size);
		return reinterpret_cast<void*>(aligned);
	}

	/**
	 * @brief Constructs a T with the given arguments inside the arena and
	 * registers its destructor (for non-trivially destructible types) so it is
	 * invoked when the arena is destroyed or reset.
	 */
	template <typename T, typename... Args>
	T* create(Args&&... args) {
		void* mem = allocate(sizeof(T), alignof(T));
		T* obj = ::new (mem) T(std::forward<Args>(args)...);
		if constexpr (!std::is_trivially_destructible_v<T>) {
			dtors.push_back({obj, &destroyThunk<T>});
		}
		return obj;
	}

	/**
	 * @brief Constructs a T inside the arena without registering its
	 * destructor.  The caller takes responsibility for explicitly destroying
	 * the object before the Arena is reset or destroyed.
	 *
	 * This variant is meant for hot paths where the caller already knows the
	 * full set of created objects and can destroy them in bulk without the
	 * per-allocation overhead of the dtor bookkeeping.
	 */
	template <typename T, typename... Args>
	T* createUnmanaged(Args&&... args) {
		void* mem = allocate(sizeof(T), alignof(T));
		return ::new (mem) T(std::forward<Args>(args)...);
	}

	/**
	 * @brief Destroys all objects created through this arena and frees all
	 * chunks.  After a reset the arena can be used to allocate again.
	 */
	void reset() {
		release();
		current = inlineBuffer;
		currentEnd = inlineBuffer + INLINE_BUFFER_SIZE;
		currentChunkIndex = INLINE_REGION_INDEX;
		nextChunkSize = INITIAL_CHUNK_SIZE;
	}

	/**
	 * @brief Resets the bump pointer and runs tracked destructors without
	 * releasing the heap chunks.
	 *
	 * The arena is left ready to service new allocations and will reuse the
	 * heap chunks it has already acquired before allocating any new ones.
	 * After softReset, allocations resume from the inline buffer; on overflow
	 * the bump pointer walks into chunk[0], chunk[1], ... in order, and only
	 * once every existing chunk is exhausted does growChunk fall back to
	 * std::malloc for a fresh chunk.  The caller is responsible for
	 * destroying any objects produced via @ref createUnmanaged before
	 * invoking this method.
	 */
	void softReset() noexcept {
		// Run tracked destructors in reverse construction order.
		for (auto it = dtors.rbegin(); it != dtors.rend(); ++it) {
			it->dtor(it->obj);
		}
		dtors.clear();
		current = inlineBuffer;
		currentEnd = inlineBuffer + INLINE_BUFFER_SIZE;
		currentChunkIndex = INLINE_REGION_INDEX;
	}

	/// Returns the total number of bytes currently held in heap chunks.
	std::size_t capacity() const noexcept {
		std::size_t total = 0;
		for (const auto& chunk : chunks) {
			total += chunk.size;
		}
		return total;
	}

	/// Returns the number of allocated objects whose destructors are tracked.
	std::size_t trackedObjectCount() const noexcept {
		return dtors.size();
	}

private:
	struct Chunk {
		std::byte* data;
		std::size_t size;
	};

	struct DtorEntry {
		void* obj;
		void (*dtor)(void*);
	};

	/// Sentinel value for @ref currentChunkIndex meaning "we are bumping
	/// inside the inline buffer, not in any heap chunk".
	static constexpr std::size_t INLINE_REGION_INDEX = static_cast<std::size_t>(-1);

	template <typename T>
	static void destroyThunk(void* ptr) {
		static_cast<T*>(ptr)->~T();
	}

	void growChunk(std::size_t size, std::size_t align) {
		// Worst-case bytes the next region must accommodate (the requested
		// size plus the alignment padding).
		std::size_t needed = size + align;

		// First try to walk into an already-allocated chunk that follows the
		// current region.  Each existing chunk is treated as fresh because
		// the bump pointer always restarts at chunk.data; this is what
		// allows softReset to recycle memory across cycles without going
		// through the system allocator.
		std::size_t nextIdx = (currentChunkIndex == INLINE_REGION_INDEX) ? 0 : currentChunkIndex + 1;
		while (nextIdx < chunks.size()) {
			const auto& chunk = chunks[nextIdx];
			if (chunk.size >= needed) {
				currentChunkIndex = nextIdx;
				current = chunk.data;
				currentEnd = chunk.data + chunk.size;
				return;
			}
			// Existing chunk is too small for this allocation; skip past it.
			// It remains available for a later allocation that does fit
			// within its size only if a future softReset rewinds the cursor
			// back through it again.
			++nextIdx;
		}

		// No existing chunk could service the allocation - allocate a new one.
		std::size_t chunkSize = std::max(nextChunkSize, needed);
		auto* mem = static_cast<std::byte*>(std::malloc(chunkSize));
		if (mem == nullptr) {
			throw std::bad_alloc();
		}
		chunks.push_back({mem, chunkSize});
		currentChunkIndex = chunks.size() - 1;
		current = mem;
		currentEnd = mem + chunkSize;
		// Grow subsequent chunks geometrically, capped at MAX_CHUNK_SIZE.
		nextChunkSize = std::min(nextChunkSize * 2, MAX_CHUNK_SIZE);
	}

	void release() noexcept {
		// Destroy tracked objects in reverse order of construction.
		for (auto it = dtors.rbegin(); it != dtors.rend(); ++it) {
			it->dtor(it->obj);
		}
		dtors.clear();
		for (auto& chunk : chunks) {
			std::free(chunk.data);
		}
		chunks.clear();
		current = nullptr;
		currentEnd = nullptr;
		currentChunkIndex = INLINE_REGION_INDEX;
	}

	alignas(std::max_align_t) std::byte inlineBuffer[INLINE_BUFFER_SIZE];
	std::byte* current;
	std::byte* currentEnd;
	/// Index into @ref chunks of the chunk we are currently bumping in, or
	/// @ref INLINE_REGION_INDEX when the bump pointer is inside @ref
	/// inlineBuffer.  Used by growChunk to walk into the next existing chunk
	/// (instead of always allocating a fresh one) after a softReset.
	std::size_t currentChunkIndex = INLINE_REGION_INDEX;
	std::size_t nextChunkSize = INITIAL_CHUNK_SIZE;
	std::vector<Chunk> chunks;
	std::vector<DtorEntry> dtors;
};

/**
 * @brief A pool of independent Arenas with recycled storage.
 *
 * Hands out @ref Handle objects that own an Arena for some scope (e.g. one
 * IRGraph, one compile cycle).  When a Handle is destroyed the Arena is
 * @ref Arena::softReset reset and pushed back into the pool, so the next
 * call to @ref acquire reuses that Arena's already-allocated heap chunks
 * instead of going through @c malloc again.  This amortises the per-Arena
 * heap-allocation cost across many compiles, which is the main reason
 * to use the pool over per-call standalone Arenas.
 *
 * The pool itself is internally synchronized with a mutex so callers from
 * different threads (e.g. main compile thread and tiered-promotion thread)
 * may share a single pool.  Acquired Handles are not thread-safe and must
 * not be shared across threads concurrently.
 *
 * Standalone use is supported: a default-constructed @ref Handle (or one
 * created without a backing pool, for example in tests) just deletes its
 * Arena on destruction instead of recycling it.  This means @ref Handle
 * is the universal Arena-owning smart pointer in the codebase.
 */
class ArenaPool {
public:
	/// Default cap on the number of spare Arenas held in the pool.  Excess
	/// released Arenas are freed instead of pushed back.
	static constexpr std::size_t DEFAULT_MAX_SPARES = 8;

	/// Custom deleter used by @ref Handle.  When @c pool is non-null the
	/// Arena is recycled into the pool; otherwise the Arena is deleted.
	struct Returner {
		ArenaPool* pool = nullptr;
		void operator()(Arena* arena) const noexcept;
	};

	/// Owning, RAII handle to an Arena.  When destroyed it either returns
	/// the Arena to its pool (if backed by one) or deletes it.
	using Handle = std::unique_ptr<Arena, Returner>;

	/// Constructs a Handle that owns a freshly heap-allocated Arena and is
	/// not backed by any pool.  Use this when you need an Arena outside
	/// the engine context (tests, benchmarks, ad-hoc compilations).
	static Handle makeStandalone() {
		return Handle(new Arena, Returner {});
	}

	explicit ArenaPool(std::size_t maxSpares = DEFAULT_MAX_SPARES) noexcept : maxSpares_(maxSpares) {
	}

	~ArenaPool() = default;

	ArenaPool(const ArenaPool&) = delete;
	ArenaPool& operator=(const ArenaPool&) = delete;
	ArenaPool(ArenaPool&&) = delete;
	ArenaPool& operator=(ArenaPool&&) = delete;

	/**
	 * @brief Acquires an Arena.  Returns a recycled Arena (with its chunks
	 * intact, ready to be bumped into) if one is available, or allocates a
	 * fresh one if the pool is empty.  The returned Handle returns the
	 * Arena to this pool when destroyed.
	 */
	Handle acquire() {
		std::unique_ptr<Arena> arena;
		{
			std::lock_guard<std::mutex> lock(mutex_);
			if (!spares_.empty()) {
				arena = std::move(spares_.back());
				spares_.pop_back();
			}
		}
		if (!arena) {
			arena = std::make_unique<Arena>();
		}
		return Handle(arena.release(), Returner {this});
	}

	/// Returns the current number of spare Arenas held in the pool.
	std::size_t spareCount() const noexcept {
		std::lock_guard<std::mutex> lock(mutex_);
		return spares_.size();
	}

private:
	friend struct Returner;

	void release(Arena* arena) noexcept {
		// Reset before pushing so the next acquirer sees an empty arena.
		arena->softReset();
		std::lock_guard<std::mutex> lock(mutex_);
		if (spares_.size() < maxSpares_) {
			spares_.emplace_back(arena);
		} else {
			delete arena;
		}
	}

	mutable std::mutex mutex_;
	std::vector<std::unique_ptr<Arena>> spares_;
	std::size_t maxSpares_;
};

inline void ArenaPool::Returner::operator()(Arena* arena) const noexcept {
	if (arena == nullptr) {
		return;
	}
	if (pool != nullptr) {
		pool->release(arena);
	} else {
		delete arena;
	}
}

} // namespace nautilus::common
