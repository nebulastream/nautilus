
#include "nautilus/compiler/ir/passes/IRVerifier.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/passes/Dominators.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <unordered_map>
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

/// Operation kinds that are never referenced as an SSA value and therefore
/// always carry the sentinel identifier `OperationIdentifier{0}` by
/// construction (their constructors don't even accept an id): the three
/// terminator kinds, `StoreOperation` (no result), and `FunctionOperation`
/// (the function container itself, addressed by name elsewhere, never by
/// identifier). V2 excludes them so that e.g. every block's terminator
/// sharing id 0 is not flagged as a collision.
bool identifierIsSignificant(Operation::OperationType opType) {
	using OT = Operation::OperationType;
	switch (opType) {
	case OT::BranchOp:
	case OT::IfOp:
	case OT::ReturnOp:
	case OT::StoreOp:
	case OT::FunctionOp:
		return false;
	default:
		return true;
	}
}

/// V2: operation identifiers (block operations and block arguments) are
/// unique **within a block**, excluding the identifier-insignificant kinds
/// documented on `identifierIsSignificant`.
///
/// Scope note: the milestone doc specifies function-wide uniqueness, but the
/// tracer's value numbering (`TraceToIRConversionPhase::createValueIdentifier`)
/// assigns identifiers from a per-value-slot space that is legitimately
/// reused across unrelated blocks -- calibrating this check against the full
/// test corpus (V7) showed every real traced function reusing dozens of
/// identifiers across blocks by construction, with no correctness impact.
/// The bug class this check exists for (#321) is a *write-time* hazard: a
/// pass minting a fresh id that collides with something already live in the
/// same block; `FunctionRewriter::freshId()` (M5, IR-0b) is what actually
/// forecloses it. Scoping the read-time check to one block at a time still
/// catches that shape (and every hand-built-fixture regression test below)
/// without flagging the pervasive, benign cross-block reuse in real IR.
void checkUniqueIdentifiers(VerificationResult& r, const FunctionOperation& fn) {
	for (const auto* block : fn.getBasicBlocks()) {
		if (block == nullptr) {
			continue;
		}
		std::unordered_map<uint32_t, const Operation*> seen;
		auto check = [&](const Operation* op) {
			if (op == nullptr || !identifierIsSignificant(op->getOperationType())) {
				return;
			}
			const uint32_t id = op->getIdentifier().getId();
			auto [it, inserted] = seen.try_emplace(id, op);
			if (!inserted && it->second != op) {
				addError(r, &fn, block,
				         fmt::format("duplicate operation identifier {}: also used by {} {}", id,
				                     it->second->getIdentifier().toString(), op->getIdentifier().toString()));
			}
		};
		for (const auto* arg : block->getArguments()) {
			check(arg);
		}
		for (const auto* op : block->getOperations()) {
			check(op);
		}
	}
}

/// V1: every `BasicBlockInvocation`'s argument count equals its target
/// block's argument count.
void checkInvocationArity(VerificationResult& r, const FunctionOperation& fn) {
	for (auto* block : fn.getBasicBlocks()) {
		if (block == nullptr || block->getOperations().empty()) {
			continue;
		}
		auto* terminator = block->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			const auto* target = inv->getBlock();
			if (target == nullptr) {
				continue; // reported separately (null-target check).
			}
			const auto argCount = inv->getArguments().size();
			const auto paramCount = target->getArguments().size();
			if (argCount != paramCount) {
				addError(r, &fn, block,
				         fmt::format("invocation to block {} passes {} argument(s), but the block expects {}",
				                     target->getIdentifier().getId(), argCount, paramCount));
			}
		}
	}
}

/// Position of a value definition within its function: the defining block
/// plus an index used to order same-block definitions against same-block
/// uses. Block arguments sort before every operation in their block (index
/// -1); a terminator's invocation arguments are uses positioned at the
/// terminator's own index (the last index in the block).
struct DefPosition {
	const BasicBlock* block;
	int index;
};

std::unordered_map<const Operation*, DefPosition> computeDefPositions(const FunctionOperation& fn) {
	std::unordered_map<const Operation*, DefPosition> positions;
	for (const auto* block : fn.getBasicBlocks()) {
		if (block == nullptr) {
			continue;
		}
		for (const auto* arg : block->getArguments()) {
			positions[arg] = {block, -1};
		}
		const auto& ops = block->getOperations();
		for (size_t i = 0; i < ops.size(); ++i) {
			if (ops[i] != nullptr) {
				positions[ops[i]] = {block, static_cast<int>(i)};
			}
		}
	}
	return positions;
}

