
#include "ExceptionBasedTraceContext.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include "symbolic_execution/TraceTerminationException.hpp"
#include <cassert>
#include <cstddef>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/format.h>
#include <sstream>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

// Thread-local ExceptionBasedTraceContext object (not a pointer)
// This is allocated in thread-local storage - zero heap allocation overhead
static thread_local ExceptionBasedTraceContext traceContext;

TraceState::TraceState(TagRecorder& tr, ExecutionTrace& et, SymbolicExecutionContext& sec, const engine::Options& opts)
    : tagRecorder(tr), executionTrace(et), symbolicExecutionContext(sec), options(opts) {
	// TraceState only holds references - the actual objects are stack-allocated in trace()
}

bool ExceptionBasedTraceContext::isActive() const {
	return state != nullptr;
}

ExceptionBasedTraceContext* ExceptionBasedTraceContext::initialize(TagRecorder& tagRecorder,
                                                                   ExecutionTrace& executionTrace,
                                                                   SymbolicExecutionContext& symbolicExecutionContext,
                                                                   const engine::Options& options) {
	traceContext.state = std::make_unique<TraceState>(tagRecorder, executionTrace, symbolicExecutionContext, options);
	setActiveTracer(&traceContext);
	return &traceContext;
}

void ExceptionBasedTraceContext::resume() {
	// Clear dynamic containers
	staticVars.clear();

	// Reset aliveVars to initial state (all counts to 0, hash to 0)
	aliveVars.reset();

	// Note: state (with executionTrace and symbolicExecutionContext) is NOT reset here
	// as it needs to persist across trace iterations
}

TypedValueRef& ExceptionBasedTraceContext::registerFunctionArgument(Type type, size_t index) {
	return state->executionTrace.setArgument(type, index);
}

bool ExceptionBasedTraceContext::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& ExceptionBasedTraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	auto consumedTag = currentOperation.tag;
	state->executionTrace.nextOperation();
	assert(currentOperation.op == op);
	// traceConstant/traceCopy's globalTagMap-collision branch (see their
	// definitions below) records a *reconciliation* ASSIGN immediately after
	// the primary operation, sharing its exact tag, so that a stale value ref
	// from an earlier, structurally-identical call site is patched to the
	// freshly recorded value -- folding two recorded operations into a single
	// traceConstant()/traceCopy() call. A later FOLLOW-mode replay only ever
	// issues one follow() call per call site, so without this the cursor
	// would desynchronize from the recorded operation stream by one entry as
	// soon as it stepped over such a pair, corrupting every subsequent follow()
	// in the block (see LazyTraceContext::follow, issue #384, for the fuller
	// writeup -- this is the same defect in this tracer's own copy of the
	// mechanism). Skip any run of same-tagged reconciliation operations here
	// to keep the cursor aligned.
	while (true) {
		auto& block = state->executionTrace.getCurrentBlock();
		if (state->executionTrace.currentOperationIndex >= block.operations.size()) {
			break;
		}
		if (!(block.operations[state->executionTrace.currentOperationIndex]->tag == consumedTag)) {
			break;
		}
		state->executionTrace.nextOperation();
	}
	return currentOperation.resultRef;
}

TypedValueRef& ExceptionBasedTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	log::debug("Trace Constant");
	auto op = Op::CONST;
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto* originalRef = state->executionTrace.getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		state->executionTrace.addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		return originalRef->resultRef;
	} else {
		return state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
	}
}

template <typename OnCreation>
TypedValueRef& ExceptionBasedTraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (isFollowing()) {
		return follow(op);
	} else {
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			return onCreation(tag);
		} else {
			// TODO find a way to handle this more graceful.
			throw TraceTerminationException();
		}
	}
}

template <typename OnCreation>
TypedValueRef& ExceptionBasedTraceContext::traceCallOperation(Op op, void* calleePtr, OnCreation&& onCreation) {
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	auto& trace = state->executionTrace;

	// Tag identity is purely a call-stack return-address chain (TagRecorder.cpp),
	// so a tag match here does not by itself prove this call is a genuine
	// control-flow re-entry (a loop back-edge or an if/else merge point): the
	// identical call-stack shape can also be reached while invoking a
	// *different* callee through a single shared call-dispatch site -- e.g. an
	// interpreter that resolves its target function pointer at runtime and
	// issues every call through one nautilus::invoke() call site, exactly how
	// a query compiler uses Nautilus (issue #381). Distinguish the two by
	// checking whether the operation already recorded at this tag targets the
	// same callee: if not, this is an unrelated call that merely collided, so
	// record it fresh instead of forcing a bogus merge/abort. Mirrors
	// traceAssignment's #382 fix and traceCopy's #95 fix for the same
	// underlying Tag-collision defect. Same defect and same fix as
	// LazyTraceContext::traceCallOperation -- this tracer keeps its own copy of
	// the mechanism.
	const operation_identifier* existing = nullptr;
	if (auto it = trace.globalTagMap.find(tag); it != trace.globalTagMap.end()) {
		existing = &it->second;
	} else if (auto localIt = trace.localTagMap.find(tag); localIt != trace.localTagMap.end()) {
		existing = &localIt->second;
	}
	if (existing != nullptr) {
		auto* existingOp = trace.getBlocks()[existing->blockIndex]->operations[existing->operationIndex];
		bool sameCallee = existingOp->op == op && std::get<FunctionCall*>(existingOp->input[0])->ptr == calleePtr;
		if (!sameCallee) {
			return onCreation(tag);
		}
	}
	if (!trace.checkTag(tag)) {
		throw TraceTerminationException();
	}
	return onCreation(tag);
}

TypedValueRef& ExceptionBasedTraceContext::traceAlloca(size_t size, size_t align) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return traceOperation(op, [&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto index = state->executionTrace.addAllocaSpec(size, align);
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	});
}

TypedValueRef& ExceptionBasedTraceContext::traceCopy(const TypedValueRef& ref) {
	log::debug("Trace Copy");
	if (isFollowing()) {
		return follow(ASSIGN);
	}
	auto tag = recordSnapshot();
	auto& trace = state->executionTrace;
	auto globalTabIter = trace.globalTagMap.find(tag);
	if (globalTabIter != trace.globalTagMap.end()) {
		// This copy's call site was already reached by a different execution
		// path (e.g. the other arm of a native `cond ? a : b` ternary
		// copy-constructing from a different pre-existing value). The source
		// ref legitimately differs per branch, so reaching the same tag here
		// is not a genuine control-flow reconvergence and must not trigger a
		// premature merge (which would silently discard the diverging copy on
		// this path -- the root cause of the "double jump" miscompilation in
		// issue #95). Record a fresh copy of the current source value, and
		// reconcile it onto the canonical result ref established by the first
		// visit, mirroring traceConstant's handling of the same situation for
		// freshly-constructed literals.
		auto& originalIdentifier = globalTabIter->second;
		auto* originalOp =
		    trace.getBlocks()[originalIdentifier.blockIndex]->operations[originalIdentifier.operationIndex];
		auto resultRef = trace.getNextValueRef();
		trace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
		trace.addAssignmentOperation(tag, originalOp->resultRef, {resultRef, ref.type}, ref.type);
		return originalOp->resultRef;
	}
	if (!trace.checkTag(tag)) {
		// A local-tag collision (same-pass revisit): defer to the existing
		// control-flow-merge machinery, unchanged from before this fix.
		throw TraceTerminationException();
	}
	auto resultRef = trace.getNextValueRef();
	return trace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
}

TypedValueRef& ExceptionBasedTraceContext::traceCall(void* fptn, Type resultType,
                                                     const std::vector<tracing::TypedValueRef>& arguments,
                                                     FunctionAttributes fnAttrs) {
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto op = Op::CALL;
	return traceCallOperation(op, fptn, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = mangledName,
		                                                                        .ptr = fptn,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& ExceptionBasedTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                             const std::vector<tracing::TypedValueRef>& arguments,
                                                             FunctionAttributes fnAttrs) {
	auto op = Op::INDIRECT_CALL;
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		auto* indirectCall = state->executionTrace.getArena().create<IndirectFunctionCall>(
		    IndirectFunctionCall {.fnPtr = fnPtrRef, .arguments = arguments, .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {indirectCall});
	});
}

