
#pragma once

#include "ExceptionBasedTraceContext.hpp"
#include "nautilus/CompilableFunction.hpp"
#include <deque>
#include <functional>
#include <list>
#include <memory>
#include <unordered_set>

namespace nautilus::tracing {
class ExecutionTrace;
class SymbolicExecutionContext;

namespace serialization {
struct ByteWriter;
struct ByteReader;
class SharedHandoffRegion;
} // namespace serialization

/**
 * @brief Tracing context that executes every path through a function exactly once by
 * snapshotting the whole process with fork() at each recorded branch.
 *
 * Where StackCopyTraceContext saves and restores stack bytes in-process, this tracer
 * lets the operating system do the snapshot: at every freshly recorded CMP the process
 * forks. The child is the suspended false-branch continuation - its copied address
 * space already contains the correct stack, heap, static-variable and alive-variable
 * state, so nothing is saved explicitly. The parent explores the true branch. Pending
 * continuations are resumed in FIFO order of their CMP discovery (matching the other
 * tracers, so the produced traces are byte-identical to the reference traces); since
 * the grow-only trace state (blocks, operations, tag maps) accumulated by other
 * processes after a continuation forked is missing from its address space, it is
 * serialized and handed over through a pipe on every resume (see TraceSerialization).
 *
 * Compared to stackCopyTracing this is slower (fork + serialization per branch) but
 * fully isolates the traced code: destructors of heap-owning plain C++ locals run
 * exactly once per process, so code that is unsafe under stack restores works here.
 *
 * Constraints: POSIX only; tracing must run single-threaded (forking a process with
 * concurrently allocating threads risks deadlocking the children).
 *
 * Selected via the engine option "engine.traceMode" = "forkTracing".
 */
class ForkTraceContext final : public TraceContextBase {
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

	// Out-of-line: the unique_ptr members require complete types at destruction.
	~ForkTraceContext() override;
	ForkTraceContext() = default;

	/// Resets persistent state between traced functions.
	void resume();

	/// Single-function tracing entry point (benchmark harness signature).
	static std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options,
	                                             Arena& arena);

	/// Multi-function tracing entry point. Traces all functions in the work-list,
	/// including nested Nautilus functions discovered during tracing.
	std::unique_ptr<TraceModule> startTrace(std::list<compiler::CompilableFunction>& functions,
	                                        const engine::Options& options, Arena& arena);
	static std::unique_ptr<TraceModule> Trace(std::list<compiler::CompilableFunction>& functions,
	                                          const engine::Options& options, Arena& arena);

private:
	/// A suspended false-branch continuation: a forked child blocked on its socket.
	struct PendingContinuation {
		int pid = -1;
		// Write end through which the resumer hands over the accumulated trace state.
		// File descriptors of continuations forked later do not exist in earlier
		// children, so they are transferred via SCM_RIGHTS alongside the state blob.
		int handoverFd = -1;
		// Trace-mutation epoch at the continuation's fork. Its copied address space
		// holds the trace exactly as of this epoch, so a resume only needs to ship
		// the mutations that are newer (see serialization::serializeTraceDelta).
		uint64_t forkEpoch = 0;
	};

	/// A nested function discovered during tracing. Name and attributes are captured
	/// at discovery time (while the definition is certainly valid in the discovering
	/// process); the wrapper pointer refers into the static NautilusFunction object,
	/// which lives at the same address in every process of the same binary.
	struct WorklistAddition {
		std::string name;
		std::unordered_map<std::string, std::string> attributes;
		const std::function<void()>* wrapper = nullptr;
	};

	template <typename OnCreation>
	TypedValueRef& traceOperation(Op op, OnCreation&& onCreation);
	Snapshot recordSnapshot();

	/// Runs the per-function exploration in a forked worker tree; called in the root.
	void traceFunctionInWorkerTree(std::function<void()>& wrapper);
	/// Worker side: explores paths, forking a continuation at every recorded CMP.
	/// Never returns - every path ends in finishPath() or an error _exit.
	void runWorker(std::function<void()>& wrapper);
	/// Worker side: called when the current path terminated; hands the trace state to
	/// the oldest pending continuation (or the final result to the root) and exits.
	[[noreturn]] void finishPath();
	/// Worker side: sends an Error message to the root (best effort) and exits.
	[[noreturn]] void reportErrorAndExit(const std::string& message);
	/// Continuation side: blocks until resumed, adopts the handed-over state and
	/// re-positions the trace at the CMP identified by @p cmpTag.
	void awaitResume(int receiveFd, const Snapshot& cmpTag);
	/// Serializes/deserializes the per-function exploration state that always
	/// travels whole (work-list discoveries, normalized-name cache, path budget).
	/// The trace itself follows separately: as an epoch delta on path handoffs,
	/// or in full for the final result to the root.
	void serializeWorkerMeta(serialization::ByteWriter& writer);
	void deserializeWorkerMeta(serialization::ByteReader& reader);
	/// Stages @p body in the shared handoff region when it fits; returns whether it
	/// did. On false the caller sends the body through the socket instead.
	bool stagePayload(const serialization::ByteWriter& body);

	// Persistent per-path state
	std::vector<StaticVarHolder> staticVars;
	AliveVariableHash aliveVars;
	bool paused_ = false;
	TypedValueRef dummyRef_ = {0, Type::v};

	/// Validates that @p definition can be referenced across processes; throws a
	/// descriptive error for NautilusFunction objects local to the traced function.
	void checkDefinitionIsShareable(const NautilusFunctionDefinition* definition);

	// Process orchestration (worker side)
	std::deque<PendingContinuation> pendingContinuations_;
	std::vector<WorklistAddition> worklistAdditions_;
	int resultFd_ = -1;
	uint64_t pathCount_ = 0;
	ExecutionTrace* currentTrace_ = nullptr;
	const engine::Options* currentOptions_ = nullptr;
	std::unique_ptr<TagRecorder> tagRecorder_;
	std::unique_ptr<SymbolicExecutionContext> symbolicExecutionContext_;
	// Created in the root before the first fork, so every process of every worker
	// tree inherits the mapping at the same address. Null when the mmap failed;
	// handoffs then fall back to sending payloads through the socket.
	std::unique_ptr<serialization::SharedHandoffRegion> handoffRegion_;

	// Work-list for multi-function tracing (drained by the root process only)
	std::list<compiler::CompilableFunction> functionsToTrace;
	std::unordered_set<std::string> registeredFunctions;
};

} // namespace nautilus::tracing
