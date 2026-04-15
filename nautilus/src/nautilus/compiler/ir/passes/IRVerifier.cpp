
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <unordered_set>

namespace nautilus::compiler::ir {

namespace {

std::string blockIdToString(BlockIdentifier id) {
	return std::to_string(id.getId());
}

void addError(VerificationResult& r, const FunctionOperation* fn, const BasicBlock* block, std::string message) {
	VerificationError err;
	err.function = fn ? fn->getName() : "";
	err.block = block ? blockIdToString(block->getIdentifier()) : "";
	err.message = std::move(message);
	r.errors.push_back(std::move(err));
}

void verifyFunction(VerificationResult& r, const FunctionOperation& fn) {
	const auto& blocks = fn.getBasicBlocks();
	if (blocks.empty()) {
		addError(r, &fn, nullptr, "function has no basic blocks");
		return;
	}

	// Unique block identifiers and a fast "is owned by this function" lookup.
	std::unordered_set<uint32_t> seenIds;
	std::unordered_set<const BasicBlock*> ownedBlocks;
	ownedBlocks.reserve(blocks.size());
	for (const auto* block : blocks) {
		if (block == nullptr) {
			addError(r, &fn, nullptr, "function contains a null basic block pointer");
			continue;
		}
		const uint32_t id = block->getIdentifier().getId();
		if (!seenIds.insert(id).second) {
			addError(r, &fn, block, fmt::format("duplicate block identifier {}", id));
		}
		ownedBlocks.insert(block);
	}

	for (const auto* block : blocks) {
		if (block == nullptr) {
			continue;
		}
		const auto& ops = block->getOperations();
		if (ops.empty()) {
			addError(r, &fn, block, "block is empty (has no terminator)");
			continue;
		}
		// Last op must be a terminator; no earlier op may be one.
		for (size_t i = 0; i + 1 < ops.size(); ++i) {
			auto* op = ops[i];
			if (op == nullptr) {
				addError(r, &fn, block, fmt::format("null operation at index {}", i));
				continue;
			}
			if (isTerminatorOp(op->getOperationType())) {
				addError(r, &fn, block,
				         fmt::format("terminator op at index {} but block length is {}", i, ops.size()));
			}
		}
		auto* terminator = ops.back();
		if (terminator == nullptr) {
			addError(r, &fn, block, "block's last operation is null");
			continue;
		}
		if (!isTerminatorOp(terminator->getOperationType())) {
			addError(r, &fn, block, "block's last operation is not a terminator");
			continue;
		}

		// Every outgoing invocation's target must be non-null and owned by
		// this function. And the target must list this block as a predecessor.
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			const auto* target = inv->getBlock();
			if (target == nullptr) {
				addError(r, &fn, block, "terminator invocation targets null block");
				continue;
			}
			if (!ownedBlocks.contains(target)) {
				addError(r, &fn, block,
				         fmt::format("terminator invocation targets block {} which is not owned by function {}",
				                     target->getIdentifier().getId(), fn.getName()));
				continue;
			}
			const auto& preds = target->getPredecessors();
			if (std::find(preds.begin(), preds.end(), block) == preds.end()) {
				addError(r, &fn, block,
				         fmt::format("target block {} does not list this block in its predecessors",
				                     target->getIdentifier().getId()));
			}
		}
	}
}

} // namespace

VerificationResult IRVerifier::verify(const IRGraph& ir) {
	VerificationResult result;
	for (const auto* fn : ir.getFunctionOperations()) {
		if (fn == nullptr) {
			VerificationError err;
			err.message = "ir graph contains a null function pointer";
			result.errors.push_back(std::move(err));
			continue;
		}
		verifyFunction(result, *fn);
	}
	return result;
}

std::string VerificationResult::toString() const {
	if (errors.empty()) {
		return "ok";
	}
	std::string out;
	out.reserve(64 * errors.size());
	for (const auto& err : errors) {
		out += fmt::format("[{}:{}] {}\n", err.function, err.block, err.message);
	}
	return out;
}

} // namespace nautilus::compiler::ir