/// V3: for every operand edge in a reachable block, the definition
/// dominates the use. Same-block edges are ordered by `DefPosition::index`;
/// cross-block edges go through `Dominators::dominates`. Operands pointing
/// outside the function are skipped here (reported separately as stale
/// pointers).
void checkSSADominance(VerificationResult& r, const FunctionOperation& fn) {
	const auto positions = computeDefPositions(fn);
	const Dominators dom(fn);

	const auto checkEdge = [&](const BasicBlock* useBlock, int useIndex, const Operation* user, const Operation* def,
	                           const char* what) {
		if (def == nullptr) {
			return;
		}
		auto it = positions.find(def);
		if (it == positions.end()) {
			return; // stale pointer, reported elsewhere.
		}
		const auto& defPos = it->second;
		const bool ok = defPos.block == useBlock ? defPos.index < useIndex : dom.dominates(defPos.block, useBlock);
		if (!ok) {
			addError(r, &fn, useBlock,
			         fmt::format("{} {} uses {} defined in block {}, which does not dominate its use in block {}", what,
			                     user->getIdentifier().toString(), def->getIdentifier().toString(),
			                     defPos.block->getIdentifier().getId(), useBlock->getIdentifier().getId()));
		}
	};

	for (const auto* block : fn.getBasicBlocks()) {
		if (block == nullptr || !dom.isReachable(block)) {
			continue;
		}
		const auto& ops = block->getOperations();
		for (size_t i = 0; i < ops.size(); ++i) {
			const auto* op = ops[i];
			if (op == nullptr) {
				continue;
			}
			for (const auto* input : op->getInputs()) {
				checkEdge(block, static_cast<int>(i), op, input, "operation");
			}
			for (auto* inv : getSuccessorInvocations(*const_cast<Operation*>(op))) {
				for (const auto* arg : inv->getArguments()) {
					checkEdge(block, static_cast<int>(i), inv, arg, "terminator invocation");
				}
			}
		}
	}
}

/// V4: predecessor lists match incoming edges as a **multiset**: for each
/// block, the number of times block `p` appears in `getPredecessors()` must
/// equal the number of invocations in `p`'s terminator targeting it.
/// Replaces a one-directional membership check, which missed both stale
/// extra entries and a target listing a predecessor fewer times than it
/// actually targets it (the shared-target-if shape).
void checkPredecessorMultiset(VerificationResult& r, const FunctionOperation& fn) {
	std::unordered_map<const BasicBlock*, std::unordered_map<const BasicBlock*, int>> actualEdges;
	for (auto* block : fn.getBasicBlocks()) {
		if (block == nullptr || block->getOperations().empty()) {
			continue;
		}
		auto* terminator = block->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			const auto* target = inv->getBlock();
			if (target != nullptr) {
				actualEdges[target][block]++;
			}
		}
	}

	for (const auto* block : fn.getBasicBlocks()) {
		if (block == nullptr) {
			continue;
		}
		std::unordered_map<const BasicBlock*, int> listedCounts;
		for (const auto* pred : block->getPredecessors()) {
			listedCounts[pred]++;
		}
		const auto& actual = actualEdges[block];

		std::unordered_set<const BasicBlock*> keys;
		for (const auto& [k, v] : listedCounts) {
			keys.insert(k);
		}
		for (const auto& [k, v] : actual) {
			keys.insert(k);
		}
		for (const auto* pred : keys) {
			const int listed = listedCounts.contains(pred) ? listedCounts.at(pred) : 0;
			const int expected = actual.contains(pred) ? actual.at(pred) : 0;
			if (listed != expected) {
				addError(r, &fn, block,
				         fmt::format("predecessor {} listed {} time(s) but targets this block via {} edge(s)",
				                     pred->getIdentifier().getId(), listed, expected));
			}
		}
	}
}

/// V5: the entry block has no predecessors, and -- when the function
/// declares a signature -- its argument count and stamps match
/// `FunctionOperation::getInputArgs()`.
///
/// Scope note: `TraceToIRConversionPhase` always leaves `inputArgs` empty
/// (see the fallback in `IRGraph.cpp`'s `FunctionOperation` formatter: "the
/// trace-to-IR conversion leaves inputArgs/inputArgNames empty; the
/// parameters live on the entry block"); the entry block's own arguments
/// are the sole source of truth for real IR. The count/stamp comparison
/// below is therefore only meaningful -- and only enforced -- when a caller
/// (typically a hand-built test fixture) opts into declaring `inputArgs`.
void checkEntryBlockAbi(VerificationResult& r, const FunctionOperation& fn) {
	auto* entry = fn.getEntryBlock();
	if (entry == nullptr) {
		return; // reported elsewhere ("function has no basic blocks").
	}
	if (!entry->getPredecessors().empty()) {
		addError(r, &fn, entry,
		         fmt::format("entry block has {} predecessor(s); the entry block must have none",
		                     entry->getPredecessors().size()));
	}
	const auto& inputArgs = fn.getInputArgs();
	if (inputArgs.empty()) {
		return;
	}
	const auto& entryArgs = entry->getArguments();
	if (entryArgs.size() != inputArgs.size()) {
		addError(r, &fn, entry,
		         fmt::format("entry block has {} argument(s), but the function signature declares {}", entryArgs.size(),
		                     inputArgs.size()));
		return;
	}
	for (size_t i = 0; i < entryArgs.size(); ++i) {
		if (entryArgs[i] == nullptr) {
			continue;
		}
		if (entryArgs[i]->getStamp() != inputArgs[i]) {
			addError(r, &fn, entry,
			         fmt::format("entry block argument {} has stamp {}, but the function signature declares {}", i,
			                     static_cast<int>(entryArgs[i]->getStamp()), static_cast<int>(inputArgs[i])));
		}
	}
}

