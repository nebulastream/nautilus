
#include "nautilus/compiler/backends/amsjit/X64PostRAPeepholePass.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include <asmjit/x86.h>

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;
using namespace ::asmjit::x86;

namespace {

/// Result of a bounded forward scan for EFLAGS liveness.
enum class FlagScan {
	// A flag-defining instruction was reached before any flag consumer, so
	// our inserted flag-clobbering instruction cannot affect observable
	// behavior. Rewriting is safe.
	Clobbered,
	// A flag-consuming instruction was reached before any flag definer. Our
	// rewrite would corrupt that consumer. Rewriting is unsafe.
	Live,
	// The scan ran out of budget or hit the end of the node list without
	// reaching either a definite clobber or a definite read. Conservative
	// fallback: treat as live.
	Unknown,
};

/// Instructions whose only reasonable use is to consume EFLAGS.
/// If we hit one of these before any instruction that clobbers EFLAGS,
/// we must not insert a new flag-clobbering instruction ahead of it.
bool isFlagReader(InstId id) noexcept {
	switch (id) {
	// Conditional jumps (kIdJmp is unconditional and is NOT a flag reader).
	// Condition aliases (jz, jc, ...) share the IDs of the cases below.
	case Inst::kIdJa:
	case Inst::kIdJae:
	case Inst::kIdJb:
	case Inst::kIdJbe:
	case Inst::kIdJe:
	case Inst::kIdJg:
	case Inst::kIdJge:
	case Inst::kIdJl:
	case Inst::kIdJle:
	case Inst::kIdJne:
	case Inst::kIdJno:
	case Inst::kIdJnp:
	case Inst::kIdJns:
	case Inst::kIdJo:
	case Inst::kIdJp:
	case Inst::kIdJs:
	// Conditional sets and moves.
	case Inst::kIdSeta:
	case Inst::kIdSetae:
	case Inst::kIdSetb:
	case Inst::kIdSetbe:
	case Inst::kIdSete:
	case Inst::kIdSetg:
	case Inst::kIdSetge:
	case Inst::kIdSetl:
	case Inst::kIdSetle:
	case Inst::kIdSetne:
	case Inst::kIdSetno:
	case Inst::kIdSetnp:
	case Inst::kIdSetns:
	case Inst::kIdSeto:
	case Inst::kIdSetp:
	case Inst::kIdSets:
	case Inst::kIdCmova:
	case Inst::kIdCmovae:
	case Inst::kIdCmovb:
	case Inst::kIdCmovbe:
	case Inst::kIdCmove:
	case Inst::kIdCmovg:
	case Inst::kIdCmovge:
	case Inst::kIdCmovl:
	case Inst::kIdCmovle:
	case Inst::kIdCmovne:
	case Inst::kIdCmovno:
	case Inst::kIdCmovnp:
	case Inst::kIdCmovns:
	case Inst::kIdCmovo:
	case Inst::kIdCmovp:
	case Inst::kIdCmovs:
	// Flag-consuming arithmetic.
	case Inst::kIdAdc:
	case Inst::kIdSbb:
		return true;
	default:
		return false;
	}
}

/// Instructions that fully overwrite the EFLAGS bits relevant to flag
/// consumers (ZF/SF/CF/OF/PF/AF) so that any prior flag state is
/// irretrievably lost before any subsequent flag consumer could read it.
/// This is a conservative subset: we only list instructions where the
/// flag-overwrite behavior is unambiguous and Nautilus actually emits
/// them in practice.
///
/// Note: calls and returns are treated as flag clobbers because callee
/// code is free to clobber EFLAGS under the SysV/Win64 ABIs; no downstream
/// consumer in reasonable code depends on flags set before a call.
bool isFlagClobberer(InstId id) noexcept {
	switch (id) {
	case Inst::kIdAdd:
	case Inst::kIdSub:
	case Inst::kIdCmp:
	case Inst::kIdTest:
	case Inst::kIdAnd:
	case Inst::kIdOr:
	case Inst::kIdXor:
	case Inst::kIdShl:
	case Inst::kIdShr:
	case Inst::kIdSar:
	case Inst::kIdNeg:
	case Inst::kIdInc:
	case Inst::kIdDec:
	case Inst::kIdMul:
	case Inst::kIdImul:
	case Inst::kIdDiv:
	case Inst::kIdIdiv:
	case Inst::kIdUcomiss:
	case Inst::kIdUcomisd:
	case Inst::kIdComiss:
	case Inst::kIdComisd:
	case Inst::kIdCall:
	case Inst::kIdRet:
		return true;
	default:
		return false;
	}
}

/// Bounded forward scan starting at `from->next()` for EFLAGS liveness.
FlagScan flagScan(BaseNode* from, std::size_t maxSteps = 16) noexcept {
	BaseNode* node = from->next();
	for (std::size_t step = 0; step < maxSteps && node != nullptr; ++step, node = node->next()) {
		if (!node->is_inst()) {
			continue; // labels, sentinels, comments, etc.
		}
		auto* inst = node->as<InstNode>();
		const InstId id = inst->inst_id();
		if (isFlagReader(id)) {
			return FlagScan::Live;
		}
		if (isFlagClobberer(id)) {
			return FlagScan::Clobbered;
		}
		// Flag-neutral instruction (mov, movzx, movsx, lea, SSE data movs,
		// float arithmetic that writes MXCSR rather than EFLAGS, ...). Keep
		// walking.
	}
	return FlagScan::Unknown;
}

/// Rule 1: decides whether an instruction is a same-register move we can
/// safely drop. Conservative allow-list — see class comment.
bool isRemovableSelfMove(const InstNode* inst) noexcept {
	if (inst->op_count() != 2) {
		return false;
	}
	const Operand& dst = inst->op(0);
	const Operand& src = inst->op(1);
	if (!dst.is_reg() || !src.is_reg()) {
		return false;
	}
	if (!dst.is_phys_reg() || !src.is_phys_reg()) {
		return false;
	}
	if (dst.id() != src.id()) {
		return false;
	}

	const InstId id = inst->inst_id();
	switch (id) {
	case Inst::kIdMov: {
		// Only delete 64-bit GPR self-moves. 32-bit self-moves are a
		// zero-extension idiom we must not touch; 8/16-bit have partial-
		// register hazards. This matches the allow-list in the design
		// doc.
		const auto& dstReg = dst.as<Reg>();
		const auto& srcReg = src.as<Reg>();
		if (dstReg.is_gp64() && srcReg.is_gp64()) {
			return true;
		}
		return false;
	}
	case Inst::kIdMovaps:
	case Inst::kIdMovapd:
	case Inst::kIdMovdqa:
	case Inst::kIdMovdqu:
		// Full 128-bit vector copies; same-register is a strict no-op.
		return true;
	default:
		return false;
	}
}

/// Rule 2 helper: detect `mov <gpreg>, imm(0)` patterns that are
/// candidates for the xor-zero rewrite.
bool isMovGpZeroImm(const InstNode* inst) noexcept {
	if (inst->inst_id() != Inst::kIdMov) {
		return false;
	}
	if (inst->op_count() != 2) {
		return false;
	}
	const Operand& dst = inst->op(0);
	const Operand& src = inst->op(1);
	if (!dst.is_reg() || !dst.is_phys_reg()) {
		return false;
	}
	if (!dst.as<Reg>().is_gp()) {
		return false;
	}
	if (!src.is_imm()) {
		return false;
	}
	return src.as<Imm>().value() == 0;
}

/// True when @p label is bound at the position directly after @p node, i.e.
/// among the run of label nodes that follows it before the next instruction.
/// Only label nodes are skipped: data (constant pools), alignment, or the
/// sentinel ending a function all sit between @p node and the label in the
/// emitted code, so a jump across them is not a fallthrough.
bool isBoundDirectlyAfter(const BaseNode* node, uint32_t labelId) noexcept {
	for (const BaseNode* next = node->next(); next != nullptr && next->is_label(); next = next->next()) {
		if (next->as<LabelNode>()->label_id() == labelId) {
			return true;
		}
	}
	return false;
}

/// The label targeted by a single-operand jump, or nullptr when @p inst is not
/// a direct jump to a label.
const Label* jumpTarget(const InstNode* inst) noexcept {
	if (inst->op_count() != 1 || !inst->op(0).is_label()) {
		return nullptr;
	}
	return &inst->op(0).as<Label>();
}

/// The condition code of a conditional jump, or kMaxValue+1 when @p id is not
/// a conditional jump.
uint32_t jccCondition(InstId id) noexcept {
	for (uint32_t cond = 0; cond < 16; cond++) {
		if (Inst::jcc_from_cond(CondCode(cond)) == id) {
			return cond;
		}
	}
	return 16;
}

/// Rules 3 and 4 (jump simplification) over the whole node list, once.
/// Returns the number of jumps removed.
int64_t simplifyJumps(BaseBuilder* cb) noexcept {
	int64_t removed = 0;
	BaseNode* node = cb->first_node();
	while (node != nullptr) {
		BaseNode* next = node->next();
		if (!node->is_inst()) {
			node = next;
			continue;
		}
		auto* inst = node->as<InstNode>();

		// Rule 3: a jump to the label bound right after it is a no-op. The
		// lowering emits one at nearly every block boundary (each
		// block-argument edge ends in an explicit jmp, and the target is
		// usually emitted next).
		if (inst->inst_id() == Inst::kIdJmp) {
			if (const Label* target = jumpTarget(inst); target != nullptr && isBoundDirectlyAfter(node, target->id())) {
				cb->remove_node(node);
				++removed;
			}
			node = next;
			continue;
		}

		// Rule 4: `jcc A; jmp B; A:` -> `jncc B; A:`. The jmp is not a branch
		// target itself (it directly follows the jcc), so folding it into the
		// inverted conditional jump removes one taken branch from the path
		// that previously went through it.
		if (const uint32_t cond = jccCondition(inst->inst_id()); cond < 16 && next != nullptr && next->is_inst()) {
			auto* jmp = next->as<InstNode>();
			const Label* condTarget = jumpTarget(inst);
			const Label* jmpTarget = jmp->inst_id() == Inst::kIdJmp ? jumpTarget(jmp) : nullptr;
			if (condTarget != nullptr && jmpTarget != nullptr && isBoundDirectlyAfter(jmp, condTarget->id())) {
				inst->set_inst_id(Inst::jcc_from_cond(negate_cond(CondCode(cond))));
				inst->set_op(0, *jmpTarget);
				// The new target may be out of rel8 range; let the assembler pick.
				inst->clear_options(InstOptions::kShortForm);
				next = jmp->next();
				cb->remove_node(jmp);
				++removed;
			}
		}
		node = next;
	}
	return removed;
}

} // namespace

