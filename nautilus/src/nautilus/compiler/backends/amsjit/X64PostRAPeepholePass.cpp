
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
	case Inst::kIdJa:
	case Inst::kIdJae:
	case Inst::kIdJb:
	case Inst::kIdJbe:
	case Inst::kIdJc:
	case Inst::kIdJe:
	case Inst::kIdJg:
	case Inst::kIdJge:
	case Inst::kIdJl:
	case Inst::kIdJle:
	case Inst::kIdJna:
	case Inst::kIdJnae:
	case Inst::kIdJnb:
	case Inst::kIdJnbe:
	case Inst::kIdJnc:
	case Inst::kIdJne:
	case Inst::kIdJng:
	case Inst::kIdJnge:
	case Inst::kIdJnl:
	case Inst::kIdJnle:
	case Inst::kIdJno:
	case Inst::kIdJnp:
	case Inst::kIdJns:
	case Inst::kIdJnz:
	case Inst::kIdJo:
	case Inst::kIdJp:
	case Inst::kIdJpe:
	case Inst::kIdJpo:
	case Inst::kIdJs:
	case Inst::kIdJz:
	// Conditional sets and moves.
	case Inst::kIdSeta:
	case Inst::kIdSetae:
	case Inst::kIdSetb:
	case Inst::kIdSetbe:
	case Inst::kIdSetc:
	case Inst::kIdSete:
	case Inst::kIdSetg:
	case Inst::kIdSetge:
	case Inst::kIdSetl:
	case Inst::kIdSetle:
	case Inst::kIdSetna:
	case Inst::kIdSetnae:
	case Inst::kIdSetnb:
	case Inst::kIdSetnbe:
	case Inst::kIdSetnc:
	case Inst::kIdSetne:
	case Inst::kIdSetng:
	case Inst::kIdSetnge:
	case Inst::kIdSetnl:
	case Inst::kIdSetnle:
	case Inst::kIdSetno:
	case Inst::kIdSetnp:
	case Inst::kIdSetns:
	case Inst::kIdSetnz:
	case Inst::kIdSeto:
	case Inst::kIdSetp:
	case Inst::kIdSetpe:
	case Inst::kIdSetpo:
	case Inst::kIdSets:
	case Inst::kIdSetz:
	case Inst::kIdCmova:
	case Inst::kIdCmovae:
	case Inst::kIdCmovb:
	case Inst::kIdCmovbe:
	case Inst::kIdCmovc:
	case Inst::kIdCmove:
	case Inst::kIdCmovg:
	case Inst::kIdCmovge:
	case Inst::kIdCmovl:
	case Inst::kIdCmovle:
	case Inst::kIdCmovna:
	case Inst::kIdCmovnae:
	case Inst::kIdCmovnb:
	case Inst::kIdCmovnbe:
	case Inst::kIdCmovnc:
	case Inst::kIdCmovne:
	case Inst::kIdCmovng:
	case Inst::kIdCmovnge:
	case Inst::kIdCmovnl:
	case Inst::kIdCmovnle:
	case Inst::kIdCmovno:
	case Inst::kIdCmovnp:
	case Inst::kIdCmovns:
	case Inst::kIdCmovnz:
	case Inst::kIdCmovo:
	case Inst::kIdCmovp:
	case Inst::kIdCmovpe:
	case Inst::kIdCmovpo:
	case Inst::kIdCmovs:
	case Inst::kIdCmovz:
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
		if (!node->isInst()) {
			continue; // labels, sentinels, comments, etc.
		}
		auto* inst = node->as<InstNode>();
		const InstId id = inst->id();
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
	if (inst->opCount() != 2) {
		return false;
	}
	const Operand& dst = inst->op(0);
	const Operand& src = inst->op(1);
	if (!dst.isReg() || !src.isReg()) {
		return false;
	}
	if (!dst.isPhysReg() || !src.isPhysReg()) {
		return false;
	}
	if (dst.id() != src.id()) {
		return false;
	}

	const InstId id = inst->id();
	switch (id) {
	case Inst::kIdMov: {
		// Only delete 64-bit GPR self-moves. 32-bit self-moves are a
		// zero-extension idiom we must not touch; 8/16-bit have partial-
		// register hazards. This matches the allow-list in the design
		// doc.
		const auto& dstReg = dst.as<Reg>();
		const auto& srcReg = src.as<Reg>();
		if (dstReg.isGp64() && srcReg.isGp64()) {
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
	if (inst->id() != Inst::kIdMov) {
		return false;
	}
	if (inst->opCount() != 2) {
		return false;
	}
	const Operand& dst = inst->op(0);
	const Operand& src = inst->op(1);
	if (!dst.isReg() || !dst.isPhysReg()) {
		return false;
	}
	if (!dst.as<Reg>().isGp()) {
		return false;
	}
	if (!src.isImm()) {
		return false;
	}
	return src.as<Imm>().value() == 0;
}

} // namespace

Error X64PostRAPeepholePass::run(Zone* /*zone*/, Logger* /*logger*/) {
	// The public cb() accessor returns a const pointer, but removeNode() is
	// a mutating operation. Access the non-const back-pointer directly via
	// the public _cb member.
	BaseBuilder* cb = _cb;
	if (cb == nullptr) {
		return kErrorOk;
	}

	int64_t selfMovesRemoved = 0;
	int64_t zeroIdiomsApplied = 0;

	BaseNode* node = cb->firstNode();
	while (node != nullptr) {
		BaseNode* next = node->next();

		if (node->isInst()) {
			auto* inst = node->as<InstNode>();

			if (isRemovableSelfMove(inst)) {
				cb->removeNode(node);
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
				inst->setId(Inst::kIdXor);
				inst->setOp(0, gp);
				inst->setOp(1, gp);
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
	}
	return kErrorOk;
}

} // namespace nautilus::compiler::asmjit