TypedValueRef& ExceptionBasedTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                             std::function<void()> fwrapper, Type resultType,
                                                             const std::vector<tracing::TypedValueRef>& arguments,
                                                             FunctionAttributes fnAttrs) {
	auto functionName = definition->name();
	auto mangledName = getMangledName((void*) definition);
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(compiler::CompilableFunction(functionName, fwrapper, definition->attributes()));
		log::debug("Added function '{}' to functionsToTrace list. List now has {} functions", functionName,
		           functionsToTrace.size());
	}
	auto op = Op::CALL;
	return traceCallOperation(op, (void*) definition, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = arguments,
		                                                                        .fnAttrs = fnAttrs});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

TypedValueRef& ExceptionBasedTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                                    std::function<void()> fwrapper) {
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(
		    compiler::CompilableFunction(functionName, std::move(fwrapper), definition->attributes()));
		log::debug("Added function '{}' to functionsToTrace list (via FUNC_ADDR). List now has {} functions",
		           functionName, functionsToTrace.size());
	}
	auto op = Op::FUNC_ADDR;
	auto resultType = Type::ptr;
	return traceCallOperation(op, (void*) definition, [&](Snapshot& tag) -> TypedValueRef& {
		auto* functionArguments =
		    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
		                                                                        .mangledName = functionName,
		                                                                        .ptr = (void*) definition,
		                                                                        .arguments = {},
		                                                                        .fnAttrs = {}});
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	});
}

void ExceptionBasedTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source,
                                                 Type resultType) {
	if (isFollowing()) {
		follow(ASSIGN);
		return;
	}
	auto tag = recordSnapshot();
	auto& trace = state->executionTrace;

	// Tag identity is purely a call-stack return-address chain (TagRecorder.cpp),
	// so a tag match here does not by itself prove this ASSIGN is a genuine
	// control-flow re-entry (a loop back-edge or an if/else merge point): the
	// identical call-stack shape, with an identical alive-variable footprint,
	// can also be reached while assigning a *different* target -- e.g. two
	// structurally-identical sibling arguments of the same call (`f(0, x, 0)`)
	// evaluated by the same argument-evaluation loop, or two slots of the same
	// array assigned via the same loop body. Distinguish the two by checking
	// whether the operation already recorded at this tag assigned the same
	// target: if not, this is an unrelated assignment that merely collided,
	// so record it fresh instead of forcing a bogus merge that would discard
	// everything traced afterwards (issue #382). This mirrors how traceCopy
	// already reconciles instead of merging when a repeated call site's
	// *source* legitimately differs (issue #95/#384). Same defect and same
	// fix as LazyTraceContext::traceAssignment -- this tracer keeps its own
	// copy of the mechanism.
	const operation_identifier* existing = nullptr;
	if (auto it = trace.globalTagMap.find(tag); it != trace.globalTagMap.end()) {
		existing = &it->second;
	} else if (auto localIt = trace.localTagMap.find(tag); localIt != trace.localTagMap.end()) {
		existing = &localIt->second;
	}
	if (existing != nullptr) {
		auto* existingOp = trace.getBlocks()[existing->blockIndex]->operations[existing->operationIndex];
		if (existingOp->op != ASSIGN || existingOp->resultRef.ref != target.ref) {
			trace.addAssignmentOperation(tag, target, source, resultType);
			return;
		}
	}
	if (!trace.checkTag(tag)) {
		throw TraceTerminationException();
	}
	trace.addAssignmentOperation(tag, target, source, resultType);
}

void ExceptionBasedTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		state->executionTrace.addReturn(tag, resultType, ref);
	}
}

TypedValueRef& ExceptionBasedTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                         const TypedValueRef& right) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	});
}

TypedValueRef& ExceptionBasedTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	});
}

