
#include "ForkTraceContext.hpp"
#include "TraceOperation.hpp"
#include "TraceSerialization.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <cerrno>
#include <climits>
#include <csignal>
#include <cstring>
#include <fmt/format.h>
#include <pthread.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <utility>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

namespace {

enum class MessageKind : uint8_t {
	PathHandoff = 0,
	FinalResult = 1,
	Error = 2,
};

/// Raises the file-descriptor limit while a worker tree is running: every pending
/// continuation costs its creator one socket descriptor, so deep branch chains can
/// exceed the conservative default soft limit.
class FdLimitGuard {
public:
	FdLimitGuard() {
		if (getrlimit(RLIMIT_NOFILE, &original_) != 0) {
			return;
		}
		rlim_t cap = original_.rlim_max;
#ifdef __APPLE__
		// macOS rejects soft limits above OPEN_MAX even when the hard limit is
		// unlimited.
		cap = std::min<rlim_t>(cap, OPEN_MAX);
#endif
		rlim_t target = std::min<rlim_t>(65536, cap);
		if (original_.rlim_cur >= target) {
			return;
		}
		rlimit raised = original_;
		raised.rlim_cur = target;
		if (setrlimit(RLIMIT_NOFILE, &raised) == 0) {
			raised_ = true;
		}
	}
	~FdLimitGuard() {
		if (raised_) {
			setrlimit(RLIMIT_NOFILE, &original_);
		}
	}
	FdLimitGuard(const FdLimitGuard&) = delete;
	FdLimitGuard& operator=(const FdLimitGuard&) = delete;
	FdLimitGuard(FdLimitGuard&&) = delete;
	FdLimitGuard& operator=(FdLimitGuard&&) = delete;

private:
	rlimit original_ {};
	bool raised_ = false;
};

} // namespace

namespace {

/// Disables SIGPIPE on @p fd where supported (macOS has no reliable MSG_NOSIGNAL;
/// a SIGPIPE would kill a tracing process silently and mask the actual error).
void disableSigpipe([[maybe_unused]] int fd) {
#ifdef SO_NOSIGPIPE
	int enable = 1;
	setsockopt(fd, SOL_SOCKET, SO_NOSIGPIPE, &enable, sizeof(enable));
#endif
}

/// Detects whether @p address lies on the current thread's stack. Definitions on the
/// stack belong to NautilusFunction objects constructed inside the traced function;
/// their addresses are meaningless in other processes of the worker tree.
bool isOnCurrentThreadStack(const void* address) {
#if defined(__APPLE__)
	auto* top = pthread_get_stackaddr_np(pthread_self());
	auto size = pthread_get_stacksize_np(pthread_self());
	const auto* bottom = static_cast<const char*>(top) - size;
	return address >= static_cast<const void*>(bottom) && address < top;
#else
	pthread_attr_t attributes;
	if (pthread_getattr_np(pthread_self(), &attributes) != 0) {
		return false;
	}
	void* stackAddress = nullptr;
	size_t stackSize = 0;
	pthread_attr_getstack(&attributes, &stackAddress, &stackSize);
	pthread_attr_destroy(&attributes);
	return address >= stackAddress && address < static_cast<void*>(static_cast<char*>(stackAddress) + stackSize);
#endif
}

} // namespace

// Thread-local ForkTraceContext object (not a pointer)
static thread_local ForkTraceContext forkTraceContext;

void ForkTraceContext::checkDefinitionIsShareable(const NautilusFunctionDefinition* definition) {
	if (isOnCurrentThreadStack(definition)) {
		throw RuntimeException("forkTracing cannot trace the nested function '" + definition->name() +
		                       "': its NautilusFunction object is constructed inside the traced function and does "
		                       "not exist in other tracing processes. Declare the NautilusFunction static (the "
		                       "documented usage) or select another engine.traceMode.");
	}
	if (definition->traceWrapper() == nullptr) {
		throw RuntimeException("forkTracing cannot trace the nested function '" + definition->name() +
		                       "': its definition has no reconstructible trace wrapper.");
	}
}

ForkTraceContext::~ForkTraceContext() = default;

void ForkTraceContext::resume() {
	staticVars.clear();
	aliveVars.reset();
	paused_ = false;
	resetPruningState();
	pendingContinuations_.clear();
	worklistAdditions_.clear();
	pathCount_ = 0;
}

TypedValueRef& ForkTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (paused_) {
		return dummyRef_;
	}
	return state->executionTrace.setArgument(type, index);
}

TypedValueRef& ForkTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (paused_) {
		return dummyRef_;
	}
	auto op = Op::CONST;
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto* originalRef = state->executionTrace.getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		state->executionTrace.addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		trackConstant(originalRef->resultRef, type, constValue);
		return originalRef->resultRef;
	} else {
		auto& resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		trackConstant(resultRef, type, constValue);
		return resultRef;
	}
}

template <typename OnCreation>
TypedValueRef& ForkTraceContext::traceOperation([[maybe_unused]] Op op, OnCreation&& onCreation) {
	if (paused_) {
		return dummyRef_;
	}
	auto tag = recordSnapshot();
	if (state->executionTrace.checkTag(tag)) {
		return onCreation(tag);
	}
	// Control-flow merge or loop detected: finish this path passively.
	paused_ = true;
	return dummyRef_;
}

TypedValueRef& ForkTraceContext::traceAlloca(size_t size, size_t align) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return traceOperation(op, [&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto index = state->executionTrace.addAllocaSpec(size, align);
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	});
}

TypedValueRef& ForkTraceContext::traceCopy(const TypedValueRef& ref) {
	if (paused_) {
		return dummyRef_;
	}
	auto& resultRef = traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto newRef = state->executionTrace.getNextValueRef();
		return state->executionTrace.addAssignmentOperation(tag, {newRef, ref.type}, ref, ref.type);
	});
	if (!paused_) {
		trackAssignment(resultRef, ref, ref.type);
	}
	return resultRef;
}

TypedValueRef& ForkTraceContext::traceCall(void* fptn, Type resultType,
                                           const std::vector<tracing::TypedValueRef>& arguments,
                                           FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto op = Op::CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = mangledName,
		                                                                        .ptr = fptn,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& ForkTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                   const std::vector<tracing::TypedValueRef>& arguments,
                                                   FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	auto op = Op::INDIRECT_CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* indirectCall = state->executionTrace.getArena().create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = fnPtrRef, .arguments = arguments, .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {indirectCall});
	});
}

TypedValueRef& ForkTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                   std::function<void()> fwrapper, Type resultType,
                                                   const std::vector<tracing::TypedValueRef>& arguments,
                                                   FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	(void) fwrapper; // work-list entries are reconstructed from the definition's wrapper
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		checkDefinitionIsShareable(definition);
		worklistAdditions_.push_back({functionName, definition->attributes(), definition->traceWrapper()});
	}
	auto op = Op::CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& ForkTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                          std::function<void()> fwrapper) {
	if (paused_) {
		return dummyRef_;
	}
	(void) fwrapper;
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		checkDefinitionIsShareable(definition);
		worklistAdditions_.push_back({functionName, definition->attributes(), definition->traceWrapper()});
	}
	auto op = Op::FUNC_ADDR;
	auto resultType = Type::ptr;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = {},
		                                                                        .fnAttrs = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void ForkTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (paused_) {
		return;
	}
	trackAssignment(target, source, resultType);
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addAssignmentOperation(tag, target, source, resultType);
	});
}

void ForkTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		return;
	}
	auto tag = recordSnapshot();
	state->executionTrace.addReturn(tag, resultType, ref);
}

TypedValueRef& ForkTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                               const TypedValueRef& right) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	});
}

TypedValueRef& ForkTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	});
}

TypedValueRef& ForkTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                const TypedValueRef& second, const TypedValueRef& third) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

bool ForkTraceContext::traceBool(const TypedValueRef& valueRef, const double probability) {
	if (paused_) {
		// In passive mode, return false to guarantee loop termination.
		return false;
	}

	auto tag = recordSnapshot();
	if (bool constantResult; findConstantBranch(valueRef, &constantResult)) {
		guardPrunedTag(tag);
		return constantResult;
	}
	if (!state->executionTrace.checkTag(tag)) {
		// Control flow merge/loop detected. Finish the path passively.
		paused_ = true;
		return false;
	}
	state->executionTrace.addCmpOperation(tag, valueRef, probability);

	int handoverPair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, handoverPair) != 0) {
		throw RuntimeException("forkTracing: failed to create a continuation socket: " +
		                       std::string(std::strerror(errno)));
	}
	disableSigpipe(handoverPair[0]);
	disableSigpipe(handoverPair[1]);
	auto childPid = fork();
	if (childPid < 0) {
		::close(handoverPair[0]);
		::close(handoverPair[1]);
		throw RuntimeException("forkTracing: fork failed: " + std::string(std::strerror(errno)));
	}
	if (childPid == 0) {
		// This process is the suspended false-branch continuation. Its copied address
		// space already holds the correct stack, heap, static-variable and
		// alive-variable state; only the grow-only trace state accumulated by other
		// processes arrives with the resume message.
		::close(handoverPair[1]);
		awaitResume(handoverPair[0], tag);
		::close(handoverPair[0]);
		auto& currentOperation = state->executionTrace.getCurrentOperation();
		assert(currentOperation.op == CMP);
		state->executionTrace.setCurrentBlock(std::get<BlockRef*>(currentOperation.input[2])->block);
		return false;
	}
	// Parent: remember the continuation and explore the true branch.
	::close(handoverPair[0]);
	pendingContinuations_.push_back({childPid, handoverPair[1]});
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);
	state->executionTrace.setCurrentBlock(std::get<BlockRef*>(currentOperation.input[1])->block);
	return true;
}

void ForkTraceContext::serializeWorkerState(serialization::ByteWriter& writer) {
	auto& trace = state->executionTrace;
	// Equivalent of ExecutionTrace::resetExecution() between the iterations of the
	// re-executing tracers.
	trace.globalTagMap.merge(trace.localTagMap);
	trace.localTagMap.clear();
	serialization::serializeTraceState(writer, trace);

	writer.write<uint64_t>(pathCount_);

	writer.write<uint64_t>(worklistAdditions_.size());
	for (const auto& addition : worklistAdditions_) {
		writer.writeString(addition.name);
		writer.write<uint64_t>(addition.attributes.size());
		for (const auto& [key, value] : addition.attributes) {
			writer.writeString(key);
			writer.writeString(value);
		}
		writer.write<const void*>(addition.wrapper);
	}

	writer.write<uint64_t>(state->normalizedFunctionNameCache.size());
	for (const auto& [ptr, index] : state->normalizedFunctionNameCache) {
		writer.write<void*>(ptr);
		writer.write<uint32_t>(index);
	}
	writer.write<uint32_t>(state->nextNormalizedFunctionIndex);
}

void ForkTraceContext::deserializeWorkerState(serialization::ByteReader& reader) {
	auto& trace = state->executionTrace;
	serialization::deserializeTraceState(reader, trace, *tagRecorder_);

	pathCount_ = reader.read<uint64_t>();

	auto worklistSize = reader.read<uint64_t>();
	worklistAdditions_.clear();
	worklistAdditions_.reserve(worklistSize);
	for (uint64_t i = 0; i < worklistSize; i++) {
		WorklistAddition addition;
		addition.name = reader.readString();
		auto attributeCount = reader.read<uint64_t>();
		for (uint64_t a = 0; a < attributeCount; a++) {
			auto key = reader.readString();
			addition.attributes[key] = reader.readString();
		}
		addition.wrapper = static_cast<const std::function<void()>*>(reader.read<const void*>());
		registeredFunctions.insert(addition.name);
		worklistAdditions_.push_back(std::move(addition));
	}

	auto cacheSize = reader.read<uint64_t>();
	state->normalizedFunctionNameCache.clear();
	for (uint64_t i = 0; i < cacheSize; i++) {
		auto* ptr = reader.read<void*>();
		auto index = reader.read<uint32_t>();
		state->normalizedFunctionNameCache[ptr] = index;
	}
	state->nextNormalizedFunctionIndex = reader.read<uint32_t>();
}

