
#include "ScopedTraceContext.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <fmt/format.h>

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};
} // namespace fmt

namespace nautilus::tracing {

namespace {

// Tracer-local cache mapping a ValueRef produced by a comparison op (EQ, NEQ,
// LT, LTE, GT, GTE) to a literal integer it was compared against.  Used by
// derivePredicate() to avoid walking the trace each time traceBool() fires.
// Cleared at the top of every trace iteration via resume().
struct ComparisonInfo {
	Op cmpOp;
	ValueRef var;
	int64_t lit;
	bool valid;
};
thread_local std::unordered_map<ValueRef, ComparisonInfo> comparisonCache;

// Tracer-local cache mapping a ValueRef produced by Op::CONST to its integer
// value.  Populated in traceConstant(), propagated through traceCopy() so the
// taraceBinaryOp() short-circuit can detect `const == const` CMPs and produce
// a constant-bool predicate even when the user code didn't go through a CMP
// op (e.g. `r = 1; if (r == 42)` after an inlined return).  Cleared at the
// top of every trace iteration via resume().
thread_local std::unordered_map<ValueRef, int64_t> integerConstants;

// Maps a ValueRef whose statically-resolvable bool value is known (because
// it came from a comparison of two integer constants) to that bool value.
// traceBool() consults this cache before the PathPredicateStore so we can
// prune CMPs that don't have a variable side at all.
thread_local std::unordered_map<ValueRef, bool> staticBoolValues;

// Pay-as-you-go gates.  Each cache lookup in the hot path (traceBinaryOp,
// traceBool, traceCopy, traceUnaryOp) is guarded by a boolean flag that flips
// the first time the corresponding cache receives a useful entry.  For
// workloads with many CMPs against purely symbolic operands (e.g.
// `nestedIf100`, `chainedIf100`), the maps stay empty and the flags stay
// false — every op pays one branch on a bool instead of an unordered_map
// lookup.  Reset in resume() alongside the caches.
thread_local bool hasIntegerConstants = false;
thread_local bool hasComparisonCache = false;
thread_local bool hasStaticBoolValues = false;

// Evaluates `lhs cmpOp rhs` for integer literals.  Mirrors the signed
// semantics that Nautilus uses for Op::EQ / NEQ / LT / LTE / GT / GTE.
bool evaluateConstCmp(Op cmpOp, int64_t lhs, int64_t rhs) noexcept {
	switch (cmpOp) {
	case EQ:
		return lhs == rhs;
	case NEQ:
		return lhs != rhs;
	case LT:
		return lhs < rhs;
	case LTE:
		return lhs <= rhs;
	case GT:
		return lhs > rhs;
	case GTE:
		return lhs >= rhs;
	default:
		return false;
	}
}

bool extractInt64(const ConstantLiteral& lit, int64_t& out) {
	bool ok = true;
	std::visit(
	    [&](auto&& v) {
		    using T = std::decay_t<decltype(v)>;
		    if constexpr (std::is_integral_v<T> && !std::is_same_v<T, bool>) {
			    out = static_cast<int64_t>(v);
		    } else if constexpr (std::is_same_v<T, bool>) {
			    out = v ? 1 : 0;
		    } else {
			    ok = false;
		    }
	    },
	    lit);
	return ok;
}

bool isComparisonOp(Op op) {
	switch (op) {
	case EQ:
	case NEQ:
	case LT:
	case LTE:
	case GT:
	case GTE:
		return true;
	default:
		return false;
	}
}

} // namespace

// Thread-local instance so the tracer survives across function-pointer reentry
// without re-allocation, matching the LazyTraceContext convention.
static thread_local ScopedTraceContext scopedTraceContext;

ScopedTraceContext* ScopedTraceContext::initialize(TagRecorder& tagRecorder, ExecutionTrace& executionTrace,
                                                   SymbolicExecutionContext& symbolicExecutionContext,
                                                   const engine::Options& options) {
	scopedTraceContext.state =
	    std::make_unique<TraceState>(tagRecorder, executionTrace, symbolicExecutionContext, options);
	scopedTraceContext.paused_ = false;
	setActiveTracer(&scopedTraceContext);
	return &scopedTraceContext;
}

void ScopedTraceContext::resume() {
	staticVars.clear();
	aliveVars.reset();
	paused_ = false;
	// Drop any predicates from the previous iteration; a fresh path begins.
	while (predicates_.size() > 0) {
		predicates_.pop();
	}
	comparisonCache.clear();
	integerConstants.clear();
	staticBoolValues.clear();
	hasIntegerConstants = false;
	hasComparisonCache = false;
	hasStaticBoolValues = false;
}

TypedValueRef& ScopedTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (paused_) {
		return dummyRef_;
	}
	return state->executionTrace.setArgument(type, index);
}