TypedValueRef& ExceptionBasedTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                          const TypedValueRef& second, const TypedValueRef& third) {
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

std::string ExceptionBasedTraceContext::formatStaticVars() const {
	std::string result;
	for (size_t i = 0; i < staticVars.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		result += std::to_string(getStaticVarValue(staticVars[i]));
	}
	return result;
}

void ExceptionBasedTraceContext::pushStaticVal(void* valPtr, size_t size) {
	staticVars.emplace_back(valPtr, size);
	if (log::options::getLogStaticVars()) {
		log::info("pushStaticVal: [{}]", formatStaticVars());
	}
}

void ExceptionBasedTraceContext::popStaticVal() {
	if (log::options::getLogStaticVars()) {
		log::info("popStaticVal: [{}] (popping last)", formatStaticVars());
	}
	staticVars.pop_back();
}

bool ExceptionBasedTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	bool result;
	if (state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		// eval execution path one step
		// we repeat the operation
		result = state->symbolicExecutionContext.follow();
	} else {
		// record
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			state->executionTrace.addCmpOperation(tag, value, probability);
			result = state->symbolicExecutionContext.record(tag);
		} else {
			// this is actually the same tag -> throw up
			throw TraceTerminationException();
		}
	}

	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint16_t nextBlock;
	if (result) {
		nextBlock = std::get<BlockRef*>(currentOperation.input[1])->block;
	} else {
		nextBlock = std::get<BlockRef*>(currentOperation.input[2])->block;
	}
	state->executionTrace.setCurrentBlock(nextBlock);
	return result;
}

std::unique_ptr<ExecutionTrace> ExceptionBasedTraceContext::trace(std::function<void()>& traceFunction,
                                                                  const engine::Options& options, Arena& arena) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// The ExecutionTrace borrows the caller-provided arena for all Block
	// and TraceOperation allocations.  The arena must outlive the returned
	// trace.
	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	SymbolicExecutionContext symbolicExecutionContext;

	// Initialize ExceptionBasedTraceContext with references to our objects
	auto tc = initialize(tr, *executionTrace, symbolicExecutionContext, options);
	// Ensure the thread-local active tracer is cleared even if an exception
	// other than TraceTerminationException escapes the loop below.
	ActiveTracerGuard activeTracerGuard;
	auto traceIteration = 0;

	// Symbolic execution loop: explore all execution paths
	while (symbolicExecutionContext.shouldContinue()) {
		try {
			traceIteration = traceIteration + 1;
			log::trace("Trace Iteration {}", traceIteration);
			log::trace("{}", *executionTrace);

			// Prepare for next iteration
			symbolicExecutionContext.next();
			executionTrace->resetExecution();
			tc->resume(); // Reset persistent state (staticVars, aliveVars)

			// Execute the traced function
			traceFunction();
		} catch (const TraceTerminationException& ex) {
			// Normal termination when we hit a known control flow merge or loop
		}
		// After each iteration, the static variable stack must be empty.
		// All static_val destructors should have fired via normal return or stack unwinding.
		assert(traceContext.staticVars.empty() && "static variable stack not empty after tracing iteration");
	}

	// Clean up: reset state pointer. activeTracer is cleared by ActiveTracerGuard.
	tc->state.reset();

	log::debug("Tracing Terminated with {} iterations", traceIteration);
	log::trace("Final trace: {}", *executionTrace);

	return executionTrace;
}

std::unique_ptr<TraceModule> ExceptionBasedTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                               const engine::Options& options, Arena& arena) {
	return traceContext.startTrace(functions, options, arena);
}