void ForkTraceContext::finishPath() {
	if (!pendingContinuations_.empty()) {
		auto next = pendingContinuations_.front();
		pendingContinuations_.pop_front();

		// The remaining pending continuations travel with the handoff: their pids in
		// the payload, their descriptors via SCM_RIGHTS (a continuation forked before
		// a younger sibling never inherited the sibling's descriptor).
		//
		// Protocol order matters: a small header and the descriptors go FIRST, the
		// large state payload last. Ancillary (SCM_RIGHTS) sends fail with EMSGSIZE
		// on some kernels (xnu) when the peer's receive buffer is occupied instead
		// of blocking like plain data, so the descriptors must hit an empty socket.
		serialization::ByteWriter header;
		header.write<uint8_t>(static_cast<uint8_t>(MessageKind::PathHandoff));
		header.write<uint64_t>(pendingContinuations_.size());
		serialization::writeMessage(next.handoverFd, header.buffer);

		std::vector<int> descriptors;
		descriptors.reserve(pendingContinuations_.size());
		for (const auto& pending : pendingContinuations_) {
			descriptors.push_back(pending.handoverFd);
		}
		serialization::sendFds(next.handoverFd, descriptors.data(), descriptors.size());

		serialization::ByteWriter body;
		serializeWorkerState(body);
		for (const auto& pending : pendingContinuations_) {
			body.write<int32_t>(pending.pid);
		}
		serialization::writeMessage(next.handoverFd, body.buffer);
		_exit(0);
	}

	serialization::ByteWriter writer;
	writer.write<uint8_t>(static_cast<uint8_t>(MessageKind::FinalResult));
	serializeWorkerState(writer);
	serialization::writeMessage(resultFd_, writer.buffer);
	_exit(0);
}

void ForkTraceContext::awaitResume(int receiveFd, const Snapshot& cmpTag) {
	auto headerPayload = serialization::readMessage(receiveFd);
	serialization::ByteReader header {headerPayload};
	auto kind = static_cast<MessageKind>(header.read<uint8_t>());
	if (kind != MessageKind::PathHandoff) {
		throw RuntimeException("forkTracing: unexpected message kind on continuation resume");
	}
	auto pendingCount = header.read<uint64_t>();

	// Descriptors inherited from the fork parent's pending deque are superseded by
	// the ones arriving with the handoff; close them to avoid descriptor leaks.
	for (const auto& pending : pendingContinuations_) {
		::close(pending.handoverFd);
	}
	pendingContinuations_.clear();

	std::vector<int> descriptors(pendingCount);
	serialization::receiveFds(receiveFd, descriptors.data(), pendingCount);

	auto payload = serialization::readMessage(receiveFd);
	serialization::ByteReader reader {payload};
	deserializeWorkerState(reader);
	if (++pathCount_ > SymbolicExecutionContext::MAX_ITERATIONS) {
		throw RuntimeException("Tracing got lost and reached the max number of iterations.");
	}

	for (uint64_t i = 0; i < pendingCount; i++) {
		auto pid = reader.read<int32_t>();
		pendingContinuations_.push_back({pid, descriptors[i]});
	}

	auto& trace = state->executionTrace;
	// Re-position at the CMP we forked at. Look it up by tag: control-flow merges of
	// previously explored paths may have moved the operation.
	auto cmpPosition = trace.globalTagMap.find(cmpTag);
	if (cmpPosition == trace.globalTagMap.end()) {
		throw RuntimeException("forkTracing: pending branch continuation lost its CMP operation");
	}
	trace.currentBlockIndex = cmpPosition->second.blockIndex;
	trace.currentOperationIndex = cmpPosition->second.operationIndex;

	// FOLLOW-replay parity: the re-executing tracers re-run the function prologue
	// once per path, bumping lastValueRef once per function argument.
	trace.lastValueRef += static_cast<ValueRef>(trace.getArguments().size());

	// The suffix is a fresh path; the pruned-tag loop guard restarts with the
	// inherited prefix state.
	prunedTags.clear();
}

