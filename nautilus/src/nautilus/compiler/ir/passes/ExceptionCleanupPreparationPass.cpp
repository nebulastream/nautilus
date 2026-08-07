#include "nautilus/compiler/ir/passes/ExceptionCleanupPreparationPass.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <optional>
#include <unordered_set>

namespace nautilus::compiler::ir {

namespace {

[[noreturn]] void fail(const FunctionOperation& function, const BasicBlock& block, const Operation& operation,
                       const std::string& reason) {
	throw RuntimeException("Invalid exception-cleanup state in function '" + function.getName() + "', block " +
	                       std::to_string(block.getIdentifier().getId()) + ", operation " +
	                       operation.getIdentifier().toString() + ": " + reason);
}

struct CallCleanupState {
	bool mayThrow;
	std::optional<CleanupStateId> state;
};

std::optional<CallCleanupState> getCallCleanupState(const Operation& operation) {
	switch (operation.getOperationType()) {
	case Operation::OperationType::ProxyCallOp: {
		const auto& call = static_cast<const ProxyCallOperation&>(operation);
		return CallCleanupState {!call.getFunctionAttributes().noUnwind, call.getCleanupState()};
	}
	case Operation::OperationType::IndirectCallOp: {
		const auto& call = static_cast<const IndirectCallOperation&>(operation);
		return CallCleanupState {!call.getFunctionAttributes().noUnwind, call.getCleanupState()};
	}
	default:
		return std::nullopt;
	}
}

const CleanupState& resolveCleanupState(const FunctionOperation& function, const BasicBlock& block,
                                        const Operation& operation, CleanupStateId stateId) {
	const auto& states = function.getCleanupStates();
	if (stateId >= states.size()) {
		fail(function, block, operation,
		     "references cleanup state " + std::to_string(stateId) + " but the function has only " +
		         std::to_string(states.size()));
	}
	const auto& state = states[stateId];
	std::unordered_set<AllocaIndex> seen;
	for (const auto alloca : state.active) {
		if (alloca >= function.getAllocaSpecs().size()) {
			fail(function, block, operation,
			     "cleanup state " + std::to_string(stateId) + " references missing alloca " + std::to_string(alloca));
		}
		if (!function.getAllocaSpecs()[alloca].destructor.has_value()) {
			fail(function, block, operation,
			     "cleanup state " + std::to_string(stateId) + " references alloca " + std::to_string(alloca) +
			         " without a destructor");
		}
		if (!seen.insert(alloca).second) {
			fail(function, block, operation,
			     "cleanup state " + std::to_string(stateId) + " contains duplicate alloca " + std::to_string(alloca));
		}
	}
	return state;
}

CleanupPadId internPad(FunctionExceptionRegion& region, const CleanupState& state) {
	auto existing = std::find_if(region.pads.begin(), region.pads.end(),
	                             [&](const CleanupPad& pad) { return pad.active == state.active; });
	if (existing != region.pads.end()) {
		return existing->id;
	}
	auto id = static_cast<CleanupPadId>(region.pads.size());
	region.pads.push_back(CleanupPad {id, state.active});
	return id;
}

FunctionExceptionRegion prepareFunction(const FunctionOperation& function) {
	FunctionExceptionRegion region;
	for (const auto* block : function.getBasicBlocks()) {
		for (const auto* operation : block->getOperations()) {
			const auto callState = getCallCleanupState(*operation);
			if (!callState.has_value() || !callState->mayThrow) {
				continue;
			}

			std::optional<CleanupPadId> cleanup;
			if (callState->state.has_value()) {
				const auto& state = resolveCleanupState(function, *block, *operation, *callState->state);
				if (!state.active.empty()) {
					cleanup = internPad(region, state);
				}
			}
			region.callSites.push_back(ExceptionalCallSite {operation, cleanup});
		}
	}
	return region;
}

} // namespace

bool ExceptionCleanupPreparationPass::apply(IRGraph& ir) {
	bool changed = false;
	for (auto* function : ir.getFunctionOperations()) {
		if (function == nullptr) {
			continue;
		}
		auto region = prepareFunction(*function);
		if (region.callSites.empty()) {
			if (function->hasExceptionRegion()) {
				function->clearExceptionRegion();
				changed = true;
			}
			continue;
		}
		if (!function->hasExceptionRegion() || function->getExceptionRegion() != region) {
			function->setExceptionRegion(std::move(region));
			changed = true;
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir
