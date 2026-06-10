
#include "ReplayTraceContext.hpp"
#include "TraceOperation.hpp"
#include "nautilus/CompilableFunction.hpp"
#include "nautilus/common/FunctionAttributes.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/nautilus_function.hpp"
#include "nautilus/tracing/TracingUtil.hpp"
#include "nautilus/tracing/exceptions/TraceDivergenceException.hpp"
#include "nautilus/tracing/exceptions/TraceIncompleteException.hpp"
#include "nautilus/tracing/exceptions/TraceLimitException.hpp"
#include "symbolic_execution/SymbolicExecutionContext.hpp"
#include <cassert>
#include <fmt/format.h>

namespace nautilus::tracing {

// Thread-local ReplayTraceContext object (not a pointer)
static thread_local ReplayTraceContext replayTraceContext;

void ReplayTraceContext::readOptions(const engine::Options& options) {
	maxIterations = static_cast<uint64_t>(options.getOptionOrDefault<int>("engine.traceMaxIterations", 100000));
	passiveStepLimit = static_cast<uint64_t>(options.getOptionOrDefault<int>("engine.tracePassiveStepLimit", 1000000));
	validateReplay = options.getOptionOrDefault<bool>("engine.traceValidateReplay", true);
}

Snapshot ReplayTraceContext::recordSnapshot() {
	return {state->tagRecorder.createTag(), hashStaticVector(staticVars) ^ aliveVars.hash() ^ prunedSitesHash};
}

void ReplayTraceContext::addPrunedSite(uint64_t key) {
	if (prunedSites.insert(key).second) {
		prunedSitesHash ^= key;
	}
}

Snapshot ReplayTraceContext::recordSnapshot(const std::optional<ConstantLiteral>& constFingerprint) {
	uint64_t stateHash = hashStaticVector(staticVars) ^ aliveVars.hash() ^ prunedSitesHash;
	if (constFingerprint) {
		// Operations in a trace-time-constant region must never share a
		// snapshot with runtime re-executions of the same site: a branch on
		// the constant region was pruned (no CMP recorded), so a tag-map
		// collision across the constness boundary would close a "loop" that
		// has no exit branch. Mixing the folded literal into the snapshot -
		// analogous to how static_val values are hashed - keeps the regions
		// distinct. The fingerprint is deterministic per ref, so cross-path
		// and cross-iteration merging within the same region is unaffected.
		stateHash ^= OperandHash().mix(*constFingerprint).value;
	}
	return {state->tagRecorder.createTag(), stateHash};
}

void ReplayTraceContext::enterPassive() {
	mode = Mode::Passive;
}

void ReplayTraceContext::latchDivergence(Op actualOp, uint64_t actualHash) {
	DivergenceInfo info;
	info.callIndex = cursor;
	info.actualOp = actualOp;
	info.actualHash = actualHash;
	if (cursor < pathLog.size()) {
		info.cursorOverrun = false;
		info.expectedOp = pathLog[cursor].op;
		info.expectedHash = pathLog[cursor].operandHash;
	} else {
		info.cursorOverrun = true;
		info.expectedOp = actualOp;
		info.expectedHash = 0;
	}
	divergence = info;
	enterPassive();
}

LogEntry* ReplayTraceContext::checkFollowEntry(Op op, uint64_t operandHash, bool wantsResult) {
	if (cursor >= pathLog.size()) {
		latchDivergence(op, operandHash);
		return nullptr;
	}
	auto& entry = pathLog[cursor];
	bool hasResult = (entry.flags & LogEntry::HAS_RESULT) != 0;
	if (entry.op != op || hasResult != wantsResult || (validateReplay && entry.operandHash != operandHash)) {
		latchDivergence(op, operandHash);
		return nullptr;
	}
	cursor++;
	return &entry;
}

TypedValueRef& ReplayTraceContext::followValue(Op op, uint64_t operandHash) {
	if (auto* entry = checkFollowEntry(op, operandHash, /*wantsResult=*/true)) {
		return entry->result;
	}
	return dummyRef_;
}

TypedValueRef& ReplayTraceContext::registerFunctionArgument(Type type, size_t index) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	// Argument registration is not logged: it is executed for real in RECORD
	// and FOLLOW alike (setArgument is idempotent per index), mirroring the
	// behavior of the existing contexts.
	auto& ref = state->executionTrace.setArgument(type, index);
	constants.setRuntime(ref.ref);
	return ref;
}