bool ScopedTraceContext::isFollowing() {
	return state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW;
}

TypedValueRef& ScopedTraceContext::follow([[maybe_unused]] Op op) {
	auto& currentOperation = state->executionTrace.getCurrentOperation();
	state->executionTrace.nextOperation();
	assert(currentOperation.op == op);
	return currentOperation.resultRef;
}

TypedValueRef& ScopedTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Constant (scoped)");
	auto op = Op::CONST;
	if (isFollowing()) {
		auto& f = follow(op);
		// Track integer constants seen during FOLLOW too — they live in the
		// trace and may feed into a subsequently-recorded CMP / traceBool.
		int64_t litValue = 0;
		if (f.ref != 0 && extractInt64(constValue, litValue)) {
			integerConstants[f.ref] = litValue;
			hasIntegerConstants = true;
		}
		return f;
	}
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	TypedValueRef* returned;
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		auto* originalRef = state->executionTrace.getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		state->executionTrace.addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		returned = &originalRef->resultRef;
	} else {
		returned = &state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
	}
	// Populate the integer-constant cache so traceBinaryOp can short-circuit
	// `const cmp const` comparisons into a known bool.
	int64_t litValue = 0;
	if (returned->ref != 0 && extractInt64(constValue, litValue)) {
		integerConstants[returned->ref] = litValue;
		hasIntegerConstants = true;
	}
	return *returned;
}

template <typename OnCreation>
TypedValueRef& ScopedTraceContext::traceOperation(Op op, OnCreation&& onCreation) {
	if (paused_) {
		return dummyRef_;
	}
	if (isFollowing()) {
		return follow(op);
	}
	auto tag = recordSnapshot();
	if (safeCheckTag(tag)) {
		return onCreation(tag);
	}
	// Repeated tag -> control-flow merge (already applied by safeCheckTag) or
	// same-block self-merge that safeCheckTag avoided.  Either way, enter
	// passive mode so the rest of the C++ function runs to natural completion
	// without recording further operations.
	paused_ = true;
	return dummyRef_;
}

bool ScopedTraceContext::safeCheckTag(Snapshot& snapshot) {
	// Defensive replacement for ExecutionTrace::checkTag: the underlying
	// processControlFlowMerge() throws RuntimeException when the recorded op
	// and the current op are in the *same* block, which happens under the
	// scoped tracer's stable Snapshot scheme on shapes like
	// pathExplosion_constraintBlind_dead.  We detect that case and request a
	// graceful passive-mode fallback (caller pauses the trace).
	auto& trace = state->executionTrace;
	auto globalIt = trace.globalTagMap.find(snapshot);
	if (globalIt != trace.globalTagMap.end()) {
		if (globalIt->second.blockIndex == trace.getCurrentBlockIndex()) {
			return false;
		}
		trace.processControlFlowMerge(globalIt->second);
		return false;
	}
	auto localIt = trace.localTagMap.find(snapshot);
	if (localIt != trace.localTagMap.end()) {
		if (localIt->second.blockIndex == trace.getCurrentBlockIndex()) {
			return false;
		}
		trace.processControlFlowMerge(localIt->second);
		return false;
	}
	return true;
}

TypedValueRef& ScopedTraceContext::traceAlloca(size_t size, size_t align) {
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	return traceOperation(op, [&, size, align](Snapshot& tag) -> TypedValueRef& {
		auto index = state->executionTrace.addAllocaSpec(size, align);
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	});
}

TypedValueRef& ScopedTraceContext::traceCopy(const TypedValueRef& ref) {
	if (paused_) {
		return dummyRef_;
	}
	log::debug("Trace Copy (scoped)");
	auto& result = traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		auto resultRef = state->executionTrace.getNextValueRef();
		return state->executionTrace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
	});
	// Propagate the source's known integer / bool value to the copy so later
	// CMPs can still short-circuit even after the val<T> got copied around.
	// Pay-as-you-go: skip the lookups entirely until the corresponding cache
	// has received at least one entry.
	if (!paused_ && result.ref != 0) {
		if (hasIntegerConstants) {
			if (auto it = integerConstants.find(ref.ref); it != integerConstants.end()) {
				integerConstants[result.ref] = it->second;
			}
		}
		if (hasStaticBoolValues) {
			if (auto it = staticBoolValues.find(ref.ref); it != staticBoolValues.end()) {
				staticBoolValues[result.ref] = it->second;
			}
		}
	}
	return result;
}

