#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>

namespace nautilus::tracing {

// The bookkeeping that every tracer keeps per scope: the static variables that go into a snapshot and the alive
// variables (with their reference counts) that go into it too. Shared by TraceContext and the NIR tracer.

struct StaticVarHolder {
	explicit StaticVarHolder(const void* ptr, size_t size) : ptr(ptr), size(size) {
	}

private:
	const void* ptr;
	size_t size;
	friend uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);
	friend size_t getStaticVarValue(const StaticVarHolder& holder);
};

/// The hash of the values of the static variables that are alive (TraceContext.cpp).
uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);

inline size_t getStaticVarValue(const StaticVarHolder& holder) {
	size_t result = 0;
	std::memcpy(&result, holder.ptr, holder.size);
	return result;
}

/**
 * @brief Efficiently tracks reference counts and computes an incremental hash of alive variables.
 *
 * ValueRefs are dense small integers (ExecutionTrace::getNextValueRef() is a plain incrementing
 * counter), so reference counts are stored in a vector indexed by id rather than a hash map.
 * The hash reflects both which variables are alive and their reference counts, updated
 * incrementally in O(1) time.
 *
 * Implementation details:
 * - Uses XOR-based hashing for O(1) incremental updates
 * - Each variable ID is mixed with a constant multiplier for better hash distribution
 * - The hash incorporates both variable identity (ID) and reference count
 * - Uses a growable vector indexed by id - no allocation on increment/decrement beyond
 *   the amortized growth needed to cover the highest id seen so far
 * - A separate alive-count is maintained so size() stays O(1) even though zero-count
 *   entries are not removed from the vector
 *
 * Performance characteristics:
 * - increment(): O(1) amortized - vector index + two XOR operations, two multiplications
 * - decrement(): O(1) - vector index + two XOR operations, two multiplications
 * - hash(): O(1) - returns cached value
 * - size(): O(1) - returns cached alive count
 *
 * @note Changed from a hash map (which erased entries to bound its size) to a vector indexed
 * by ValueRef. This trades peak-alive-sized memory for maxRef-sized memory in exchange for
 * removing the malloc/free pair that the map's insert/erase pair cost per traced value.
 */
class AliveVariableHash {
	static constexpr uint64_t HASH_MULTIPLIER = 0x9e3779b97f4a7c15; // Golden ratio constant for good mixing

	std::vector<uint32_t> counts;
	size_t aliveCount = 0;
	uint64_t alive_hash = 0;

public:
	/**
	 * @brief Default constructor. No initialization needed as counts are zero-initialized.
	 */
	AliveVariableHash() = default;

	/**
	 * @brief Increments the reference count for a variable and updates the hash.
	 *
	 * The hash is updated by XOR-ing out the old contribution ((id * HASH_MULTIPLIER) * old_count)
	 * and XOR-ing in the new contribution ((id * HASH_MULTIPLIER) * new_count).
	 *
	 * @param id Variable identifier (32-bit value)
	 */
	inline void increment(uint32_t id) noexcept {
		if (id >= counts.size()) {
			counts.resize(id + 1, 0);
		}
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		if (c == 0) {
			++aliveCount;
		}
		++c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
	}

	/**
	 * @brief Decrements the reference count for a variable and updates the hash.
	 *
	 * The hash is updated by XOR-ing out the old contribution ((id * HASH_MULTIPLIER) * old_count)
	 * and XOR-ing in the new contribution ((id * HASH_MULTIPLIER) * new_count).
	 *
	 * @param id Variable identifier (32-bit value), previously passed to increment()
	 */
	inline void decrement(uint32_t id) noexcept {
		// Releasing a ref this scope never took is legitimate and must not underflow.
		// A scope's environment is reset between exploration passes (resume()), but a
		// C++ val<T> can outlive the pass that created it -- a std::optional or vector
		// declared outside a region() and written inside it holds its ref into the next
		// pass, and releases it there. The count for that ref is already zero, and there
		// is nothing to give back. Underflowing here used to be masked by reset()
		// unconditionally refilling the vector; it no longer is, and the invariant that
		// aliveCount equals the number of non-zero entries is what reset() and
		// forEachAlive now rely on to stay O(1).
		if (id >= counts.size() || counts[id] == 0) {
			return;
		}
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		--c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		if (c == 0) {
			--aliveCount;
		}
	}

	/**
	 * @brief Returns the current hash value representing the state of alive variables.
	 *
	 * The hash reflects both which variables have non-zero reference counts and the
	 * magnitude of those counts. This value is maintained incrementally and can be
	 * retrieved in O(1) time.
	 *
	 * @return 64-bit hash value representing current variable state
	 */
	inline uint64_t hash() const noexcept {
		return alive_hash;
	}

	/**
	 * @brief Returns the number of currently-alive variables (non-zero reference count).
	 * @return Number of currently-alive variables
	 */
	inline size_t size() const noexcept {
		return aliveCount;
	}

	/**
	 * @brief Returns whether @p id currently has a non-zero reference count.
	 */
	inline bool isAlive(uint32_t id) const noexcept {
		return id < counts.size() && counts[id] != 0;
	}

	/**
	 * @brief Invokes @p fn(id, count) for every currently-alive variable.
	 *
	 * Used at a region boundary, to hand the region's still-alive refs over to the
	 * enclosing scope (see docs/region.md). Nothing being alive is the overwhelmingly
	 * common case there, and ids are global and dense, so the scan would otherwise be
	 * linear in the highest ref the *whole* trace has reached -- paid once per region,
	 * which is quadratic over a function built out of many regions. Both the empty
	 * early-out and the alive-count countdown below exist to keep that off the profile.
	 */
	template <typename F>
	inline void forEachAlive(F&& fn) const {
		if (aliveCount == 0) {
			return;
		}
		size_t remaining = aliveCount;
		for (size_t id = 0; id < counts.size() && remaining > 0; id++) {
			if (counts[id] != 0) {
				--remaining;
				fn(static_cast<uint32_t>(id), counts[id]);
			}
		}
	}

	/**
	 * @brief Resets all reference counts and hash to initial state.
	 *
	 * Zeroes every slot in the backing vector rather than shrinking it, so the vector's
	 * capacity - and thus the highest id it can hold without reallocating - is retained
	 * across trace iterations.
	 */
	inline void reset() noexcept {
		// aliveCount is by construction the number of non-zero entries, so when it is
		// zero every slot is already zero and the fill is pure cost. That is the normal
		// state at the end of a pass -- the traced body returned, so its val<T>s were
		// destructed -- and the fill is linear in the highest ref seen, paid once per
		// pass of every scope.
		if (aliveCount != 0) {
			std::fill(counts.begin(), counts.end(), 0);
			aliveCount = 0;
		}
		alive_hash = 0;
	}
};

} // namespace nautilus::tracing