TypedValueRef& ReplayTraceContext::traceConstant(Type type, const ConstantLiteral& constValue) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto op = Op::CONST;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, type).mix(constValue).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto tag = recordSnapshot();
	auto globalTabIter = state->executionTrace.globalTagMap.find(tag);
	if (globalTabIter != state->executionTrace.globalTagMap.end()) {
		// Repeated constant site: emit a fresh CONST plus an ASSIGN to the
		// original ref and return the original ref, so refs defined in one
		// branch arm are also defined on the other arm.
		auto& ref = globalTabIter->second;
		auto* originalRef = state->executionTrace.getBlocks()[ref.blockIndex]->operations[ref.operationIndex];
		auto resultRef = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
		constants.setConst(resultRef.ref, constValue);
		state->executionTrace.addAssignmentOperation(tag, originalRef->resultRef, resultRef, resultRef.type);
		pathLog.push_back({op, LogEntry::HAS_RESULT, type, originalRef->resultRef, hash});
		return originalRef->resultRef;
	}
	auto& result = state->executionTrace.addOperationWithResult(tag, op, type, {constValue});
	constants.setConst(result.ref, constValue);
	pathLog.push_back({op, LogEntry::HAS_RESULT, type, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceAlloca(size_t size, size_t align) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto op = Op::ALLOCA;
	auto resultType = Type::ptr;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, resultType).mix(size).mix(align).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	// The alloca spec is only appended for genuinely new alloca sites: in this
	// context the creation path runs exclusively in RECORD mode after a
	// successful checkTag, preserving the one-spec-per-site invariant.
	auto index = state->executionTrace.addAllocaSpec(size, align);
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {index});
	constants.setRuntime(result.ref);
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceCopy(const TypedValueRef& ref) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto op = Op::ASSIGN;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, ref.type).mix(ref).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	// A copy inherits the constness of its source.
	auto sourceLiteral = constants.get(ref.ref);
	auto tag = recordSnapshot(sourceLiteral);
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto resultRef = state->executionTrace.getNextValueRef();
	auto& result = state->executionTrace.addAssignmentOperation(tag, {resultRef, ref.type}, ref, ref.type);
	if (sourceLiteral) {
		constants.setConst(result.ref, *sourceLiteral);
	} else {
		constants.setRuntime(result.ref);
	}
	pathLog.push_back({op, LogEntry::HAS_RESULT, ref.type, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceBinaryOp(Op op, Type resultType, const TypedValueRef& left,
                                                 const TypedValueRef& right) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, resultType).mix(left).mix(right).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto leftLiteral = constants.get(left.ref);
	auto rightLiteral = constants.get(right.ref);
	std::optional<ConstantLiteral> folded;
	if (leftLiteral && rightLiteral) {
		folded = ConstantTracker::foldBinary(op, resultType, *leftLiteral, *rightLiteral);
	}
	auto tag = recordSnapshot(folded);
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {left, right});
	if (folded) {
		constants.setConst(result.ref, *folded);
	} else {
		constants.setRuntime(result.ref);
	}
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceUnaryOp(Op op, Type resultType, const TypedValueRef& input) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, resultType).mix(input).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	std::optional<ConstantLiteral> folded;
	if (auto literal = constants.get(input.ref)) {
		folded = ConstantTracker::foldUnary(op, resultType, *literal);
	}
	auto tag = recordSnapshot(folded);
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {input});
	if (folded) {
		constants.setConst(result.ref, *folded);
	} else {
		constants.setRuntime(result.ref);
	}
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceTernaryOp(Op op, Type resultType, const TypedValueRef& first,
                                                  const TypedValueRef& second, const TypedValueRef& third) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, resultType).mix(first).mix(second).mix(third).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {first, second, third});
	constants.setRuntime(result.ref);
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

