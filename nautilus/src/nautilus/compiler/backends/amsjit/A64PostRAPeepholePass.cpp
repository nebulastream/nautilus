
#include "nautilus/compiler/backends/amsjit/A64PostRAPeepholePass.hpp"
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

} // namespace

Error A64PostRAPeepholePass::run(Zone* /*zone*/, Logger* /*logger*/) {
	BaseBuilder* cb = _cb;
	if (cb == nullptr) {
		return kErrorOk;
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
	}
	return kErrorOk;
}

} // namespace nautilus::compiler::asmjit