std::unique_ptr<TraceModule> ExceptionBasedTraceContext::startTrace(std::list<compiler::CompilableFunction>& functions,
                                                                    const engine::Options& options, Arena& arena) {
	log::debug("Initialize Tracing");
	auto traceModule = std::make_unique<TraceModule>();
	functionsToTrace = functions;
	registeredFunctions.clear();
	setActiveTracer(this);
	// Ensure the thread-local active tracer is cleared even if an exception
	// other than TraceTerminationException escapes the per-function loop below.
	ActiveTracerGuard activeTracerGuard;

	bool isFirstFunction = true;
	while (!functionsToTrace.empty()) {
		auto currentFunction = functionsToTrace.front();
		functionsToTrace.pop_front();
		if (traceModule->hasFunction(currentFunction.getName())) {
			log::debug("Function '{}' already traced, skipping.", currentFunction.getName());
			continue;
		}

		auto& executionTrace = traceModule->addNewFunction(currentFunction.getName(), arena);
		// Tag the first popped function as the entry point. Everything else in
		// the queue was `invoke`d by some other traced function; that alone
		// cannot tell us host-vs-device side (a plain NautilusFunction may be
		// invoked from either), so non-entry classification is left to a
		// downstream call-graph pass. `kernel` rides through unchanged from
		// NautilusKernelFunction.
		auto attributes = currentFunction.getAttributes();
		if (isFirstFunction) {
			attributes["entry"] = "true";
			isFirstFunction = false;
		}
		traceModule->setFunctionAttributes(currentFunction.getName(), attributes);
		auto wrapperFunc = currentFunction.getFunction();

		auto rootAddress = __builtin_return_address(0);
		auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);
		SymbolicExecutionContext symbolicExecutionContext;
		state = std::make_unique<TraceState>(tr, executionTrace, symbolicExecutionContext, options);
		auto traceIteration = 0;

		while (symbolicExecutionContext.shouldContinue()) {
			try {
				traceIteration = traceIteration + 1;
				log::trace("Trace Iteration {}", traceIteration);
				log::trace("{}", executionTrace);
				symbolicExecutionContext.next();
				executionTrace.resetExecution();
				resume();
				wrapperFunc();
			} catch (const TraceTerminationException& ex) {
			}
			assert(staticVars.empty() && "static variable stack not empty after tracing iteration");
		}

		state.reset();
		log::debug("Tracing Terminated with {} iterations", traceIteration);
		log::trace("Final trace: {}", executionTrace);
	}

	// activeTracer is cleared by ActiveTracerGuard.
	return traceModule;
}

void ExceptionBasedTraceContext::allocateValRef(ValueRef ref) {
	aliveVars.increment(ref);
}
void ExceptionBasedTraceContext::freeValRef(ValueRef ref) {
	aliveVars.decrement(ref);
}

std::string TraceContextBase::getMangledName(void* fnptr) {
	if (const auto it = mangledNameCache.find(fnptr); it != mangledNameCache.end()) {
		return it->second;
	}

	Dl_info info;
	if (dladdr(fnptr, &info) != 0 && info.dli_sname != nullptr) {
		mangledNameCache[fnptr] = info.dli_sname;
		return info.dli_sname;
	}
	std::stringstream ss;
	ss << fnptr;
	std::string ptrStr = ss.str();
	mangledNameCache[fnptr] = ptrStr;
	return ptrStr;
}

std::string TraceContextBase::getFunctionName(void* fnptr, const std::string& mangledName) {
	bool normalizeFunctionNames = state->options.getOptionOrDefault("engine.normalizeFunctionNames", false);

	if (normalizeFunctionNames) {
		auto it = state->normalizedFunctionNameCache.find(fnptr);
		if (it != state->normalizedFunctionNameCache.end()) {
			return "runtimeFunc" + std::to_string(it->second);
		}
		uint32_t index = state->nextNormalizedFunctionIndex++;
		state->normalizedFunctionNameCache[fnptr] = index;
		return "runtimeFunc" + std::to_string(index);
	}

	bool demangleFunctionNames = state->options.getOptionOrDefault("engine.demangleFunctionNames", true);

	if (!demangleFunctionNames) {
		return mangledName;
	}

	int status;
	char* demangled = __cxxabiv1::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
	if (status == 0 && demangled != nullptr) {
		std::string result(demangled);
		std::free(demangled);
		return result;
	}

	return mangledName;
}

constexpr size_t fnv_prime = 0x100000001b3;
constexpr size_t offset_basis = 0xcbf29ce484222325;

uint64_t hashStaticVector(const std::vector<StaticVarHolder>& data) {
	size_t hash = offset_basis;
	for (auto& entry : data) {
		uint64_t val = 0;
		std::memcpy(&val, entry.ptr, entry.size);
		hash ^= val;
		hash *= fnv_prime;
	}
	return hash;
}

Snapshot ExceptionBasedTraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
