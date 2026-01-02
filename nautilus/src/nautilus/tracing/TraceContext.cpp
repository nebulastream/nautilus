
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <cassert>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <ranges>
#include <sstream>
#include <sys/wait.h>

// #define TRACER_LOG(msg, ...) fmt::println(stderr, "[TRACER {}] " msg, getpid() __VA_OPT__(, ) __VA_ARGS__)
#define TRACER_LOG(msg, ...)

enum class ForkRole { CHILD, PARENT };
static thread_local std::vector<std::pair<pid_t, int>> pendingForks;
static thread_local size_t currentForkDepth = 0;
static thread_local bool invertNextFork = false;

[[noreturn]] void exitChild() {
	for (auto [pid, pipe] : pendingForks) {
		write(pipe, "A", 1);
		int status;
		waitpid(pid, &status, 0);
		assert(status == 0);
	}

	TRACER_LOG("EXIT");
	// Shared memory is automatically visible to parent - no need to serialize
	_exit(0);
}

ForkRole forkExecution(nautilus::tracing::Trace& trace) {
	int sync_pipe[2];
	pipe(sync_pipe);

	int pid = fork();
	if (pid == 0) {
		assert(trace.globalTagMap.header->magic == 0xDEADBEEF);
		close(sync_pipe[1]);
		char buf;
		read(sync_pipe[0], &buf, 1);
		pendingForks.clear();

		// Increment fork depth in child process
		currentForkDepth++;

		// Update max fork depth in shared trace header (atomic update to handle races)
		if (currentForkDepth > trace.view->maxForkDepth) {
			trace.view->maxForkDepth = currentForkDepth;
		}

		TRACER_LOG("Child at depth {}", currentForkDepth);
		return ForkRole::CHILD;
	} else {
		TRACER_LOG("WAITING");
		close(sync_pipe[0]);
		pendingForks.emplace_back(pid, sync_pipe[1]);
		return ForkRole::PARENT;
	}
}

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

// Thread-local TraceContext object (not a pointer)
// This is allocated in thread-local storage - zero heap allocation overhead
static thread_local TraceContext traceContext;

TraceState::TraceState(TagRecorder& tr, Trace& tt, const engine::Options& opts)
    : tagRecorder(tr), executionTrace(tt), options(opts) {
	// TraceState only holds references - the actual objects are stack-allocated in trace()
}

TraceContext* TraceContext::get() {
	return traceContext.state ? &traceContext : nullptr;
}

TraceContext* TraceContext::initialize(TagRecorder& tagRecorder, Trace& tracerTrace, const engine::Options& options) {
	traceContext.state = std::make_unique<TraceState>(tagRecorder, tracerTrace, options);
	return &traceContext;
}

void TraceContext::resume() {
	// Clear dynamic containers
	staticVars.clear();

	// Reset aliveVars to initial state (all counts to 0, hash to 0)
	aliveVars.reset();

	// Note: state (with executionTrace and symbolicExecutionContext) is NOT reset here
	// as it needs to persist across trace iterations
}

TypedValueRef TraceContext::registerFunctionArgument(Type type, size_t) {
	return state->executionTrace.addArgument(type);
}

void TraceContext::traceValueDestruction(nautilus::tracing::TypedValueRef) {
	// currently yed not implemented
}

TypedValueRef TraceContext::traceConstValue(Type type, const ConstantLiteral& constValue) {
	log::debug("Trace Constant");
	auto tag = recordSnapshot();
	if (auto ref = state->executionTrace.globalTagMap.get(tag)) {
		auto originalOp = state->executionTrace.getOperation(*ref);
		auto [resultRef, _] = state->executionTrace.addConstOperation(tag, type, constValue);
		state->executionTrace.addAssignmentOperation(tag, originalOp.getResultRef(), resultRef);
		return originalOp.getResultRef();
	} else {
		auto [result, oi] = state->executionTrace.addConstOperation(tag, type, constValue);
		state->executionTrace.globalTagMap.insert(tag, oi);
		return result;
	}
}

template <typename OnCreation>
TypedValueRef TraceContext::traceOperation([[maybe_unused]] Op op, OnCreation&& onCreation) {
	auto tag = recordSnapshot();
	if (state->executionTrace.checkTag(tag)) {
		auto [result, oi] = onCreation(tag);
		state->executionTrace.globalTagMap.insert(tag, oi);
		return result;
	} else {
		exitChild();
	}
}

TypedValueRef TraceContext::traceCopy(const TypedValueRef& ref) {
	log::debug("Trace Copy");
	return traceOperation(ASSIGN,
	                      [&](Snapshot& tag) { return state->executionTrace.addAssignmentOperation(tag, ref); });
}

TypedValueRef TraceContext::traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                      const FunctionAttributes fnAttrs) {
	return traceOperation(CALL, [&](Snapshot& tag) {
		return state->executionTrace.addCallOperation(tag, resultType, fptn, fnAttrs,
		                                              std::span(arguments.begin(), arguments.end()));
	});
}

void TraceContext::traceAssignment(const TypedValueRef& targetRef, const TypedValueRef& sourceRef,
                                   [[maybe_unused]] Type resultType) {
	assert(targetRef.type == resultType && sourceRef.type == resultType && "I hope this is true");
	traceOperation(
	    ASSIGN, [&](Snapshot& tag) { return state->executionTrace.addAssignmentOperation(tag, targetRef, sourceRef); });
}

void TraceContext::traceReturnOperation([[maybe_unused]] Type resultType, const TypedValueRef& ref) {
	assert(resultType == ref.type && "I hope this is true");
	TRACER_LOG("RETURN");
	auto tag = recordSnapshot();
	state->executionTrace.addReturn(tag, ref);
	exitChild();
}

TypedValueRef TraceContext::traceOperation(Op op, Type resultType, std::vector<TypedValueRef> inputs) {
	return traceOperation(op, [&](Snapshot& tag) {
		return state->executionTrace.addOperationWithResult(tag, resultType, op,
		                                                    std::span(inputs.begin(), inputs.end()));
	});
}

bool TraceContext::traceCmp(const TypedValueRef& targetRef, const double probability) {

	log::debug("Trace CMP");
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		exitChild();
	}
	auto [_, oi] = state->executionTrace.appendCmp(tag, targetRef, probability);
	TRACER_LOG("CMP at position: Block: {} Operation: {}", oi.blockIndex.offset, oi.operationOffset.offset);
	state->executionTrace.globalTagMap.insert(tag, oi);
	state->executionTrace.view->forkCount++;

	bool exploreBranchFirst = !invertNextFork;
	invertNextFork = false;

	if (forkExecution(state->executionTrace) == ForkRole::PARENT) {
		if (exploreBranchFirst) {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().trueBlock =
			    state->executionTrace.createNewBlock().getBlockId();
			state->executionTrace.getOperation(oi).arguments<CmpInput>().falseBlock =
			    std::numeric_limits<std::uint16_t>::max();
		} else {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().falseBlock =
			    state->executionTrace.createNewBlock().getBlockId();
			state->executionTrace.getOperation(oi).arguments<CmpInput>().trueBlock =
			    std::numeric_limits<std::uint16_t>::max();
		}
		return exploreBranchFirst;
	} else {
		auto oi = *state->executionTrace.globalTagMap.get(tag);
		if (exploreBranchFirst) {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().falseBlock =
			    state->executionTrace.createNewBlock().getBlockId();
		} else {
			state->executionTrace.getOperation(oi).arguments<CmpInput>().trueBlock =
			    state->executionTrace.createNewBlock().getBlockId();
		}
		return !exploreBranchFirst;
	}
}

std::unique_ptr<ExecutionTrace> TraceContext::trace(std::function<void()>& traceFunction,
                                                    const engine::Options& options) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// Allocate Trace (shared memory, no heap allocs) and SymbolicExecutionContext on the stack
	Trace tracerTrace;

	// Initialize TraceContext with references to our stack objects
	initialize(tr, tracerTrace, options);

	// Reset fork depth for new trace
	currentForkDepth = 0;

	auto pid = fork();
	if (pid == 0) {
		assert(tracerTrace.globalTagMap.header->magic == 0xDEADBEEF);
		tracerTrace.createNewBlock();
		try {
			traceFunction();
		} catch (...) {
			fmt::println(stderr, "Trace Function threw an exception!. Terminating Trace");
			std::cerr << std::flush;
			_exit(1);
		}
		TRACER_LOG("Actually I am not sure how we got here!");
	} else {
		int status;
		waitpid(pid, &status, 0);
		if (status != 0) {
			throw RuntimeException("Tracing failed");
		}
		TRACER_LOG("Tracing done");
	}
	traceContext.state.reset();

	// Convert Trace to ExecutionTrace for SSA phase and return
	auto executionTrace = toExecutionTrace(tracerTrace);
	log::trace("Final trace: {}", executionTrace);
	log::info("{} Forks, Max Depth: {}", tracerTrace.view->forkCount, tracerTrace.view->maxForkDepth);

	return std::make_unique<ExecutionTrace>(std::move(executionTrace));
}

std::vector<StaticVarHolder>& TraceContext::getStaticVars() {
	return staticVars;
}

void TraceContext::allocateValRef(ValueRef ref) {
	aliveVars.increment(ref);
}
void TraceContext::freeValRef(ValueRef ref) {
	aliveVars.decrement(ref);
}
void TraceContext::suggestInvertedBranch() {
	invertNextFork = true;
}

constexpr size_t fnv_prime = 0x100000001b3;
constexpr size_t offset_basis = 0xcbf29ce484222325;

uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data) {
	size_t hash = offset_basis;
	for (auto& value : data) {
		hash ^= *value.ptr;
		hash *= fnv_prime;
	}
	return hash;
}

Snapshot TraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
