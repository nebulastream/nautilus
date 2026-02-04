#include "SymbolicExecutionTraceContext.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/symbolic_tracing/SymbolicExecutionState.hpp"
#include "nautilus/tracing/symbolic_tracing/TraceTerminationException.hpp"
#include <cassert>
#include <cxxabi.h>
#include <dlfcn.h>
#include <fmt/format.h>
#include <sstream>
#include <sys/stat.h>

namespace nautilus::tracing {

bool SymbolicExecutionTraceContext::isFollowing() {
	return symbolicExecutionState->getCurrentMode() == SymbolicExecutionState::MODE::FOLLOW;
}

TypedValueRef& SymbolicExecutionTraceContext::follow([[maybe_unused]] Op op) {
	skipJumps();
	assert(currentBlockIterator != currentBlockOps.end());
	assert(currentBlockIterator->view->op == op);
	auto& currentOperation = currentBlockIterator->getResultRef();
	++currentBlockIterator;
	return currentOperation;
}

void SymbolicExecutionTraceContext::skipJumps() { // NOLINT(misc-no-recursion)
	// process and skip jump operations that were added during control flow merging
	assert(currentBlockIterator != currentBlockOps.end());
	if (currentBlockIterator->view->op == JMP) {
		auto& arg = state->executionTrace.getCurrentBlock().getOperations().back().arguments<JmpInput>();
		moveCurrentBlock(arg.targetBlock);
		skipJumps();
	}
}

void SymbolicExecutionTraceContext::moveCurrentBlock(u_int16_t targetBlockId) {
	// move trace pointer to the target block
	state->executionTrace.view->currentBlockOffset = blockMap[targetBlockId];
	currentBlockOps = state->executionTrace.getCurrentBlock().getOperations();
	currentBlockIterator = currentBlockOps.begin();
}

void SymbolicExecutionTraceContext::saveCurrentBlock() {
	// add current block with its ID and offset to the blockMap
	blockMap[state->executionTrace.getCurrentBlock().getBlockId()] = state->executionTrace.view->currentBlockOffset;
}

TypedValueRef SymbolicExecutionTraceContext::traceConstValue(Type type, const ConstantLiteral& constValue) {
	log::debug("Trace Constant");
	auto op = Op::CONST;
	if (isFollowing()) {
		return follow(op);
	}
	return TraceContext::traceConstValue(type, constValue);
}

TypedValueRef SymbolicExecutionTraceContext::traceOperation(
    [[maybe_unused]] Op op,
    const std::function<std::pair<TypedValueRef, Trace::TracerOperationIdentifier>(Snapshot&)>& onCreation) {
	if (isFollowing()) {
		return follow(op);
	} else {
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			auto [result, oi] = onCreation(tag);
			state->executionTrace.globalTagMap.insert(tag, oi);
			return result;
		} else {
			saveCurrentBlock();
			// TODO find a way to handle this more graceful.
			throw TraceTerminationException();
		}
	}
}

TypedValueRef SymbolicExecutionTraceContext::registerFunctionArgument(Type type, size_t index) {
	return state->executionTrace.setArgument(type, index);
}

void SymbolicExecutionTraceContext::traceReturnOperation(Type resultType, const TypedValueRef& ref) {
	log::debug("Trace Return");
	assert(resultType == ref.type && "I hope this is true");
	if (isFollowing()) {
		follow(RETURN);
	} else {
		auto tag = recordSnapshot();
		state->executionTrace.addReturn(tag, ref);
	}
}

bool SymbolicExecutionTraceContext::traceCmp(const TypedValueRef& targetRef, const double probability) {
	constexpr auto blockIdPlaceholder = std::numeric_limits<std::uint16_t>::max();

	log::debug("Trace CMP");
	bool result;
	if (symbolicExecutionState->getCurrentMode() == SymbolicExecutionState::MODE::FOLLOW) {
		// eval execution path one step
		// we repeat the operation
		skipJumps();

		// jump to next block; create if it doesnt exist yet
		result = symbolicExecutionState->follow();
		auto& args = state->executionTrace.getCurrentBlock().getOperations().back().arguments<CmpInput>();
		uint16_t blockId;
		if (result) {
			blockId = args.trueBlock;
			if (blockId == blockIdPlaceholder) {
				blockId = state->executionTrace.createNewBlock().getBlockId();
				args.trueBlock = blockId;
				saveCurrentBlock();
			}
		} else {
			blockId = args.falseBlock;
			if (blockId == blockIdPlaceholder) {
				blockId = state->executionTrace.createNewBlock().getBlockId();
				args.falseBlock = blockId;
				saveCurrentBlock();
			}
		}
		moveCurrentBlock(blockId);
	} else {
		// record
		auto tag = recordSnapshot();
		if (state->executionTrace.checkTag(tag)) {
			// Create CMP operation
			auto oi = std::get<1>(state->executionTrace.appendCmp(tag, targetRef, probability));
			state->executionTrace.globalTagMap.insert(tag, oi);
			result = symbolicExecutionState->record(tag);

			// Create next block and move into it
			auto blockId = state->executionTrace.createNewBlock().getBlockId();
			auto& args = state->executionTrace.getOperation(oi).arguments<CmpInput>();
			if (result) {
				args.trueBlock = blockId;
				args.falseBlock = blockIdPlaceholder;
			} else {
				args.falseBlock = blockId;
				args.trueBlock = blockIdPlaceholder;
			}
			saveCurrentBlock();
		} else {
			// this is actually the same tag -> throw up
			throw TraceTerminationException();
		}
	}
	return result;
}

void SymbolicExecutionTraceContext::resetState() {
	TraceContext::resetState();
	blockMap.clear();
	symbolicExecutionState = nullptr;
}

std::unique_ptr<ExecutionTrace> SymbolicExecutionTraceContext::trace(std::function<void()>& traceFunction,
                                                                     const engine::Options& options) {
	log::debug("Initialize Tracing");
	auto rootAddress = __builtin_return_address(0);
	auto tr = tracing::TagRecorder((tracing::TagAddress) rootAddress);

	// Allocate Trace (shared memory, no heap allocs) and SymbolicExecutionContext on the stack
	Trace tracerTrace;
	SymbolicExecutionState seCtx;

	// Initialize TraceContext with references to our stack objects
	initialize(tr, tracerTrace, options);
	symbolicExecutionState = &seCtx;

	// Create entry block
	tracerTrace.createNewBlock();
	saveCurrentBlock();
	auto initialBlock = tracerTrace.getCurrentBlock().getBlockId();

	// Symbolic execution loop: explore all execution paths
	auto traceIteration = 0;
	while (symbolicExecutionState->shouldContinue()) {
		try {
			traceIteration = traceIteration + 1;
			log::trace("Trace Iteration {}", traceIteration);

			// Prepare for next iteration
			symbolicExecutionState->next();

			// Reset persistent state (staticVars, aliveVars)
			resume();

			// Execute the traced function
			traceFunction();
		} catch (const TraceTerminationException& ex) {
			// Normal termination when we hit a known control flow merge or loop
		}
		moveCurrentBlock(initialBlock);
	}

	// Clean up: reset tracer state
	resetState();

	log::debug("Tracing Terminated with {} iterations", traceIteration);

	auto executionTrace = toExecutionTrace(tracerTrace);
	log::trace("Final trace: {}", executionTrace);

	return std::make_unique<ExecutionTrace>(std::move(executionTrace));
}

} // namespace nautilus::tracing
