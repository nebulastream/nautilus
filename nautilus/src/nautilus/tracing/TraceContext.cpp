
#include "TraceContext.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/fork_tracing/ForkTraceContext.hpp"
#include "nautilus/tracing/symbolic_tracing/SymbolicExecutionTraceContext.hpp"
#include <cassert>
#include <cpptrace/cpptrace.hpp>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <llvm-19/llvm/ADT/APFloat.h>
#include <ranges>
#include <sstream>
#include <sys/wait.h>

// #define TRACER_LOG(msg, ...) fmt::println(stderr, "[TRACER {}] " msg, getpid() __VA_OPT__(, ) __VA_ARGS__)
#define TRACER_LOG(msg, ...)

namespace nautilus::tracing {

TraceState::TraceState(TagRecorder& tr, Trace& tt, const engine::Options& opts)
    : tagRecorder(tr), executionTrace(tt), options(opts) {
	// TraceState only holds references - the actual objects are stack-allocated in trace()
}

thread_local ForkTraceContext forkTraceContext;
thread_local SymbolicExecutionTraceContext seTraceContext;
thread_local TraceContext* activeTraceContext = &forkTraceContext;

std::unique_ptr<ExecutionTrace> trace(std::function<void()>& traceFunction, const engine::Options& options) {
	// TODO potentially add decision rule for choosing tracer if not force set
	auto tracer = options.getOptionOrDefault("engine.tracer", std::string {"symbolic"});
	if (tracer == "fork") {
		activeTraceContext = &forkTraceContext;
	} else if (tracer == "symbolic") {
		activeTraceContext = &seTraceContext;
	} else {
		llvm::errs() << "Unknown tracer option: " << tracer;
		assert(false);
	}
	return activeTraceContext->trace(traceFunction, options);
}

TraceContext* TraceContext::get() {
	return activeTraceContext->state ? activeTraceContext : nullptr;
}

TraceContext* TraceContext::initialize(TagRecorder& tagRecorder, Trace& tracerTrace, const engine::Options& options) {
	activeTraceContext->state = std::make_unique<TraceState>(tagRecorder, tracerTrace, options);
	return activeTraceContext;
}

void TraceContext::resume() {
	// Clear dynamic containers
	staticVars.clear();

	// Reset aliveVars to initial state (all counts to 0, hash to 0)
	aliveVars.reset();

	// Note: state (with executionTrace and symbolicExecutionContext) is NOT reset here
	// as it needs to persist across trace iterations
}
void TraceContext::resetState() {
	state.reset();
}

void TraceContext::suggestInvertedBranch() {
	// not implemented by default;
}

void TraceContext::traceValueDestruction(nautilus::tracing::TypedValueRef) {
	// currently yet not implemented
	assert(false);
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

TypedValueRef TraceContext::traceCopy(const TypedValueRef& ref) {
	log::debug("Trace Copy");
	return traceOperation(ASSIGN,
	                      [&](Snapshot& tag) { return state->executionTrace.addAssignmentOperation(tag, ref); });
}

void TraceContext::traceAssignment(const TypedValueRef& targetRef, const TypedValueRef& sourceRef, Type resultType) {
	assert(targetRef.type == resultType && sourceRef.type == resultType && "I hope this is true");
	traceOperation(
	    ASSIGN, [&](Snapshot& tag) { return state->executionTrace.addAssignmentOperation(tag, targetRef, sourceRef); });
}

TypedValueRef TraceContext::traceOperation(Op op, Type resultType, std::vector<TypedValueRef> inputs) {
	return traceOperation(op, [&](Snapshot& tag) {
		return state->executionTrace.addOperationWithResult(tag, resultType, op,
		                                                    std::span(inputs.begin(), inputs.end()));
	});
}

TypedValueRef TraceContext::traceCall(void* fptn, Type resultType, const std::vector<tracing::TypedValueRef>& arguments,
                                      const FunctionAttributes fnAttrs) {
	return traceOperation(CALL, [&](Snapshot& tag) {
		return state->executionTrace.addCallOperation(tag, resultType, fptn, fnAttrs,
		                                              std::span(arguments.begin(), arguments.end()));
	});
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
