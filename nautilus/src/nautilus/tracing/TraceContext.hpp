#pragma once

#include "ExecutionTrace.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/options.hpp"
#include "nautilus/tracing/TracingInterface.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "tag/Tag.hpp"
#include "tag/TagRecorder.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus {
class NautilusFunctionDefinition;
}

namespace nautilus::tracing {
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
	/// `engine.normalizeFunctionNames`, read once: it is consulted on every traced call.
	bool normalizeFunctionNames;
	std::unordered_map<void*, uint32_t> normalizedFunctionNameCache; // Maps function pointers to normalized indices
	uint32_t nextNormalizedFunctionIndex = 0;                        // Counter for normalized function names

	TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts);
};

/**
 * @brief Records a symbolic execution trace of a Nautilus function.
 *
 * The traced function is executed once per explored path. When a path reaches a point that
 * was already explored (a fully explored branch, a control-flow merge or a loop back-edge),
 * the context enters "passive mode" instead of aborting the execution: every trace method
 * becomes a no-op that returns dummyRef_, and traceBool() returns false so the function runs
 * to its natural exit. The traced function therefore always returns normally, which keeps
 * the C++ destructors of its locals (and of any val<T> it holds) running in order.
 *
 * Lifecycle:
 * 1. startTrace() pops a function off the work-list and creates its ExecutionTrace,
 *    TagRecorder and SymbolicExecutionContext; TraceState holds references to them.
 * 2. runScope() re-invokes the function once per explored path, calling resume() before
 *    each pass to reset the per-pass state (staticVars, aliveVars, destructors, paused_).
 * 3. Nautilus functions invoked while tracing are appended to the work-list and traced in turn.
 */
class TraceContext final : public TracingInterface {
public:
	// --- TracingInterface overrides ---

	TypedValueRef& registerFunctionArgument(Type type, size_t index) override;
	TypedValueRef& traceConstant(Type type, const ConstantLiteral& value) override;
	TypedValueRef& traceAlloca(size_t size, size_t align) override;
	TypedValueRef& traceCopy(const TypedValueRef& ref) override;
	TypedValueRef& traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
	                             const TypedValueRef& right) override;
	TypedValueRef& traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) override;
	TypedValueRef& traceTernaryOp(Op op, Type resultType, const TypedValueRef& first, const TypedValueRef& second,
	                              const TypedValueRef& third) override;
	void traceReturnOperation(Type type, const TypedValueRef& ref) override;
	void traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) override;
	TypedValueRef& traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                         FunctionAttributes fnAttrs) override;
	TypedValueRef& traceCallWithExceptionHandling(void* fptn, Type resultType,
	                                              const std::vector<tracing::TypedValueRef>& arguments,
	                                              FunctionAttributes fnAttrs, void* captureFunc = nullptr) override;
	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments, FunctionAttributes fnAttrs,
	                                 void* captureFunc = nullptr) override;

	TypedValueRef& traceIndirectCallWithExceptionHandling(const TypedValueRef& fnPtrRef, Type resultType,
	                                                      const std::vector<tracing::TypedValueRef>& arguments,
	                                                      FunctionAttributes fnAttrs,
	                                                      void* captureFunc = nullptr) override;
	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusCallWithExceptionHandling(const NautilusFunctionDefinition* definition,
	                                                      std::function<void()> fwrapper, Type resultType,
	                                                      const std::vector<tracing::TypedValueRef>& arguments,
	                                                      FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;
	bool traceBool(const TypedValueRef& value, double probability) override;
	void traceRegion(std::function<void()>& regionFunction, const RegionAttributes& attributes) override;
	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;
	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;
	void registerDestructor(const TypedValueRef& address, void* destructor) override;
	void unregisterDestructor(const TypedValueRef& address) override;
	void transferDestructor(const TypedValueRef& from, const TypedValueRef& to) override;

	// --- Non-interface public API ---

	~TraceContext() override = default;

	/**
	 * @brief Resets persistent state between trace iterations.
	 * Also resets the paused_ flag.
	 */
	void resume();

	/**
	 * @brief Initialize the thread-local trace context with references to stack-allocated objects
	 * and register it as the active tracer.
	 */
	static TraceContext* initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
	                                SymbolicExecutionContext& symbolicExecutionContext, const engine::Options& options);

	/**
	 * @brief Single-function tracing entry point. Traces @p traceFunction alone; Nautilus
	 * functions it invokes are recorded as calls but not traced.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and TraceOperations;
	 *              must outlive the returned trace.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Multi-function tracing entry point. Traces all functions in the work-list,
	 * including nested Nautilus functions discovered during tracing.
	 * @param functions Initial list of functions to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena backing all traces in the returned module; must
	 *              outlive the returned module.
	 * @return unique_ptr to TraceModule containing all function traces.
	 */
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functions,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functions,
	                                          const engine::Options& options, Arena& arena);

	TraceContext() = default;

