#pragma once

#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "tag/TagRecorder.hpp"
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

/**
 * @brief Efficiently tracks reference counts and computes an incremental hash of alive variables.
 *
 * This class maintains reference counts using a sparse hash map to support the full uint32_t range
 * of variable IDs while keeping memory usage reasonable. The hash reflects both which variables
 * are alive and their reference counts, updated incrementally in O(1) average time.
 *
 * Implementation details:
 * - Uses XOR-based hashing for O(1) incremental updates
 * - Each variable ID is mixed with a constant multiplier for better hash distribution
 * - The hash incorporates both variable identity (ID) and reference count
 * - Uses unordered_map for sparse storage (only allocates for variables that exist)
 * - Entries are erased when their count reaches zero, so the map stays bounded by the
 *   peak number of alive variables instead of growing with every value ref ever seen
 *
 * Performance characteristics:
 * - increment(): O(1) average - hash map lookup + two XOR operations, two multiplications
 * - decrement(): O(1) average - hash map lookup + two XOR operations, two multiplications
 * - hash(): O(1) - returns cached value
 *
 * @note Changed from fixed array to hash map to support uint32_t ValueRef (was uint16_t).
 */
class AliveVariableHash {
	static constexpr uint64_t HASH_MULTIPLIER = 0x9e3779b97f4a7c15; // Golden ratio constant for good mixing

	std::unordered_map<uint32_t, uint32_t> counts;
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
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		++c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
	}

	/**
	 * @brief Decrements the reference count for a variable and updates the hash.
	 *
	 * The hash is updated by XOR-ing out the old contribution ((id * HASH_MULTIPLIER) * old_count)
	 * and XOR-ing in the new contribution ((id * HASH_MULTIPLIER) * new_count).
	 *
	 * Entries that reach a count of zero are erased. This is hash-neutral (a zero count
	 * contributes 0 to the XOR hash, identical to an absent entry) and keeps the map size
	 * proportional to the number of currently-alive variables. Without the erase, the map
	 * grows with every value ref ever seen and — because the trace context is thread_local —
	 * persists across trace iterations and across traced functions.
	 *
	 * @param id Variable identifier (32-bit value)
	 */
	inline void decrement(uint32_t id) noexcept {
		const auto it = counts.try_emplace(id, 0).first;
		uint32_t& c = it->second;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		--c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		if (c == 0) {
			counts.erase(it);
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
	 * @brief Returns the number of tracked entries.
	 *
	 * Since zero-count entries are erased by decrement(), this is the number of
	 * currently-alive variables (variables with a non-zero reference count).
	 *
	 * @return Number of entries in the underlying map
	 */
	inline size_t size() const noexcept {
		return counts.size();
	}

	/**
	 * @brief Invokes @p fn with each alive variable id and its reference count.
	 *
	 * Zero-count entries are erased by decrement(), so every entry visited has a
	 * positive count. The callback is called once per alive variable with its
	 * current reference count.
	 *
	 * @param fn Callback invoked as fn(id, count) for each alive variable
	 */
	template <typename F>
	inline void forEachAliveRef(F&& fn) const noexcept {
		for (const auto& entry : counts) {
			fn(entry.first, entry.second);
		}
	}

	/**
	 * @brief Resets all reference counts and hash to initial state.
	 *
	 * This efficiently clears all counts without creating a temporary object.
	 * Since decrement() erases entries when they reach zero, a balanced trace iteration
	 * leaves the map empty and this is a no-op. The emptiness check (rather than a hash
	 * check) also guards against the rare XOR collision where live entries hash to 0.
	 */
	inline void reset() noexcept {
		if (!counts.empty()) {
			counts.clear();
		}
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

	uint64_t currentStateHash() const;

	TraceEnv& getEnv() override;

	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	/// Returns the thread-local state's execution trace (the root's shared trace).
	ExecutionTrace& getExecutionTrace();

	/// Per-call-site tag recorders and per-call-site region memos. Owned by the
	/// root context and shared with all of its regions.
	std::unordered_map<TagAddress, TagRecorder*> regionRecorders;
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
