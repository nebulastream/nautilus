
#include "nautilus/compiler/backends/asmjit/A64PostRAPeepholePass.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include <asmjit/a64.h>

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;
using namespace ::asmjit::a64;

namespace {

bool isRemovableSelfMove(const InstNode* inst) noexcept {
	if (inst->opCount() != 2) {
		return false;
	}
	const Operand& dst = inst->op(0);
	const Operand& src = inst->op(1);
	if (!dst.isReg() || !src.isReg() || !dst.isPhysReg() || !src.isPhysReg()) {
		return false;
	}
	if (dst.id() != src.id()) {
		return false;
	}

	const InstId id = inst->id();
	switch (id) {
	case Inst::kIdMov: {
		// Only delete 64-bit GPR self-moves. W-register self-moves are a
		// zero-extension idiom and must not be touched.
		const auto& dstReg = dst.as<Gp>();
		const auto& srcReg = src.as<Gp>();
		if (dstReg.isGpX() && srcReg.isGpX()) {
			return true;
		}
		return false;
	}
	// Intentionally no vector cases: fmov Sd,Sd / fmov Dd,Dd zero the
	// upper bits of the V register and are therefore NOT pure no-ops.
	// mov V.16B (kIdMov_v with full 128-bit element) would be safe, but
	// Nautilus emits fmov (not mov) for Vec copies, so this case never
	// arises in practice.
	default:
		return false;
	}
}

/// True when @p label is bound at the position directly after @p node, i.e.
/// among the run of label nodes that follows it before the next instruction.
/// Only label nodes are skipped: data (constant pools), alignment, or the
/// sentinel ending a function all sit between @p node and the label in the
/// emitted code, so a branch across them is not a fallthrough.
bool isBoundDirectlyAfter(const BaseNode* node, uint32_t labelId) noexcept {
	for (const BaseNode* next = node->next(); next != nullptr && next->isLabel(); next = next->next()) {
		if (next->as<LabelNode>()->labelId() == labelId) {
			return true;
		}
	}
	return false;
}

/// The label operand @p index of @p inst, or nullptr when it is not a label.
const Label* labelOperand(const InstNode* inst, uint32_t index) noexcept {
	if (inst->opCount() <= index || !inst->op(index).isLabel()) {
		return nullptr;
	}
	return &inst->op(index).as<Label>();
}

/// The target of an unconditional `b label`, or nullptr.
const Label* unconditionalTarget(const InstNode* inst) noexcept {
	if (inst->id() != Inst::kIdB || inst->opCount() != 1) {
		return nullptr;
	}
	return labelOperand(inst, 0);
}

/// Rules 2 and 3 (branch simplification) over the whole node list, once.
/// Returns the number of branches removed.
int64_t simplifyBranches(BaseBuilder* cb) noexcept {
	int64_t removed = 0;
	BaseNode* node = cb->firstNode();
	while (node != nullptr) {
		BaseNode* next = node->next();
		if (!node->isInst()) {
			node = next;
			continue;
		}
		auto* inst = node->as<InstNode>();

		// Rule 2: a branch to the label bound right after it is a no-op. The
		// lowering emits one at nearly every block boundary (each
		// block-argument edge ends in an explicit `b`, and the target is
		// usually emitted next).
		if (const Label* target = unconditionalTarget(inst)) {
			if (isBoundDirectlyAfter(node, target->id())) {
				cb->removeNode(node);
				++removed;
			}
			node = next;
			continue;
		}

		// Rule 3: `b.cond A; b B; A:` -> `b.!cond B; A:`, and likewise for
		// cbz/cbnz. The `b` is not a branch target itself (it directly follows
		// the conditional branch), so folding it into the inverted branch
		// removes one taken branch from the path that previously went through
		// it.
		const InstId realId = BaseInst::extractRealId(inst->id());
		const arm::CondCode cond = BaseInst::extractARMCondCode(inst->id());
		const bool isCondBranch = realId == Inst::kIdB && cond != arm::CondCode::kAL && cond != arm::CondCode::kNA;
		const bool isCompareBranch = realId == Inst::kIdCbz || realId == Inst::kIdCbnz;
		if ((isCondBranch || isCompareBranch) && next != nullptr && next->isInst()) {
			auto* jump = next->as<InstNode>();
			const uint32_t labelIndex = isCondBranch ? 0 : 1;
			const Label* condTarget = labelOperand(inst, labelIndex);
			const Label* jumpTarget = unconditionalTarget(jump);
			if (condTarget != nullptr && jumpTarget != nullptr && isBoundDirectlyAfter(jump, condTarget->id())) {
				if (isCondBranch) {
					inst->setId(BaseInst::composeARMInstId(Inst::kIdB, arm::negateCond(cond)));
				} else {
					inst->setId(realId == Inst::kIdCbz ? Inst::kIdCbnz : Inst::kIdCbz);
				}
				inst->setOp(labelIndex, *jumpTarget);
				next = jump->next();
				cb->removeNode(jump);
				++removed;
			}
		}
		node = next;
	}
	return removed;
}

} // namespace

Error A64PostRAPeepholePass::run(Zone* /*zone*/, Logger* /*logger*/) {
	BaseBuilder* cb = _cb;
	if (cb == nullptr) {
		return kErrorOk;
	}

	// Removing one branch can make another one a fallthrough (`b L; M: b L;
	// L:`), so iterate to a fixpoint; each round only removes nodes.
	int64_t jumpsRemoved = 0;
	for (int64_t round = simplifyBranches(cb); round != 0; round = simplifyBranches(cb)) {
		jumpsRemoved += round;
	}

	int64_t selfMovesRemoved = 0;
	BaseNode* node = cb->firstNode();
	while (node != nullptr) {
		BaseNode* next = node->next();
		if (node->isInst()) {
			auto* inst = node->as<InstNode>();
			if (isRemovableSelfMove(inst)) {
				cb->removeNode(node);
				++selfMovesRemoved;
			}
		}
		node = next;
	}

	if (statistics_ != nullptr) {
		statistics_->add("asmjit.peephole.selfMovesRemoved", selfMovesRemoved);
		statistics_->add("asmjit.peephole.jumpsRemoved", jumpsRemoved);
	}
	return kErrorOk;
}

} // namespace nautilus::compiler::asmjit