private:
	bool isFollowing();
	TypedValueRef& follow(Op op);
	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();
	std::string formatStaticVars() const;
	/// The normalized name ("runtimeFuncN") of the native callee @p fnptr when
	/// `engine.normalizeFunctionNames` is set, empty otherwise. Indices follow the order
	/// callees are first traced in, so this is assigned at trace time; every other name is
	/// resolved once tracing is done (see resolveCalleeNames). Session-wide, so a region
	/// scope numbers a callee the same way its enclosing function does.
	std::string normalizedFunctionName(void* fnptr);

	/// Fills in the symbol and display names of every native callee and destructor recorded
	/// in @p trace. Tracing records only their pointers: resolving a name costs a dladdr
	/// symbol-table scan, which is paid here once per callee instead of once per traced call
	/// site and scope, and is cached process-wide beyond that (see resolveFunctionName). When
	/// !shouldResolveCalleeNames(), a callee is instead named by its address.
	static void resolveCalleeNames(ExecutionTrace& trace, const engine::Options& options);

	/// `engine.resolveFunctionNames`, which defaults to on only when the compiled code will
	/// be inspected: `debug`, `perf` or `perf.sample`.
	static bool shouldResolveCalleeNames(const engine::Options& options);

	/**
	 * @brief Runs the symbolic-execution loop of one *trace scope* to completion.
	 *
	 * A scope is a body that is explored path by path against its own
	 * SymbolicExecutionContext, TagRecorder, staticVars/aliveVars and passive-mode
	 * flag -- i.e. exactly the state this object holds. Tracing a whole function is
	 * running a scope whose entry block is block 0; a region (see docs/region.md) is
	 * the same loop over a scope whose entry block is a freshly created block inside
	 * the enclosing scope's trace. `resetExecution()` is by definition
	 * `setCurrentBlock(0)`, so the two differ only in `entryBlock_`.
	 *
	 * @param body The scope body, re-invoked once per explored path.
	 */
	void runScope(std::function<void()>& body);

	/**
	 * @brief Terminates one completed pass of a region scope.
	 *
	 * A function scope needs no equivalent: its body already emits a tagged RETURN per
	 * completed pass. A region body has no terminator of its own, so this records a
	 * tagged jump to the region's exit block. Because the jump is tagged, a second pass
	 * that ends in a different block hits the same tag and is merged by the ordinary
	 * control-flow-merge machinery -- the same code that merges the arms of an `if`.
	 *
	 * This is also where a value created inside the body that outlives it is rejected;
	 * see the check itself for why that cannot be supported.
	 */
	void traceScopeExit();

	/// Prepares this (possibly pooled) context to trace the body of the region @p attributes
	/// describes, opened by @p parent and recorded into @p parent's trace between @p entry
	/// and @p exit.
	void initRegionScope(TraceContext& parent, uint32_t entry, uint32_t exit, TagRecorder& recorder,
	                     const RegionAttributes& attributes);

	/// Returns the pooled context used for regions opened by this scope, creating it on
	/// first use. Regions nest strictly LIFO and a scope traces at most one region at a
	/// time, so one slot per scope covers a whole nesting chain and each depth's
	/// SymbolicExecutionContext (and its tag map) is allocated once per thread.
	TraceContext& acquireChildScope();

	/// A region recorded in the enclosing trace: the block its body starts in and the
	/// block the enclosing scope continues in afterwards.
	struct RegionRecord {
		uint32_t entryBlock;
		uint32_t exitBlock;
	};

	// Injected state - holds references to the objects of the function or region scope being traced.
	// Empty when not tracing and stored inline to avoid a per-trace heap allocation.
	std::optional<TraceState> state;

	std::vector<FunctionCall::Destructor> activeDestructors;

	/// The block a pass of this scope rewinds to before re-invoking the body.
	/// 0 for a function scope (the trace's own entry block).
	uint32_t entryBlock_ = 0;

	/// Non-null exactly for a region scope: the scope that opened this region.
	/// Read on the freeValRef hot path, so it stays a plain member here.
	TraceContext* parent_ = nullptr;

	/// Everything a scope needs only once region() is involved, held behind one pointer
	/// and allocated on first use.
	///
	/// This is deliberately not inlined into the object. A TraceContext is otherwise
	/// small and its hot members (state, staticVars, aliveVars, paused_) are touched on
	/// every traced operation and every val<T> construction; carrying ~250 bytes of
	/// region state inline pushed them apart and cost 9-13% on tracing benchmarks that
	/// never use a region at all. Functions that use no region never allocate this.
	struct RegionScopeState {
		/// Region scopes only: the block the enclosing scope resumes in.
		uint32_t exitBlock = 0;

		/// Region scopes only: what the region() call site whose body this scope traces
		/// said about itself -- its optional name and its source location (see
		/// docs/region.md). Carried here so a body this scope has to reject can be
		/// reported against the call site the user wrote.
		RegionAttributes attributes;

		/// Region scopes only: the exit snapshot of the first completed pass. Every later
		/// completed pass must agree with it, or what escapes the region would depend on
		/// which path happened to be explored last (see docs/region.md).
		std::optional<Snapshot> exitSnapshot;

		/// The refs still alive at the end of the first completed pass, ascending -- the
		/// values that escape the region. Compared directly against every later pass
		/// rather than relying on exitSnapshot alone: that snapshot folds the escape set
		/// into an XOR hash together with the static-variable hash, so two different
		/// escape sets can collide, and a static-variable change can cancel an escape
		/// change. Comparing the sets also lets the diagnostic name the refs involved.

		/// Regions opened *by* this scope, keyed by their call-site snapshot. Consulted
		/// when this scope replays a recorded path and reaches the region again: the body
		/// is not re-executed, the cursor jumps straight to the region's exit block.
		std::unordered_map<Snapshot, RegionRecord> regionMemo;

		/// The SymbolicExecutionContext this scope's state refers to when it is a region
		/// scope. Owned here (a function scope's is owned by its caller's stack frame) and
		/// reset rather than reconstructed, so a pooled scope keeps its tag map
		/// allocation.
		SymbolicExecutionContext symbolicExecutionContext {kRegionExpectedTags};

		/// TagRecorders of every region traced during the current function; only the
		/// session's copy is used. A TagRecorder's trie root is a member Tag and its nodes
		/// carry pointers to it, while the Tag* it mints are stored in the trace and in
		/// the tag map -- both of which outlive the region. So recorders must live as long
		/// as the trace they tag, not as long as the region engagement, and must not move
		/// once created.
		std::deque<TagRecorder> tagRecorders;

		/// The pooled context used for regions opened by this scope. Regions nest strictly
		/// LIFO and a scope traces at most one region at a time, so one slot per scope
		/// covers a whole nesting chain and each depth's SymbolicExecutionContext (and its
		/// tag map) is allocated once per thread.
		std::unique_ptr<TraceContext> childScope;
	};

	/// Sized for the branches structurally inside one region body, not a whole function.
	static constexpr size_t kRegionExpectedTags = 8;

	/// Returns this scope's region state, allocating it on first use.
	RegionScopeState& regionState();

	/// The context that owns the cross-scope bookkeeping shared by every scope of
	/// one tracing session: the function work-list, the registered-function set and
	/// the normalized function-name cache. Always the outermost context;
	/// `this` for a function scope.
	TraceContext* session_ = this;

	// Persistent state - reset between trace iterations via resume()
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;

	// Passive mode state
	bool paused_ = false;
	// Returned by all trace methods when paused. Safe because callers (val<T> constructors)
	// always copy the TypedValueRef by value — no one holds the reference across calls.
	TypedValueRef dummyRef_ = {0, Type::v};

	// Work-list for multi-function tracing (session-owned; see session_)
	std::list<compiler::CompilableFunction> functionsToTrace;
	/// Definition identity -> the name that definition is traced under.
	///
	/// Keyed on the NautilusFunctionDefinition, not on its name: two distinct
	/// NautilusFunctions may share a name, and deduping by name meant the
	/// second was never traced while every call to it dispatched into the
	/// first one's body, with no diagnostic. The stored name is uniquified
	/// against `usedFunctionNames` so both bodies get traced and emitted.
	std::unordered_map<const void*, std::string> registeredFunctions;
	std::unordered_set<std::string> usedFunctionNames;

	/// Name and registration site of the function whose body is currently being traced
	/// (session-owned; see session_). Read by the "Invalid region()" diagnostics so a
	/// rejected region body says which enclosing function it came from, not just where the
	/// region() call site itself sits.
	std::string currentFunctionName_;
	SourceLocation currentFunctionLocation_;

	/// " in function 'name' (registered at file:line:column)" for the function currently
	/// being traced, or empty before any function has started. Used to extend an
	/// "Invalid region()" diagnostic with the enclosing function's identity.
	std::string describeCurrentFunction() const;

	/// Returns the trace-unique name for @p definition, registering it for
	/// tracing on first sight. @p newlyRegistered reports whether this call
	/// was the first.
	const std::string& registerNautilusFunction(const NautilusFunctionDefinition* definition,
	                                            std::function<void()> fwrapper, bool& newlyRegistered);

	/// Allocated on the first region() this scope opens; null for a scope that never
	/// sees one. Placed after the hot members on purpose -- see RegionScopeState.
	std::unique_ptr<RegionScopeState> regionState_;
};

} // namespace nautilus::tracing