void ReplayTraceContext::traceReturnOperation(Type type, const TypedValueRef& ref) {
	if (mode == Mode::Passive) {
		return;
	}
	auto op = Op::RETURN;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, type).mix(ref).value;
	}
	if (mode == Mode::Follow) {
		// The replayed prefix never contains a RETURN (a return always ends the
		// iteration that recorded it and is discarded by log truncation), so
		// this validation always latches a divergence.
		checkFollowEntry(op, hash, /*wantsResult=*/false);
		return;
	}
	auto tag = recordSnapshot();
	state->executionTrace.addReturn(tag, type, ref);
	pathLog.push_back({op, 0, type, {0, Type::v}, hash});
}

void ReplayTraceContext::traceAssignment(const TypedValueRef& target, const TypedValueRef& source, Type resultType) {
	if (mode == Mode::Passive) {
		return;
	}
	auto op = Op::ASSIGN;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, resultType).mix(target).mix(source).value;
	}
	if (mode == Mode::Follow) {
		checkFollowEntry(op, hash, /*wantsResult=*/false);
		return;
	}
	auto tag = recordSnapshot(constants.get(source.ref));
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return;
	}
	state->executionTrace.addAssignmentOperation(tag, target, source, resultType);
	// An assignment target is mutable state whose value can differ per path;
	// it is always killed to Runtime, which keeps per-path constness sound
	// across control-flow merges and bounds trace-time loop unrolling.
	constants.setRuntime(target.ref);
	pathLog.push_back({op, 0, resultType, {0, Type::v}, hash});
}

TypedValueRef& ReplayTraceContext::traceCall(void* fptn, Type resultType,
                                             const std::vector<tracing::TypedValueRef>& arguments,
                                             FunctionAttributes fnAttrs) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto op = Op::CALL;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		auto hasher = OperandHash().mix(op, resultType).mix(reinterpret_cast<uint64_t>(fptn));
		for (const auto& argument : arguments) {
			hasher.mix(argument);
		}
		hash = hasher.value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto mangledName = getMangledName(fptn);
	auto functionName = getFunctionName(fptn, mangledName);
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto* functionArguments =
	    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
	                                                                        .mangledName = mangledName,
	                                                                        .ptr = fptn,
	                                                                        .arguments = arguments,
	                                                                        .fnAttrs = fnAttrs});
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	constants.setRuntime(result.ref);
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceIndirectCall(const TypedValueRef& fnPtrRef, Type resultType,
                                                     const std::vector<tracing::TypedValueRef>& arguments,
                                                     FunctionAttributes fnAttrs) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto op = Op::INDIRECT_CALL;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		auto hasher = OperandHash().mix(op, resultType).mix(fnPtrRef);
		for (const auto& argument : arguments) {
			hasher.mix(argument);
		}
		hash = hasher.value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto* indirectCall = state->executionTrace.getArena().create<IndirectFunctionCall>(
	    IndirectFunctionCall {.fnPtr = fnPtrRef, .arguments = arguments, .fnAttrs = fnAttrs});
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {indirectCall});
	constants.setRuntime(result.ref);
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceNautilusCall(const NautilusFunctionDefinition* definition,
                                                     std::function<void()> fwrapper, Type resultType,
                                                     const std::vector<tracing::TypedValueRef>& arguments,
                                                     FunctionAttributes fnAttrs) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(compiler::CompilableFunction(functionName, fwrapper, definition->attributes()));
		log::debug("Added function '{}' to functionsToTrace list. List now has {} functions", functionName,
		           functionsToTrace.size());
	}
	auto op = Op::CALL;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		auto hasher = OperandHash().mix(op, resultType).mix(reinterpret_cast<uint64_t>(definition));
		for (const auto& argument : arguments) {
			hasher.mix(argument);
		}
		hash = hasher.value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto* functionArguments =
	    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
	                                                                        .mangledName = functionName,
	                                                                        .ptr = (void*) definition,
	                                                                        .arguments = arguments,
	                                                                        .fnAttrs = fnAttrs});
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	constants.setRuntime(result.ref);
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

TypedValueRef& ReplayTraceContext::traceNautilusFunctionPtr(const NautilusFunctionDefinition* definition,
                                                            std::function<void()> fwrapper) {
	if (mode == Mode::Passive) {
		return dummyRef_;
	}
	auto functionName = definition->name();
	if (registeredFunctions.insert(functionName).second) {
		functionsToTrace.push_back(
		    compiler::CompilableFunction(functionName, std::move(fwrapper), definition->attributes()));
		log::debug("Added function '{}' to functionsToTrace list (via FUNC_ADDR). List now has {} functions",
		           functionName, functionsToTrace.size());
	}
	auto op = Op::FUNC_ADDR;
	auto resultType = Type::ptr;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, resultType).mix(reinterpret_cast<uint64_t>(definition)).value;
	}
	if (mode == Mode::Follow) {
		return followValue(op, hash);
	}
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		enterPassive();
		return dummyRef_;
	}
	auto* functionArguments =
	    state->executionTrace.getArena().create<FunctionCall>(FunctionCall {.functionName = functionName,
	                                                                        .mangledName = functionName,
	                                                                        .ptr = (void*) definition,
	                                                                        .arguments = {},
	                                                                        .fnAttrs = {}});
	auto& result = state->executionTrace.addOperationWithResult(tag, op, resultType, {functionArguments});
	constants.setRuntime(result.ref);
	pathLog.push_back({op, LogEntry::HAS_RESULT, resultType, result, hash});
	return result;
}

bool ReplayTraceContext::traceBool(const TypedValueRef& value, const double probability) {
	if (mode == Mode::Passive) {
		// Passive mode returns false to drive the function to its natural
		// exit. The step limit converts loops that can never exit this way
		// (break-only loops on runtime conditions) into a hard diagnostic
		// instead of an endless spin.
		if (++passiveSteps > passiveStepLimit) {
			throw TraceIncompleteException(
			    "Passive trace replay could not drive the traced function to its exit within " +
			    std::to_string(passiveStepLimit) +
			    " branch decisions. This usually means a loop that only exits via break on a runtime "
			    "condition (e.g. `for (;;) { if (c) break; }`), which passive mode cannot leave. "
			    "The limit is configurable via the option engine.tracePassiveStepLimit.");
		}
		return false;
	}
	auto op = Op::CMP;
	uint64_t hash = 0;
	if (wantsOperandHash()) {
		hash = OperandHash().mix(op, Type::v).mix(value).value;
	}
	if (mode == Mode::Follow) {
		auto* entry = checkFollowEntry(op, hash, /*wantsResult=*/false);
		if (entry == nullptr) {
			return false;
		}
		bool decision = entry->decision();
		if ((entry->flags & LogEntry::PRUNED) != 0) {
			// Rebuild the pruned-site poison set during replay so that the
			// post-flip RECORD segment computes the same snapshots as the
			// iteration that recorded the prefix.
			addPrunedSite(entry->prunedSiteKey);
		}
		// cursor was already advanced past the entry by checkFollowEntry.
		if (cursor - 1 == flipPos) {
			// This is the pending branch: its stored decision was pre-flipped
			// to false when it was popped. Record into the still-empty false
			// block from here on.
			mode = Mode::Record;
			state->executionTrace.setCurrentBlock(pendingFalseBlock);
		}
		return decision;
	}
	// Record mode: prune branches whose condition is a trace-time constant.
	// Nothing is recorded and the untaken side is never explored.
	if (auto folded = constants.tryFoldBool(value.ref)) {
		uint8_t flags = LogEntry::PRUNED;
		if (*folded) {
			flags |= LogEntry::DECISION;
		}
		const void* site = state->tagRecorder.createTag();
		uint64_t siteKey = OperandHash()
		                       .mix(reinterpret_cast<uint64_t>(site))
		                       .mix(hashStaticVector(staticVars))
		                       .mix(*folded ? 1 : 0)
		                       .value;
		addPrunedSite(siteKey);
		pathLog.push_back({op, flags, Type::v, {0, Type::v}, hash, siteKey});
		return *folded;
	}
	auto tag = recordSnapshot();
	if (!state->executionTrace.checkTag(tag)) {
		// Control flow merge/loop closure at this branch: the path is done.
		enterPassive();
		return false;
	}
	state->executionTrace.addCmpOperation(tag, value, probability);
	auto& cmpOperation = state->executionTrace.getCurrentOperation();
	assert(cmpOperation.op == CMP);
	uint32_t trueBlock = std::get<BlockRef*>(cmpOperation.input[1])->block;
	uint32_t falseBlock = std::get<BlockRef*>(cmpOperation.input[2])->block;
	worklist.push_back({static_cast<uint32_t>(pathLog.size()), falseBlock});
	pathLog.push_back({op, LogEntry::DECISION, Type::v, {0, Type::v}, hash});
	// Explore the true side first; the false side gets its own iteration.
	state->executionTrace.setCurrentBlock(trueBlock);
	return true;
}