/// V6: stamp agreement on block edges: each invocation argument's stamp
/// equals the corresponding block argument's stamp. Arity mismatches are
/// V1's concern; here we only compare slots present on both sides.
void checkEdgeStampAgreement(VerificationResult& r, const FunctionOperation& fn) {
	for (auto* block : fn.getBasicBlocks()) {
		if (block == nullptr || block->getOperations().empty()) {
			continue;
		}
		auto* terminator = block->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			const auto* target = inv->getBlock();
			if (target == nullptr) {
				continue;
			}
			const auto args = inv->getArguments();
			const auto& params = target->getArguments();
			const size_t n = std::min(args.size(), params.size());
			for (size_t i = 0; i < n; ++i) {
				if (args[i] == nullptr || params[i] == nullptr) {
					continue;
				}
				if (args[i]->getStamp() != params[i]->getStamp()) {
					addError(r, &fn, block,
					         fmt::format("invocation to block {} passes argument {} with stamp {}, but the "
					                     "target's parameter stamp is {}",
					                     target->getIdentifier().getId(), i, static_cast<int>(args[i]->getStamp()),
					                     static_cast<int>(params[i]->getStamp())));
				}
			}
		}
	}
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

	// Every operation defined in this function: block operations plus block
	// arguments. Input edges are checked against this set below — a pointer
	// to anything else is stale, e.g. an operation a pass replaced and
	// removed without rewiring its consumers (issue #327).
	std::unordered_set<const Operation*> definedOps;
	for (const auto* block : blocks) {
		if (block == nullptr) {
			continue;
		}
		for (const auto* op : block->getOperations()) {
			definedOps.insert(op);
		}
		for (const auto* arg : block->getArguments()) {
			definedOps.insert(arg);
		}
	}
	const auto checkInputs = [&](const BasicBlock* block, const Operation* op, const char* what) {
		for (const auto* input : op->getInputs()) {
			if (input == nullptr) {
				addError(r, &fn, block, fmt::format("{} {} has a null input", what, op->getIdentifier().toString()));
				continue;
			}
			if (!definedOps.contains(input)) {
				addError(r, &fn, block,
				         fmt::format("{} {} input {} points at an operation not defined in function {}", what,
				                     op->getIdentifier().toString(), input->getIdentifier().toString(), fn.getName()));
			}
		}
	};

	bool structurallySound = true;
	for (const auto* block : blocks) {
		if (block == nullptr) {
			continue;
		}
		const auto& ops = block->getOperations();
		if (ops.empty()) {
			addError(r, &fn, block, "block is empty (has no terminator)");
			structurallySound = false;
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
				addError(r, &fn, block, fmt::format("terminator op at index {} but block length is {}", i, ops.size()));
			}
			checkInputs(block, op, "operation");
		}
		auto* terminator = ops.back();
		if (terminator == nullptr) {
			addError(r, &fn, block, "block's last operation is null");
			structurallySound = false;
			continue;
		}
		if (!isTerminatorOp(terminator->getOperationType())) {
			addError(r, &fn, block, "block's last operation is not a terminator");
			structurallySound = false;
			continue;
		}
		checkInputs(block, terminator, "terminator");

		// Every outgoing invocation's target must be non-null and owned by
		// this function.
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			checkInputs(block, inv, "terminator invocation");
			const auto* target = inv->getBlock();
			if (target == nullptr) {
				addError(r, &fn, block, "terminator invocation targets null block");
				continue;
			}
			if (!ownedBlocks.contains(target)) {
				addError(r, &fn, block,
				         fmt::format("terminator invocation targets block {} which is not owned by function {}",
				                     target->getIdentifier().getId(), fn.getName()));
			}
		}
	}

	if (!structurallySound) {
		// V1/V3/V4/V5/V6 all assume every block ends in a terminator; skip
		// them rather than compounding a structural error with noise.
		return;
	}

	checkInvocationArity(r, fn);     // V1
	checkUniqueIdentifiers(r, fn);   // V2
	checkSSADominance(r, fn);        // V3
	checkPredecessorMultiset(r, fn); // V4
	checkEntryBlockAbi(r, fn);       // V5
	checkEdgeStampAgreement(r, fn);  // V6
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
