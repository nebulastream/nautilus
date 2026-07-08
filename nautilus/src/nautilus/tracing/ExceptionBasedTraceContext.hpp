
#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <list>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;
class TraceModule;
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
	 * @param id Variable identifier (32-bit value)
	 */
	inline void decrement(uint32_t id) noexcept {
		uint32_t& c = counts[id];
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
		--c;
		alive_hash ^= (id * HASH_MULTIPLIER) * c;
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
	 * @brief Resets all reference counts and hash to initial state.
	 *
	 * This efficiently clears all counts without creating a temporary object.
	 * Optimized: if hash is already 0, we assume counts are already empty and skip the clear.
	 */
	inline void reset() noexcept {
		if (alive_hash != 0) {
			counts.clear();
			alive_hash = 0;
		}
	}
};

/**
 * @brief Scope-relative view of `AliveVariableHash` keyed on the Tag chain.
 *
 * Stack frames mirror the prefix of the current Tag's parent chain (root → leaf).
 * Each frame captures `aliveVars.hash()` at the moment the chain first reached that
 * depth; the snapshot then computes its alive-vars contribution as `currentHash XOR
 * frames.back().aliveHashOnEntry`, i.e. the delta accumulated since the deepest
 * scope was entered. This makes in-callee CMPs insensitive to caller-side ValueRef
 * ids (collapsing call-site path explosion) while keeping loop iterations within
 * the same scope distinguishable via the growing alive delta.
 */
class ScopeFrameStack {
public:
	struct Frame {
		const Tag* tag_at_depth;
		uint64_t alive_hash_on_entry;
	};

	/**
	 * @brief Reconcile the frame stack with the current Tag's parent chain.
	 *
	 * Truncates frames at the first divergence from the chain prefix, then pushes a
	 * frame for each newly entered depth — capturing @p current_alive_hash as the new
	 * frame's `alive_hash_on_entry`.
	 */
	inline void sync(const Tag* current_tag, uint64_t current_alive_hash) {
		chain_buffer.clear();
		for (const Tag* t = current_tag; t != nullptr && t->getParent() != nullptr; t = t->getParent()) {
			chain_buffer.push_back(t);
		}
		std::reverse(chain_buffer.begin(), chain_buffer.end());
		const size_t depth = chain_buffer.size();

		size_t common = 0;
		while (common < frames.size() && common < depth && frames[common].tag_at_depth == chain_buffer[common]) {
			++common;
		}
		frames.resize(common);

		while (frames.size() < depth) {
			frames.push_back({chain_buffer[frames.size()], current_alive_hash});
		}
	}

	/// Returns the alive-vars hash captured when the innermost scope was entered, or
	/// 0 when the chain is empty (top-of-trace).
	[[nodiscard]] inline uint64_t entryHash() const noexcept {
		return frames.empty() ? 0 : frames.back().alive_hash_on_entry;
	}

	inline void reset() noexcept {
		frames.clear();
		chain_buffer.clear();
	}

private:
	std::vector<Frame> frames;
	std::vector<const Tag*> chain_buffer; // reused across sync() calls
};

/**
 * @brief State that requires initialization for tracing operations.
 * This is allocated when tracing begins and freed when it ends.
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
 * Holds shared state (TraceState, mangledNameCache) and provides
 * getMangledName() / getFunctionName() so they are not duplicated
 * across ExceptionBasedTraceContext and LazyTraceContext.
 */
class TraceContextBase : public TracingInterface {
public:
	~TraceContextBase() override = default;

	std::string getMangledName(void* fnptr);
	std::string getFunctionName(void* fnptr, const std::string& mangledName);

protected:
	// Injected state - holds references to stack-allocated objects (ExecutionTrace, SymbolicExecutionContext)
	// nullptr when not tracing, set during initialize()
	std::unique_ptr<TraceState> state;

	std::unordered_map<void*, std::string> mangledNameCache;
};

/**
 * @brief The trace context manages a thread local instance to record a symbolic execution trace of a given Nautilus
 * function.
 *
 * Design Philosophy:
 * - ExceptionBasedTraceContext is a simple thread_local object (not a pointer) - zero heap allocation
 * - ExecutionTrace and SymbolicExecutionContext are allocated on the stack in trace()
 * - TraceState holds references to these stack objects and is created during initialization
 * - staticVars and aliveVars are persistent members that get reset between trace iterations
 * - Inherits from TraceContextBase so different implementations can be swapped per trace via setActiveTracer().
 *
 * Lifecycle:
 * 1. trace() allocates ExecutionTrace and SymbolicExecutionContext on its stack
 * 2. initialize() creates TraceState with references to these stack objects and registers itself
 *    as the active tracer via setActiveTracer(this)
 * 3. Multiple trace iterations execute, calling resume() to reset persistent state
 * 4. After tracing completes, setActiveTracer(nullptr) is called and ExecutionTrace is returned
 */
class ExceptionBasedTraceContext final : public TraceContextBase {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;

	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;

	/**
	 * @brief Main tracing entry point - allocates all objects on stack and executes symbolic tracing.
	 * @param functionsToTrace List of functions to trace
	 * @param options Engine options for configuration
	 * @param arena Arena used to allocate Blocks/TraceOperations for every
	 *              ExecutionTrace in the returned TraceModule; must outlive
	 *              the returned module.
	 * @return unique_ptr to TraceModule containing all function traces
	 */
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functionsToTrace,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functionsToTrace,
	                                          const engine::Options& options, Arena& arena);

	TypedValueRef& traceCopy(const TypedValueRef& ref) override;

	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override;

	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override;

	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
	void traceReturnOperation(Type type, const TypedValueRef& ref) override;

	void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) override;

	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override;

	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;

	bool traceBool(const TypedValueRef& value, double probability) override;

	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;

	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;

	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;

	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	// --- Non-interface public API ---

	~ExceptionBasedTraceContext() override = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Clears staticVars and resets aliveVars hash/counts.
	 * Does NOT reset state (executionTrace/symbolicExecutionContext) - they persist across iterations.
	 */
	void resume();

	/**
	 * @brief Initialize the trace context with references to stack-allocated objects.
	 * Sets this context as the active tracer via setActiveTracer().
	 * @param tagRecorder Reference to TagRecorder for creating unique tags
	 * @param executionTrace Reference to stack-allocated ExecutionTrace
	 * @param symbolicExecutionContext Reference to stack-allocated SymbolicExecutionContext
	 * @param options Reference to engine options for configuration
	 * @return Pointer to initialized thread_local ExceptionBasedTraceContext
	 */
	static ExceptionBasedTraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                              SymbolicExecutionContext& symbolicExecutionContext,
	                                              const engine::Options& options);

	/**
	 * @brief Main tracing entry point - executes symbolic tracing of the
	 * supplied function.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and
	 *              TraceOperations; must outlive the returned trace.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Default constructor - public to allow thread_local storage.
	 * Initializes with empty state (state == nullptr means not initialized).
	 */
	ExceptionBasedTraceContext() = default;

	bool isActive() const;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	Snapshot recordBranchSnapshot();
	std::string formatStaticVars() const;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars; // Tracks static variable states for snapshot hashing
	AliveVariableHash aliveVars;             // Tracks alive variables with incremental hash (256KB)
	ScopeFrameStack scopeFrames;             // Scope-relative view of aliveVars, keyed on the Tag chain
	std::list<compiler::CompilableFunction> functionsToTrace = std::list<compiler::CompilableFunction> {};
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
