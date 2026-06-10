
#pragma once

#include "ExceptionBasedTraceContext.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "replay/ConstantTracker.hpp"
#include "replay/ReplayLog.hpp"
#include <functional>
#include <list>
#include <memory>
#include <optional>
#include <unordered_set>

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;

/**
 * @brief Log-based, exception-free tracing context.
 *
 * ReplayTraceContext keeps the re-execution model of the other contexts but
 * replaces the replay and scheduling layers (see
 * docs/replay-trace-context-design.md for the full design):
 *
 * - Every interface call made in RECORD mode appends a LogEntry (op kind,
 *   returned ref, operand hash) to a flat per-path log. Re-executions replay
 *   the recorded prefix from this log with an O(1) cursor (FOLLOW mode) -
 *   no snapshots, no tag lookups, no block-graph access - and validate each
 *   call against the log, turning non-determinism into a precise
 *   TraceDivergenceException instead of a corrupt trace.
 * - Branch identity is the position in the log instead of a Snapshot hash.
 *   Each recorded CMP pushes exactly one PendingBranch; the LIFO worklist
 *   (depth-first exploration) with log truncation guarantees each dynamic
 *   branch is recorded exactly once.
 * - Conditions that are trace-time constants (tracked by ConstantTracker)
 *   are pruned: no CMP is recorded and the statically impossible side is
 *   never executed in any iteration.
 * - Path termination uses passive mode (like LazyTraceContext) so no
 *   exception ever unwinds through user code on a normal path. The passive
 *   step limit and the exploration iteration limit are enforced and raise
 *   TraceIncompleteException / TraceLimitException.
 *
 * RECORD mode reuses the existing snapshot/tag machinery of ExecutionTrace
 * unchanged (operation dedup, control-flow merges, loop closure).
 *
 * Selected via the engine option engine.traceMode = "replayTracing".
 * Additional options: engine.traceMaxIterations (int, default 100000),
 * engine.tracePassiveStepLimit (int, default 1000000),
 * engine.traceValidateReplay (bool, default true).
 */
class ReplayTraceContext final : public TraceContextBase {
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
	TypedValueRef& traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
	                                 const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusCall(const NautilusFunctionDefinition* definition, std::function<void()> fwrapper,
	                                 Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
	                                 FunctionAttributes fnAttrs) override;
	TypedValueRef& traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
	                                        std::function<void()> fwrapper) override;
	bool traceBool(const TypedValueRef& value, double probability) override;
	void allocateValRef(ValueRef ref) override;
	void freeValRef(ValueRef ref) override;
	void pushStaticVal(void* ptr, size_t size) override;
	void popStaticVal() override;

	// --- Non-interface public API ---

	~ReplayTraceContext() override = default;
	ReplayTraceContext() = default;

	/**
	 * @brief Single-function tracing entry point.
	 * @param traceFunction The function to trace.
	 * @param options Engine options for configuration.
	 * @param arena Arena used to allocate the trace's Blocks and
	 *              TraceOperations; must outlive the returned trace.
	 * @return unique_ptr to ExecutionTrace containing the complete trace.
	 */
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/**
	 * @brief Multi-function tracing entry point. Traces all functions in the
	 * work-list, including nested Nautilus functions discovered during tracing.
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

private:
	enum class Mode : uint8_t { Record, Follow, Passive };
	static constexpr uint32_t kNoFlip = UINT32_MAX;

	/// Validates the FOLLOW-mode log entry at the cursor; on mismatch latches
	/// the divergence and enters passive mode. Returns the entry, or nullptr.
	LogEntry* checkFollowEntry(Op op, uint64_t operandHash, bool wantsResult);
	TypedValueRef& followValue(Op op, uint64_t operandHash);

	void enterPassive();
	void latchDivergence(Op actualOp, uint64_t actualHash);
	Snapshot recordSnapshot();
	/// Snapshot variant that mixes a constness fingerprint (the folded literal
	/// of the operation's result, or a Const source for copies/assignments)
	/// into the hash, so operations in trace-time-constant regions never
	/// collide with runtime re-executions of the same site.
	Snapshot recordSnapshot(const std::optional<ConstantLiteral>& constFingerprint);
	void readOptions(const engine::Options& options);

	/// Runs the exploration driver loop for the current function: iteration 0
	/// records the spine, then each pending branch is replayed and flipped.
	void runExplorationLoop(const std::function<void()>& wrapperFunc, const std::string& functionName);
	void resetIterationState(Mode startMode);
	void checkPostIteration(const std::string& functionName, uint64_t iteration);

	bool wantsOperandHash() const {
		return mode == Mode::Record || validateReplay;
	}

	// --- per-function exploration state ---
	std::vector<LogEntry> pathLog;
	std::vector<PendingBranch> worklist; // LIFO => depth-first exploration
	ConstantTracker constants;

	/// Registers a pruned branch instance for the current iteration and
	/// updates the poison hash mixed into every subsequent snapshot.
	void addPrunedSite(uint64_t key);

	// --- per-iteration state ---
	Mode mode = Mode::Record;
	uint32_t cursor = 0;
	uint32_t flipPos = kNoFlip;
	uint32_t pendingFalseBlock = 0;
	uint64_t passiveSteps = 0;
	std::optional<DivergenceInfo> divergence;
	// Set of branch instances pruned so far in this iteration (keyed by site
	// tag + static-state hash + decision), and its accumulated hash. The hash
	// poisons every snapshot taken after a prune so that operations recorded
	// under pruned (constant) control flow can never tag-collide with runtime
	// re-executions of the same sites - otherwise loop closure would freeze
	// pruned control flow into a loop body (see section 6 of
	// docs/replay-trace-context-design.md). Set semantics guarantee that the
	// poison stabilizes for recurring prunes of the same instance, so loop
	// closure still terminates; distinct static states (unrolled static_val
	// loops) yield distinct instances.
	std::unordered_set<uint64_t> prunedSites;
	uint64_t prunedSitesHash = 0;

	// --- configuration (read once per startTrace) ---
	uint64_t maxIterations = 100000;
	uint64_t passiveStepLimit = 1000000;
	bool validateReplay = true;

	// --- persistent state shared with the other contexts' design ---
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
	// Returned by all trace methods in passive mode and on divergence. Safe
	// because callers (val<T> constructors) always copy the TypedValueRef by
	// value - no one holds the reference across calls.
	TypedValueRef dummyRef_ = {0, Type::v};

	// Work-list for multi-function tracing
	std::list<compiler::CompilableFunction> functionsToTrace;
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