void ReplayTraceContext::allocateValRef(ValueRef ref) {
	if (mode == Mode::Passive) {
		return;
	}
	aliveVars.increment(ref);
}

void ReplayTraceContext::freeValRef(ValueRef ref) {
	if (mode == Mode::Passive) {
		return;
	}
	aliveVars.decrement(ref);
}

void ReplayTraceContext::pushStaticVal(void* valPtr, size_t size) {
	// Always maintain the static variable stack, even in passive mode:
	// destructors of static_vals pushed before entering passive mode still
	// call popStaticVal afterwards.
	staticVars.emplace_back(valPtr, size);
}

void ReplayTraceContext::popStaticVal() {
	staticVars.pop_back();
}

void ReplayTraceContext::resetIterationState(Mode startMode) {
	staticVars.clear();
	aliveVars.reset();
	mode = startMode;
	cursor = 0;
	passiveSteps = 0;
	divergence.reset();
	prunedSites.clear();
	prunedSitesHash = 0;
}

void ReplayTraceContext::checkPostIteration(const std::string& functionName, uint64_t iteration) {
	if (divergence) {
		const auto& info = *divergence;
		if (info.cursorOverrun) {
			throw TraceDivergenceException(fmt::format(
			    "Trace replay diverged in function '{}' (iteration {}, call #{}): the function performed more "
			    "trace calls than previously recorded (observed {}). The traced function must be deterministic "
			    "across re-executions. Common causes: branching on invoke() results, time/random sources, "
			    "pointer addresses, or mutated external state.",
			    functionName, iteration, info.callIndex, toString(info.actualOp)));
		}
		throw TraceDivergenceException(fmt::format(
		    "Trace replay diverged in function '{}' (iteration {}, call #{}): expected {} (operand hash {:#x}), "
		    "observed {} (operand hash {:#x}). The traced function must be deterministic across re-executions. "
		    "Common causes: branching on invoke() results, time/random sources, pointer addresses, or mutated "
		    "external state.",
		    functionName, iteration, info.callIndex, toString(info.expectedOp), info.expectedHash,
		    toString(info.actualOp), info.actualHash));
	}
	if (mode == Mode::Follow) {
		throw TraceDivergenceException(fmt::format(
		    "Trace replay diverged in function '{}' (iteration {}): the function returned after {} trace calls, "
		    "before reaching the pending branch at call #{}. The traced function must be deterministic across "
		    "re-executions.",
		    functionName, iteration, cursor, flipPos));
	}
	if (!staticVars.empty()) {
		throw TraceDivergenceException(
		    fmt::format("Static variable stack not empty after tracing iteration {} of function '{}': {} entries "
		                "remain. push/pop of static_val values must be balanced.",
		                iteration, functionName, staticVars.size()));
	}
}

void ReplayTraceContext::runExplorationLoop(const std::function<void()>& wrapperFunc,
                                            const std::string& functionName) {
	auto& executionTrace = state->executionTrace;
	pathLog.clear();
	worklist.clear();
	constants.clear();
	flipPos = kNoFlip;
	uint64_t iteration = 0;

	// Iteration 0 records the spine of the function; every following iteration
	// replays the recorded prefix of one pending branch and records its false
	// side. LIFO order (depth-first) guarantees that truncating the log never
	// destroys a prefix some pending branch still needs: all remaining entries
	// have logPos <= the popped one.
	//
	// All iterations call the traced function from this single call site:
	// snapshot tags include the return address of this frame, so a second call
	// site would make tags from different iterations never match and break
	// control-flow merging.
	while (iteration == 0 || !worklist.empty()) {
		if (iteration > maxIterations) {
			throw TraceLimitException(
			    fmt::format("Tracing of function '{}' exceeded the maximum number of exploration iterations ({}). "
			                "The limit is configurable via the option engine.traceMaxIterations.",
			                functionName, maxIterations));
		}
		if (iteration == 0) {
			resetIterationState(Mode::Record);
		} else {
			auto pending = worklist.back();
			worklist.pop_back();
			assert(pending.logPos < pathLog.size());
			assert(pathLog[pending.logPos].op == Op::CMP && !(pathLog[pending.logPos].flags & LogEntry::PRUNED));
			// Discard the explored side's suffix and pre-flip the decision.
			pathLog.resize(pending.logPos + 1);
			pathLog[pending.logPos].flags &= static_cast<uint8_t>(~LogEntry::DECISION);
			flipPos = pending.logPos;
			pendingFalseBlock = pending.falseBlockId;
			resetIterationState(Mode::Follow);
		}
		executionTrace.resetExecution();
		wrapperFunc();
		checkPostIteration(functionName, iteration);
		iteration++;
	}
	log::debug("Replay tracing of '{}' terminated after {} iterations", functionName, iteration);
}

std::unique_ptr<ExecutionTrace> ReplayTraceContext::trace(std::function<void()>& traceFunction,
                                                          const engine::Options& options, Arena& arena) {
	log::debug("Initialize Replay Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// The ExecutionTrace borrows the caller-provided arena for all
	// allocations; the arena must outlive the returned trace.
	auto executionTrace = std::make_unique<ExecutionTrace>(arena);
	// The replay context does not use the symbolic execution context; the
	// instance only exists to satisfy the shared TraceState constructor.
	SymbolicExecutionContext unusedSymbolicExecutionContext;

	auto* tc = &replayTraceContext;
	tc->state = std::make_unique<TraceState>(tr, *executionTrace, unusedSymbolicExecutionContext, options);
	tc->readOptions(options);
	setActiveTracer(tc);
	ActiveTracerGuard activeTracerGuard;

	tc->runExplorationLoop(traceFunction, "execute");

	tc->state.reset();
	log::trace("Final trace: {}", executionTrace->toString());
	return executionTrace;
}

std::unique_ptr<TraceModule> ReplayTraceContext::Trace(std::list<compiler::CompilableFunction>& functions,
                                                       const engine::Options& options, Arena& arena) {
	return replayTraceContext.startTrace(functions, options, arena);
}

std::unique_ptr<TraceModule> ReplayTraceContext::startTrace(std::list<compiler::CompilableFunction>& functions,
                                                            const engine::Options& options, Arena& arena) {
	log::debug("Initialize Replay Tracing");
	auto traceModule = std::make_unique<TraceModule>();
	functionsToTrace = functions;
	registeredFunctions.clear();
	readOptions(options);
	setActiveTracer(this);
	// Ensure the thread-local active tracer is cleared even if an exception
	// escapes the per-function loop below.
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
		// Tag the first popped function as the entry point, mirroring the
		// other trace contexts.
		auto attributes = currentFunction.getAttributes();
		if (isFirstFunction) {
			attributes["entry"] = "true";
			isFirstFunction = false;
		}
		traceModule->setFunctionAttributes(currentFunction.getName(), attributes);
		auto wrapperFunc = currentFunction.getFunction();

		auto rootAddress = __builtin_return_address(0);
		auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);
		SymbolicExecutionContext unusedSymbolicExecutionContext;
		state = std::make_unique<TraceState>(tr, executionTrace, unusedSymbolicExecutionContext, options);

		runExplorationLoop(wrapperFunc, currentFunction.getName());

		state.reset();
		log::trace("Final trace: {}", executionTrace.toString());
	}

	// activeTracer is cleared by ActiveTracerGuard.
	return traceModule;
}

} // namespace nautilus::tracing