Error X64PostRAPeepholePass::run(axl::Arena& /*arena*/, Logger* /*logger*/) {
	BaseBuilder* cb = &Pass::cb();

	int64_t selfMovesRemoved = 0;
	int64_t zeroIdiomsApplied = 0;
	// Removing one jump can make another one a fallthrough (`jmp L; M: jmp L;
	// L:`), so iterate to a fixpoint; each round only removes nodes.
	int64_t jumpsRemoved = 0;
	for (int64_t round = simplifyJumps(cb); round != 0; round = simplifyJumps(cb)) {
		jumpsRemoved += round;
	}

	BaseNode* node = cb->first_node();
	while (node != nullptr) {
		BaseNode* next = node->next();

		if (node->is_inst()) {
			auto* inst = node->as<InstNode>();

			if (isRemovableSelfMove(inst)) {
				cb->remove_node(node);
				++selfMovesRemoved;
				node = next;
				continue;
			}

			if (isMovGpZeroImm(inst) && flagScan(node) == FlagScan::Clobbered) {
				// Rewrite `mov <gp>, 0` as `xor <r32>, <r32>`. Using the
				// 32-bit alias of the destination implicitly zero-extends
				// to 64 on x86-64, matching the semantics of the original
				// `mov r64, 0`. Also shorter to encode and breaks the
				// data dependency on the previous value of the register.
				Gp gp = inst->op(0).as<Gp>().r32();
				inst->set_inst_id(Inst::kIdXor);
				inst->set_op(0, gp);
				inst->set_op(1, gp);
				++zeroIdiomsApplied;
			}
		}

		node = next;
	}

	// Publish counters via the pipeline-wide statistics sink. `add` creates
	// the key on first write and increments on subsequent calls, so this
	// handles multi-function compilation units correctly.
	if (statistics_ != nullptr) {
		statistics_->add("asmjit.peephole.selfMovesRemoved", selfMovesRemoved);
		statistics_->add("asmjit.peephole.zeroIdiomsApplied", zeroIdiomsApplied);
		statistics_->add("asmjit.peephole.jumpsRemoved", jumpsRemoved);
	}
	return kErrorOk;
}

} // namespace nautilus::compiler::asmjit
