
#include "nautilus/compiler/ir/passes/LocalCSEPass.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include <cstdint>
#include <unordered_map>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

using OpType = Operation::OperationType;

/// A CSE candidate: a pure operation that is not a constant. Constants are pure
/// and structurally trivial to number, but deduping them is deliberately
/// excluded in v1: merging two equal constants can make a single constant feed
/// both a store/edge and another use, a sharing pattern that the direct-
/// lowering interpreter backends (`bc`/`tbc`) mis-lower (their constant-image /
/// edge-copy machinery assumes trace IR's one-constant-per-use shape). Constant
/// duplication is already handled by constant folding + DCE, so the loss is
/// small; deduping the *computed* redundancy (address arithmetic, compares,
/// casts, ...) is where the payoff and the safety both are.
bool isCseCandidate(const Operation* op) {
	const auto type = op->getOperationType();
	return isPureOp(type) && !isConstantOp(type);
}

/// Commutative operations may match regardless of operand order; their key
/// normalizes the two operands into a canonical (sorted) order. `eq`/`ne`
/// compares are commutative; the ordered compares (`lt`/`le`/`gt`/`ge`) are
/// not (they carry a direction), so they are keyed with operand order intact.
bool isCommutative(const Operation* op) {
	switch (op->getOperationType()) {
	case OpType::AddOp:
	case OpType::MulOp:
	case OpType::AndOp:
	case OpType::OrOp:
	case OpType::BinaryComp:
		return true;
	case OpType::CompareOp: {
		const auto cmp = cast<CompareOperation>(op)->getComparator();
		return cmp == CompareOperation::EQ || cmp == CompareOperation::NE;
	}
	default:
		return false;
	}
}

/// The op-specific discriminator two structurally identical operations must
/// also agree on to be the same value: a compare's direction or a
/// shift/bitwise op's kind. (A cast's target is its result stamp, already part
/// of the key.) Structural equality of opType + stamp + operands is otherwise
/// enough.
int64_t payloadOf(const Operation* op) {
	switch (op->getOperationType()) {
	case OpType::CompareOp:
		return cast<CompareOperation>(op)->getComparator();
	case OpType::BinaryComp:
		return cast<BinaryCompOperation>(op)->getType();
	case OpType::ShiftOp:
		return cast<ShiftOperation>(op)->getType();
	default:
		return 0;
	}
}

struct ValueKey {
	uint8_t opType;
	uint8_t stamp;
	int64_t payload;
	std::vector<Operation*> operands;

	bool operator==(const ValueKey& other) const = default;
};

struct ValueKeyHash {
	size_t operator()(const ValueKey& k) const {
		size_t h = std::hash<uint32_t> {}((static_cast<uint32_t>(k.opType) << 8) | k.stamp);
		auto mix = [&h](size_t v) {
			h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2);
		};
		mix(std::hash<int64_t> {}(k.payload));
		for (auto* operand : k.operands) {
			mix(std::hash<const void*> {}(operand));
		}
		return h;
	}
};

ValueKey makeKey(Operation* op) {
	ValueKey key;
	key.opType = static_cast<uint8_t>(op->getOperationType());
	key.stamp = static_cast<uint8_t>(op->getStamp());
	key.payload = payloadOf(op);
	const auto inputs = op->getInputs();
	key.operands.assign(inputs.begin(), inputs.end());
	if (isCommutative(op) && key.operands.size() == 2) {
		if (key.operands[1] < key.operands[0]) {
			std::swap(key.operands[0], key.operands[1]);
		}
	}
	return key;
}

bool applyToFunction(FunctionOperation& fn, common::Arena& arena) {
	FunctionRewriter rewriter(fn, arena);
	bool changed = false;

	for (auto* block : fn.getBasicBlocks()) {
		std::unordered_map<ValueKey, Operation*, ValueKeyHash> table;
		// Snapshot: erasing a duplicate mutates the block's operation list.
		const auto ops = block->getOperations();
		for (auto* op : ops) {
			// An earlier duplicate's erase may already have removed this op (it
			// is then untracked); and only computed pure ops are numbered.
			if (rewriter.definingBlock(op) == nullptr || !isCseCandidate(op)) {
				continue;
			}
			auto [it, inserted] = table.try_emplace(makeKey(op), op);
			if (inserted) {
				continue;
			}
			Operation* representative = it->second;
			if (rewriter.useCount(op) > 0) {
				rewriter.replaceAllUses(op, representative);
			}
			// Erase *only* the duplicate (a plain non-cascading `erase`, not
			// `eraseIfDead`). Cascading would be unsafe: if the duplicate was
			// the sole remaining user of one of its operands, and that operand
			// is itself a representative in `table`, the cascade would free it
			// and leave a dangling entry a later hit could rewire to. The
			// duplicate's now-dead operands are left for DCE to sweep.
			rewriter.erase(op);
			changed = true;
		}
	}
	return changed;
}

} // namespace

bool LocalCSEPass::apply(IRGraph& ir) {
	common::Arena& arena = ir.getArena();
	bool changed = false;
	for (auto* fn : ir.getFunctionOperations()) {
		if (fn != nullptr) {
			changed |= applyToFunction(*fn, arena);
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir
