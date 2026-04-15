
#include "SSAVerifier.hpp"
#include "fmt/core.h"
#include <nautilus/tracing/ExecutionTrace.hpp>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::tracing {

SSAVerificationResult VerifySSA(const ExecutionTrace& trace) {
	SSAVerificationResult result;
	result.valid = true;

	// Build a map from blockId to index for quick lookup
	std::unordered_map<uint16_t, size_t> blockIdToIndex;
	for (size_t i = 0; i < trace.blocks.size(); i++) {
		blockIdToIndex[trace.blocks[i]->blockId] = i;
	}

	for (const auto* blockPtr : trace.blocks) {
		const auto& block = *blockPtr;
		// Collect the set of value refs defined in this block:
		// block arguments + operation results (in order)
		std::unordered_set<uint16_t> definedRefs;
		// Track definitions for duplicate detection
		std::unordered_set<uint16_t> opDefinedRefs;

		// Block arguments are defined at the start of the block
		for (const auto& arg : block.arguments) {
			definedRefs.insert(arg.ref);
		}

		for (size_t opIdx = 0; opIdx < block.operations.size(); opIdx++) {
			const auto& operation = *block.operations[opIdx];

			// Check 1: No ASSIGN operations should remain
			if (operation.op == Op::ASSIGN) {
				result.valid = false;
				result.errors.push_back(fmt::format(
				    "B{}: ASSIGN operation found at index {} (should be eliminated by SSA)", block.blockId, opIdx));
			}

			// Check 2: Every TypedValueRef input must be defined locally
			auto checkRef = [&](const TypedValueRef& ref, const char* context) {
				// Skip void refs (ref 0 is used for void/no-result operations)
				if (ref.ref == 0) {
					return;
				}
				if (!definedRefs.contains(ref.ref)) {
					result.valid = false;
					result.errors.push_back(fmt::format("B{} op {}: {} uses undefined value ref ${}", block.blockId,
					                                    opIdx, context, ref.ref));
				}
			};

			// Check 3: BlockRef argument count matches target block
			auto checkBlockRef = [&](const BlockRef& blockRef) {
				auto it = blockIdToIndex.find(blockRef.block);
				if (it == blockIdToIndex.end()) {
					result.valid = false;
					result.errors.push_back(fmt::format("B{} op {}: references non-existent block B{}", block.blockId,
					                                    opIdx, blockRef.block));
					return;
				}
				const auto& targetBlock = *trace.blocks[it->second];
				if (blockRef.arguments.size() != targetBlock.arguments.size()) {
					result.valid = false;
					result.errors.push_back(
					    fmt::format("B{} op {}: BlockRef to B{} passes {} arguments but target has {}", block.blockId,
					                opIdx, blockRef.block, blockRef.arguments.size(), targetBlock.arguments.size()));
				}
				// Check each argument ref is defined in the current block
				for (const auto& arg : blockRef.arguments) {
					checkRef(arg, "BlockRef argument");
				}
			};

			for (const auto& input : operation.input) {
				if (const auto* valueRef = std::get_if<TypedValueRef>(&input)) {
					checkRef(*valueRef, "input");
				} else if (const auto* blockRef = std::get_if<BlockRef>(&input)) {
					checkBlockRef(*blockRef);
				} else if (const auto* fcall = std::get_if<FunctionCall>(&input)) {
					for (const auto& arg : fcall->arguments) {
						checkRef(arg, "function call argument");
					}
				}
				// None, ConstantLiteral, BranchProbability need no ref checks
			}

			// After processing the operation, add its result to defined refs
			// (only if it produces a value, i.e. ref != 0)
			if (operation.resultRef.ref != 0) {
				// Check 5: No duplicate value definitions
				if (opDefinedRefs.contains(operation.resultRef.ref)) {
					result.valid = false;
					result.errors.push_back(fmt::format("B{} op {}: duplicate definition of ${}", block.blockId, opIdx,
					                                    operation.resultRef.ref));
				}
				opDefinedRefs.insert(operation.resultRef.ref);
				definedRefs.insert(operation.resultRef.ref);
			}
		}

		// Check 4: Every non-empty block must end with a terminator
		if (!block.operations.empty()) {
			auto lastOp = block.operations.back()->op;
			if (lastOp != Op::JMP && lastOp != Op::CMP && lastOp != Op::RETURN) {
				result.valid = false;
				result.errors.push_back(
				    fmt::format("B{}: block does not end with a terminator (JMP, CMP, or RETURN), last op is {}",
				                block.blockId, toString(lastOp)));
			}
		}
	}

	return result;
}

} // namespace nautilus::tracing