void ForkTraceContext::reportErrorAndExit(const std::string& message) {
	serialization::ByteWriter writer;
	writer.write<uint8_t>(static_cast<uint8_t>(MessageKind::Error));
	writer.writeString(message);
	try {
		serialization::writeMessage(resultFd_, writer.buffer);
	} catch (...) { // NOLINT(bugprone-empty-catch) - the root reports via timeout
	}
	_exit(1);
}

void ForkTraceContext::runWorker(std::function<void()>& wrapper) {
	try {
		auto rootAddress = (TagAddress) __builtin_return_address(0);
		tagRecorder_ = std::make_unique<TagRecorder>(rootAddress);
		state =
		    std::make_unique<TraceState>(*tagRecorder_, *currentTrace_, *symbolicExecutionContext_, *currentOptions_);
		resume();
		wrapper();
	} catch (const std::exception& exception) {
		reportErrorAndExit(exception.what());
	} catch (...) {
		reportErrorAndExit("forkTracing: traced function threw a non-standard exception");
	}
	try {
		finishPath();
	} catch (const std::exception& exception) {
		// Broken handoff chain; surface the root cause instead of letting the EOF
		// cascade of the orphaned continuations mask it.
		reportErrorAndExit(exception.what());
	} catch (...) {
		reportErrorAndExit("forkTracing: handoff failed with a non-standard exception");
	}
}

void ForkTraceContext::traceFunctionInWorkerTree(std::function<void()>& wrapper) {
	FdLimitGuard fdLimitGuard;

	int resultPair[2];
	if (socketpair(AF_UNIX, SOCK_STREAM, 0, resultPair) != 0) {
		throw RuntimeException("forkTracing: failed to create the result socket: " + std::string(std::strerror(errno)));
	}
	disableSigpipe(resultPair[0]);
	disableSigpipe(resultPair[1]);

	// Created before the fork so the worker tree inherits the objects at stable
	// addresses; the root reuses the tag recorder's trie to re-intern the result.
	symbolicExecutionContext_ = std::make_unique<SymbolicExecutionContext>();
	tagRecorder_ = std::make_unique<TagRecorder>((TagAddress) 0);

	auto workerPid = fork();
	if (workerPid < 0) {
		::close(resultPair[0]);
		::close(resultPair[1]);
		throw RuntimeException("forkTracing: fork failed: " + std::string(std::strerror(errno)));
	}
	if (workerPid == 0) {
		// Worker tree root: a dedicated process group so the tracing root can clean
		// up all descendants on error.
		setpgid(0, 0);
		::close(resultPair[0]);
		resultFd_ = resultPair[1];
		runWorker(wrapper); // never returns
		_exit(2);
	}
	setpgid(workerPid, workerPid); // racing the child's own call is fine
	::close(resultPair[1]);

	auto timeoutMs = currentOptions_->getOptionOrDefault<int>("engine.forkTraceTimeoutMs", 300000);
	std::vector<std::byte> payload;
	std::exception_ptr failure;
	try {
		payload = serialization::readMessageWithTimeout(resultPair[0], timeoutMs);
	} catch (...) {
		failure = std::current_exception();
	}
	::close(resultPair[0]);
	// Normally every process has exited; on errors this kills blocked continuations.
	kill(-workerPid, SIGKILL);
	// Reap the whole worker process group, not just the direct child: descendants
	// orphaned by their exiting parents reparent to init in most setups, but when the
	// embedding process is a subreaper or PID 1 (typical in containers) they reparent
	// to us and would accumulate as zombies in a long-lived engine. When we are not
	// their parent, waitpid(-pgid) stops with ECHILD right after the direct child.
	while (waitpid(-workerPid, nullptr, 0) > 0 || errno == EINTR) {
	}
	if (failure != nullptr) {
		std::rethrow_exception(failure);
	}
	if (payload.empty()) {
		throw RuntimeException("forkTracing: the worker tree terminated without producing a result");
	}

	serialization::ByteReader reader {payload};
	auto kind = static_cast<MessageKind>(reader.read<uint8_t>());
	if (kind == MessageKind::Error) {
		throw RuntimeException(reader.readString());
	}
	if (kind != MessageKind::FinalResult) {
		throw RuntimeException("forkTracing: unexpected result message kind");
	}
	// Adopt the final trace and the work-list discoveries in the root.
	state = std::make_unique<TraceState>(*tagRecorder_, *currentTrace_, *symbolicExecutionContext_, *currentOptions_);
	worklistAdditions_.clear();
	pendingContinuations_.clear();
	deserializeWorkerState(reader);
	for (const auto& addition : worklistAdditions_) {
		functionsToTrace.emplace_back(addition.name, *addition.wrapper, addition.attributes);
	}
	worklistAdditions_.clear();
	state.reset();
	tagRecorder_.reset();
	symbolicExecutionContext_.reset();
}

std::unique_ptr<ExecutionTrace> ForkTraceContext::trace(std::function<void()>& traceFunction,
                                                        const engine::Options& options, Arena& arena) {
	log::debug("Initialize Fork Tracing");
	auto& tc = forkTraceContext;
	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	setActiveTracer(&tc);
	ActiveTracerGuard activeTracerGuard;

	tc.functionsToTrace.clear();
	tc.registeredFunctions.clear();
	tc.currentOptions_ = &options;
	tc.currentTrace_ = executionTrace.get();
	tc.traceFunctionInWorkerTree(traceFunction);

	log::debug("Fork Tracing Terminated");
	return executionTrace;
}

std::unique_ptr<TraceModule> ForkTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                     const engine::Options& options, Arena& arena) {
	return forkTraceContext.startTrace(functions, options, arena);
}

std::unique_ptr<TraceModule> ForkTraceContext::startTrace(std::list<compiler::CompilableFunction>& functions,
                                                          const engine::Options& options, Arena& arena) {
	log::debug("Initialize Fork Tracing");
	auto traceModule = std::make_unique<TraceModule>();
	functionsToTrace = functions;
	registeredFunctions.clear();
	setActiveTracer(this);
	ActiveTracerGuard activeTracerGuard;
	currentOptions_ = &options;

	bool isFirstFunction = true;
	while (!functionsToTrace.empty()) {
		auto currentFunction = functionsToTrace.front();
		functionsToTrace.pop_front();
		if (traceModule->hasFunction(currentFunction.getName())) {
			log::debug("Function '{}' already traced, skipping.", currentFunction.getName());
			continue;
		}

		auto& executionTrace = traceModule->addNewFunction(currentFunction.getName(), arena);
		auto attributes = currentFunction.getAttributes();
		if (isFirstFunction) {
			attributes["entry"] = "true";
			isFirstFunction = false;
		}
		traceModule->setFunctionAttributes(currentFunction.getName(), attributes);
		auto wrapperFunc = currentFunction.getFunction();

		currentTrace_ = &executionTrace;
		traceFunctionInWorkerTree(wrapperFunc);

		log::debug("Fork Tracing of '{}' terminated", currentFunction.getName());
	}

	return traceModule;
}

void ForkTraceContext::allocateValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.increment(ref);
}

void ForkTraceContext::freeValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.decrement(ref);
}

void ForkTraceContext::pushStaticVal(void* valPtr, size_t size) {
	staticVars.emplace_back(valPtr, size);
}

void ForkTraceContext::popStaticVal() {
	staticVars.pop_back();
}

Snapshot ForkTraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
