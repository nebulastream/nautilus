#pragma once

#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "tag/TagRecorder.hpp"
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;
struct StaticVarHolder {
	explicit StaticVarHolder(const void* ptr, size_t size) : ptr(ptr), size(size) {
	}

private:
	const void* ptr;
	size_t size;
	friend uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);
	friend size_t getStaticVarValue(const StaticVarHolder& holder);
};

inline size_t getStaticVarValue(const StaticVarHolder& holder) {
	size_t result = 0;
	std::memcpy(&result, holder.ptr, holder.size);
	return result;
}

/// FNV-1a hash of a static-variable stack snapshot. Exposed (not file-local) so
/// LazyTraceContext's currentStateHash() override can hash a region frame's own
/// staticVars the same way TraceContextBase::currentStateHash() hashes the root's.
uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data);

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
		assert(id < counts.size() && counts[id] > 0 && "decrement() on a variable that is not alive");
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
	 * @brief Invokes @p fn with each alive variable id and its reference count.
	 *
	 * Id is the index into the backing vector; slots with a zero count (never seen, or
	 * decremented back to zero) are skipped, so the callback is called once per
	 * currently-alive variable with its current reference count.
	 *
	 * @param fn Callback invoked as fn(id, count) for each alive variable
	 */
	template <typename F>
	inline void forEachAliveRef(F&& fn) const noexcept {
		for (size_t id = 0; id < counts.size(); ++id) {
			if (counts[id] != 0) {
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
		std::fill(counts.begin(), counts.end(), 0);
		aliveCount = 0;
		alive_hash = 0;
	}
};

/**
 * @brief View over the environment (static variables + alive variables) of a trace context.
 * Holds references to the owning context's storage, which lives exactly once in the base.
 */
struct TraceEnv {
	std::vector<StaticVarHolder>& staticVars;
	AliveVariableHash& aliveVars;
};

struct RegionExec {
	ValueRef continuationOperationIndex;
	uint32_t continuationBlockIndex;
};

using RegionMemo = std::unordered_map<uint64_t, RegionExec>;

/**
 * @brief State that requires initialization for tracing operations.
 * This is initialized in the trace context when tracing begins and reset when it ends.
 * Holds references to stack-allocated objects.
 */
struct TraceState {
	TagRecorder& tagRecorder;
	ExecutionTrace& executionTrace;
	SymbolicExecutionContext& symbolicExecutionContext;
	const engine::Options& options;
	std::unordered_map<void*, uint32_t> normalizedFunctionNameCache; // Maps function pointers to normalized indices
	uint32_t nextNormalizedFunctionIndex = 0;                        // Counter for normalized function names

	TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts);
};

/**
 * @brief Common base class for trace context implementations.
 *
 * Holds shared tracing state (TraceState, mangledNameCache, staticVars, aliveVars) and
 * provides getMangledName() / getFunctionName() / currentStateHash() plus the static-variable
 * stack management so they are not duplicated across ExceptionBasedTraceContext and
 * LazyTraceContext.
 */
class TraceContextBase : public TracingInterface {
public:
	~TraceContextBase() override = default;

	std::string getMangledName(void* fnptr);
	std::string getFunctionName(void* fnptr, const std::string& mangledName);

	virtual uint64_t currentStateHash() const;

	TraceEnv& getEnv() override;

	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	/// Returns the thread-local state's execution trace (the root's shared trace).
	ExecutionTrace& getExecutionTrace();

	/// Per-call-site tag recorders and per-call-site region memos. Owned by the
	/// root context and shared with all of its regions. TagRecorder is stored by
	/// value (cleared with the map) so recorders are freed between trace
	/// iterations instead of leaking on the heap.
	std::unordered_map<TagAddress, TagRecorder> regionRecorders;
	std::unordered_map<TagAddress, RegionMemo> regionMemos;

protected:
	// Injected state - holds references to stack-allocated objects (ExecutionTrace, SymbolicExecutionContext).
	// Empty when not tracing and stored inline to avoid a per-trace heap allocation.
	std::optional<TraceState> state;

	std::unordered_map<void*, std::string> mangledNameCache;

	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
	// View over staticVars/aliveVars - the single storage location is the two members above.
	TraceEnv env {staticVars, aliveVars};

	std::string formatStaticVars() const;
};

} // namespace nautilus::tracing
