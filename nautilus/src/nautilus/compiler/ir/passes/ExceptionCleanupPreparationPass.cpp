#include "nautilus/compiler/ir/passes/ExceptionCleanupPreparationPass.hpp"
#include "nautilus/compiler/ir/ExceptionRegion.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <deque>
#include <optional>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::ir {

namespace {

using CleanupState = std::vector<AllocaIndex>;

[[noreturn]] void fail(const FunctionOperation& function, const BasicBlock& block, const Operation& operation,
                       const std::string& reason) {
	throw RuntimeException("Invalid exception-cleanup state in function '" + function.getName() + "', block " +
	                       std::to_string(block.getIdentifier().getId()) + ", operation " +
	                       operation.getIdentifier().toString() + ": " + reason);
}

std::optional<CleanupEffect> getCleanupEffect(const Operation& operation) {
	switch (operation.getOperationType()) {
	case Operation::OperationType::AllocaOp:
		return static_cast<const AllocaOperation&>(operation).getCleanupEffect();
	case Operation::OperationType::ProxyCallOp:
		return static_cast<const ProxyCallOperation&>(operation).getCleanupEffect();
	case Operation::OperationType::IndirectCallOp:
		return static_cast<const IndirectCallOperation&>(operation).getCleanupEffect();
	default:
		return std::nullopt;
	}
}

bool mayThrow(const Operation& operation) {
	switch (operation.getOperationType()) {
	case Operation::OperationType::ProxyCallOp:
		return !static_cast<const ProxyCallOperation&>(operation).getFunctionAttributes().noUnwind;
	case Operation::OperationType::IndirectCallOp:
		return !static_cast<const IndirectCallOperation&>(operation).getFunctionAttributes().noUnwind;
	default:
		return false;
	}
}

void validateAlloca(const FunctionOperation& function, const BasicBlock& block, const Operation& operation,
                    AllocaIndex alloca) {
	const auto& specs = function.getAllocaSpecs();
	if (alloca >= specs.size()) {
		fail(function, block, operation,
		     "cleanup effect references alloca " + std::to_string(alloca) + " but the function has only " +
		         std::to_string(specs.size()));
	}
	if (!specs[alloca].destructor.has_value()) {
		fail(function, block, operation,
		     "cleanup effect references alloca " + std::to_string(alloca) + " without a destructor");
	}
}

void deactivate(const FunctionOperation& function, const BasicBlock& block, const Operation& operation,
                CleanupState& state, AllocaIndex alloca) {
	validateAlloca(function, block, operation, alloca);
	auto position = std::find(state.begin(), state.end(), alloca);
	if (position == state.end()) {
		fail(function, block, operation, "deactivates inactive alloca " + std::to_string(alloca));
	}
	state.erase(position);
}

void activate(const FunctionOperation& function, const BasicBlock& block, const Operation& operation,
              CleanupState& state, AllocaIndex alloca) {
	validateAlloca(function, block, operation, alloca);
	if (std::find(state.begin(), state.end(), alloca) != state.end()) {
		fail(function, block, operation, "activates already-active alloca " + std::to_string(alloca));
	}
	state.push_back(alloca);
}

CleanupPadId internPad(FunctionExceptionRegion& region, const CleanupState& state) {
	auto existing = std::find_if(region.pads.begin(), region.pads.end(),
	                             [&](const CleanupPad& pad) { return pad.active == state; });
	if (existing != region.pads.end()) {
		return existing->id;
	}
	auto id = static_cast<CleanupPadId>(region.pads.size());
	region.pads.push_back(CleanupPad {id, state});
	return id;
}

FunctionExceptionRegion prepareFunction(const FunctionOperation& function) {
	FunctionExceptionRegion region;
	std::unordered_map<const BasicBlock*, CleanupState> incoming;
	std::unordered_set<const BasicBlock*> processed;
	std::deque<const BasicBlock*> worklist;
	incoming.emplace(function.getEntryBlock(), CleanupState {});
	worklist.push_back(function.getEntryBlock());

	while (!worklist.empty()) {
		const auto* block = worklist.front();
		worklist.pop_front();
		if (!processed.insert(block).second) {
			continue;
		}
		auto state = incoming.at(block);
		for (const auto* operation : block->getOperations()) {
			auto effect = getCleanupEffect(*operation);
			if (effect.has_value() && effect->kind == CleanupEffectKind::DeactivateBeforeCall) {
				deactivate(function, *block, *operation, state, effect->alloca);
			}

			if (mayThrow(*operation)) {
				std::optional<CleanupPadId> cleanup;
				if (!state.empty()) {
					cleanup = internPad(region, state);
				}
				region.callSites.push_back(ExceptionalCallSite {operation, cleanup});
			}

			if (effect.has_value() && effect->kind == CleanupEffectKind::ActivateAfterSuccess) {
				activate(function, *block, *operation, state, effect->alloca);
			}

			if (operation->getOperationType() == Operation::OperationType::ReturnOp && !state.empty()) {
				fail(function, *block, *operation,
				     "returns with active alloca " + std::to_string(state.back()) + " still requiring destruction");
			}
		}

		for (const auto* successor : const_cast<BasicBlock*>(block)->getSuccessors()) {
			auto [position, inserted] = incoming.emplace(successor, state);
			if (!inserted && position->second != state) {
				const auto* terminator = block->getOperations().back();
				fail(function, *block, *terminator,
				     "successor block " + std::to_string(successor->getIdentifier().getId()) +
				         " receives unequal active-destructor states");
			}
			if (!processed.contains(successor)) {
				worklist.push_back(successor);
			}
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