TypedValueRef& ScopedTraceContext::traceCall(void* fptn, Type resultType,
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

TypedValueRef& ScopedTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
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

TypedValueRef& ScopedTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                     std::function<void()> fwrapper, Type resultType,
                                                     const std::vector<tracing::TypedValueRef>& arguments,
                                                     FunctionAttributes fnAttrs) {
	if (paused_) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	auto mangledName = getMangledName((void*) definition);
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(compiler::CompilableFunction(functionName, fwrapper, definition->attributes()));
		log::debug("[scoped] Added function '{}' to functionsToTrace list", functionName);
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

TypedValueRef& ScopedTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                            std::function<void()> fwrapper) {
	if (paused_) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(
		    compiler::CompilableFunction(functionName, std::move(fwrapper), definition->attributes()));
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

void ScopedTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (paused_) {
		return;
	}
	traceOperation(ASSIGN, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addAssignmentOperation(tag, target, source, resultType);
	});
}

void ScopedTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	if (paused_) {
		return;
	}
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		state->executionTrace.addReturn(tag, resultType, ref);
	}
}

TypedValueRef& ScopedTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                 const TypedValueRef& right) {
	if (paused_) {
		return dummyRef_;
	}
	auto& result = traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	});

	// Pay-as-you-go: the lookups below only fire after at least one
	// integer-CONST has been recorded.  Workloads with no integer constants
	// (e.g. nestedIf100 against a symbolic argument) skip the whole block
	// after a single boolean check.
	if (!paused_ && hasIntegerConstants && isComparisonOp(op) && result.ref != 0) {
		auto leftIt = integerConstants.find(left.ref);
		auto rightIt = integerConstants.find(right.ref);
		const bool leftKnown = leftIt != integerConstants.end();
		const bool rightKnown = rightIt != integerConstants.end();

		if (leftKnown && rightKnown) {
			// Fully constant comparison — record the static bool result so
			// traceBool can prune the branch without any predicate reasoning.
			staticBoolValues[result.ref] = evaluateConstCmp(op, leftIt->second, rightIt->second);
			hasStaticBoolValues = true;
		} else if (leftKnown) {
			// `lit cmp var` — compute the equivalent `var cmp' lit` by mirroring
			// the comparison operator.
			Op mirrored = op;
			switch (op) {
			case LT:
				mirrored = GT;
				break;
			case LTE:
				mirrored = GTE;
				break;
			case GT:
				mirrored = LT;
				break;
			case GTE:
				mirrored = LTE;
				break;
			default:
				break;
			}
			comparisonCache[result.ref] = ComparisonInfo {mirrored, right.ref, leftIt->second, true};
			hasComparisonCache = true;
		} else if (rightKnown) {
			// Common case: `var cmp lit`.
			comparisonCache[result.ref] = ComparisonInfo {op, left.ref, rightIt->second, true};
			hasComparisonCache = true;
		}
		// Otherwise: variable vs variable — no predicate can be derived.
	}

	return result;
}

TypedValueRef& ScopedTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (paused_) {
		return dummyRef_;
	}
	auto& result = traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	});

	// Propagate predicate info across a NOT: `!(x == c)` is `x != c`.  Skipped
	// entirely if no comparison-cache entry has been recorded yet.
	if (!paused_ && hasComparisonCache && op == NOT && result.ref != 0) {
		auto it = comparisonCache.find(input.ref);
		if (it != comparisonCache.end() && it->second.valid) {
			comparisonCache[result.ref] =
			    ComparisonInfo {negateOp(it->second.cmpOp), it->second.var, it->second.lit, true};
			// hasComparisonCache already true (we're inside the gate).
		}
	}
	return result;
}

TypedValueRef& ScopedTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                  const TypedValueRef& second, const TypedValueRef& third) {
	if (paused_) {
		return dummyRef_;
	}
	return traceOperation(op, [&](Snapshot& tag) -> TypedValueRef& {
		return state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	});
}

bool ScopedTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	if (paused_) {
		return false;
	}

	bool result;
	bool shouldTerminate = false;

	// Derive the candidate predicate for this CMP up front so it is available
	// in both FOLLOW and RECORD paths.
	auto candidate = derivePredicate(value);

	if (state->symbolicExecutionContext.getCurrentMode() == SymbolicExecutionContext::MODE::FOLLOW) {
		auto recordResult = state->symbolicExecutionContext.followNoThrow();
		result = recordResult.branchDirection;
		shouldTerminate = recordResult.shouldTerminate;
	} else {
		auto tag = recordSnapshot();
		if (!safeCheckTag(tag)) {
			paused_ = true;
			return false;
		}
		state->executionTrace.addCmpOperation(tag, value, probability);

		// Pruning order:
		//   1. Static bool from constant-folded CMP (left & right both Op::CONST).
		//      This handles cases like `1 == 42` after an inlined return.
		//   2. PathPredicateStore for dominator-implied / contradicted CMPs.
		//   3. Otherwise, hand the decision to the symbolic executor.
		//
		// Both lookups are gated so workloads without integer constants or
		// dominator predicates skip them with a single boolean check.
		std::optional<bool> pruned;
		if (hasStaticBoolValues) {
			if (auto sit = staticBoolValues.find(value.ref); sit != staticBoolValues.end()) {
				pruned = sit->second;
			}
		}
		if (!pruned.has_value() && candidate.isValid && predicates_.size() > 0) {
			pruned = predicates_.evaluate(candidate);
		}

		RecordResult recordResult = pruned.has_value()
		                                ? state->symbolicExecutionContext.recordPrunedNoThrow(tag, *pruned)
		                                : state->symbolicExecutionContext.recordNoThrow(tag);
		result = recordResult.branchDirection;
		shouldTerminate = recordResult.shouldTerminate;
	}

	// Push the predicate aligned with the direction actually taken.  Negate
	// the comparison op when the false arm was selected so the store reflects
	// the constraint that holds along this path.
	if (candidate.isValid) {
		PathPredicateStore::Predicate effective = candidate;
		if (!result) {
			effective.cmpOp = negateOp(effective.cmpOp);
		}
		predicates_.push(effective);
	} else {
		// Push an invalid predicate so push/pop stays balanced with the number
		// of branch decisions taken.  evaluate() ignores invalid entries.
		predicates_.push(candidate);
	}

	if (shouldTerminate) {
		paused_ = true;
		return false;
	}

	auto& currentOperation = state->executionTrace.getCurrentOperation();
	assert(currentOperation.op == CMP);

	uint32_t nextBlock;
	if (result) {
		nextBlock = std::get<BlockRef*>(currentOperation.input[1])->block;
	} else {
		nextBlock = std::get<BlockRef*>(currentOperation.input[2])->block;
	}
	state->executionTrace.setCurrentBlock(nextBlock);
	return result;
}

PathPredicateStore::Predicate ScopedTraceContext::derivePredicate(const TypedValueRef& boolRef) const {
	// Gate the map lookup behind the comparison-cache flag.  When no CMP op
	// has produced a useful predicate yet, we return the invalid sentinel
	// directly without paying the hash-map probe.
	if (!hasComparisonCache) {
		return PathPredicateStore::Predicate {0, EQ, 0, false};
	}
	auto it = comparisonCache.find(boolRef.ref);
	if (it == comparisonCache.end() || !it->second.valid) {
		return PathPredicateStore::Predicate {0, EQ, 0, false};
	}
	const auto& info = it->second;
	return PathPredicateStore::Predicate {info.var, info.cmpOp, info.lit, true};
}

Op ScopedTraceContext::negateOp(Op op) noexcept {
	switch (op) {
	case EQ:
		return NEQ;
	case NEQ:
		return EQ;
	case LT:
		return GTE;
	case LTE:
		return GT;
	case GT:
		return LTE;
	case GTE:
		return LT;
	default:
		return op;
	}
}

std::unique_ptr<ExecutionTrace> ScopedTraceContext::trace(std::function<void()>& traceFunction,
                                                          const engine::Options& options, Arena& arena) {
	log::debug("Initialize Scoped Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	SymbolicExecutionContext symbolicExecutionContext;
	auto tc = initialize(tr, *executionTrace, symbolicExecutionContext, options);
	auto traceIteration = 0;

	while (symbolicExecutionContext.shouldContinue()) {
		traceIteration = traceIteration + 1;
		log::trace("Scoped Trace Iteration {}", traceIteration);
		log::trace("{}", *executionTrace);

		symbolicExecutionContext.next();
		executionTrace->resetExecution();
		tc->resume();

		traceFunction();

		assert(scopedTraceContext.staticVars.empty() &&
		       "static variable stack not empty after scoped tracing iteration");
	}

	setActiveTracer(nullptr);
	tc->state.reset();

	log::debug("Scoped Tracing Terminated with {} iterations", traceIteration);
	log::trace("Final trace: {}", *executionTrace);

	return executionTrace;
}

std::unique_ptr<TraceModule> ScopedTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                       const engine::Options& options, Arena& arena) {
	return scopedTraceContext.startTrace(functions, options, arena);
}

std::unique_ptr<TraceModule> ScopedTraceContext::startTrace(std::list<compiler::CompilableFunction>& functions,
                                                            const engine::Options& options, Arena& arena) {
	log::debug("Initialize Scoped Multi-function Tracing");
	auto traceModule = std::make_unique<TraceModule>();
	functionsToTrace = functions;
	registeredFunctions.clear();
	setActiveTracer(this);

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

		auto rootAddress = __builtin_return_address(0);
		auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);
		SymbolicExecutionContext symbolicExecutionContext;
		state = std::make_unique<TraceState>(tr, executionTrace, symbolicExecutionContext, options);
		auto traceIteration = 0;

		while (symbolicExecutionContext.shouldContinue()) {
			traceIteration = traceIteration + 1;
			log::trace("Scoped Trace Iteration {}", traceIteration);
			log::trace("{}", executionTrace);
			symbolicExecutionContext.next();
			executionTrace.resetExecution();
			resume();
			wrapperFunc();
			assert(staticVars.empty() && "static variable stack not empty after scoped tracing iteration");
		}

		state.reset();
		log::debug("Scoped Tracing Terminated with {} iterations", traceIteration);
		log::trace("Final trace: {}", executionTrace);
	}

	setActiveTracer(nullptr);
	return traceModule;
}

void ScopedTraceContext::allocateValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.increment(ref);
}

void ScopedTraceContext::freeValRef(ValueRef ref) {
	if (paused_) {
		return;
	}
	aliveVars.decrement(ref);
}

void ScopedTraceContext::pushStaticVal(void* valPtr, size_t size) {
	staticVars.emplace_back(valPtr, size);
	if (!paused_ && log::options::getLogStaticVars()) {
		log::info("pushStaticVal: [{}]", formatStaticVars());
	}
}

void ScopedTraceContext::popStaticVal() {
	if (!paused_ && log::options::getLogStaticVars()) {
		log::info("popStaticVal: [{}] (popping last)", formatStaticVars());
	}
	staticVars.pop_back();
}

std::string ScopedTraceContext::formatStaticVars() const {
	std::string result;
	for (size_t i = 0; i < staticVars.size(); i++) {
		if (i > 0) {
			result += ", ";
		}
		result += std::to_string(getStaticVarValue(staticVars[i]));
	}
	return result;
}

Snapshot ScopedTraceContext::recordSnapshot() {
	// Snapshot identity matches LazyTraceContext: (Tag*, staticHash ^ aliveHash).
	//
	// An earlier revision of this tracer omitted aliveVars.hash() in an attempt
	// to collapse the postCallBranch fan-out described in
	// PathExplosionFunctions.hpp:45-67, but it produced malformed traces:
	// without per-iteration discrimination, RETURN ops at the same source line
	// across different symbolic-execution iterations collide on a single
	// Snapshot.  processControlFlowMerge then accumulates their inputs into one
	// op, and SSACreationPhase rejects the result with "Wrong number of
	// arguments in trace".
	//
	// The actual path-explosion fix needs a different lever: per-function
	// sub-traces so the callee body is recorded once and re-encounters emit a
	// CALL op (see the scope_stack work in the same TU).  Until that lever
	// is in place, this Snapshot is structurally identical to Lazy's; the
	// added value of ScopedTraceContext over Lazy is the PathPredicateStore
	// it maintains for forthcoming symbolic-executor pruning.
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash()};
}

} // namespace nautilus::tracing
