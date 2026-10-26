#pragma once

#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Records.hpp"
#include <cstdint>

namespace nautilus::nir {

/// Calls `f(Ref&)` for every value reference an instruction holds, in operand order: the fields a and b, then the
/// words of its `extra` lists (call and branch arguments, and at the pre-SSA level the objects of cleanup lists).
/// Constants are visited too; callers filter with isValueRef. NONE slots are skipped.
/// InstT is `Inst` or `const Inst`, ExtraT is `uint32_t` or `const uint32_t`. Core opcodes only.
template <class InstT, class ExtraT, class F>
void forEachRef(InstT& inst, ExtraT* extra, Level level, F&& f) {
	auto* info = coreOpInfo(inst.op);
	if (info == nullptr) {
		return;
	}
	auto visit = [&](auto& slot) {
		if (slot != NONE) {
			f(slot);
		}
	};
	// cleanup list of a pre-SSA may-throw instruction: [n, (destructor callee, object ref) * n]
	auto cleanup = [&](uint32_t unwind) {
		if (level == Level::PreSSA && unwind != NO_UNWIND) {
			ExtraT* list = extra + unwind;
			for (uint32_t i = 0; i < list[0]; ++i) {
				visit(list[2 + 2 * i]);
			}
		}
	};
	if (info->a == OperandKind::Value || info->a == OperandKind::ValueOpt) {
		visit(inst.a);
	}
	if (info->b == OperandKind::Value) {
		visit(inst.b);
	}
	switch (static_cast<Op>(inst.op)) {
	case Op::Div:
	case Op::Rem:
		if (inst.flags & 2) {
			cleanup(inst.c);
		}
		break;
	case Op::Call:
	case Op::CallIndirect: {
		ExtraT* list = extra + inst.c;
		uint32_t argc = list[0];
		for (uint32_t i = 0; i < argc; ++i) {
			visit(list[1 + i]);
		}
		cleanup(list[1 + argc]);
		break;
	}
	case Op::Br: {
		ExtraT* list = extra + inst.c;
		for (uint32_t i = 0; i < list[0]; ++i) {
			visit(list[1 + i]);
		}
		break;
	}
	case Op::CondBr: {
		ExtraT* list = extra + inst.c;
		uint32_t tc = list[1];
		for (uint32_t i = 0; i < tc; ++i) {
			visit(list[2 + i]);
		}
		uint32_t fc = list[3 + tc];
		for (uint32_t i = 0; i < fc; ++i) {
			visit(list[4 + tc + i]);
		}
		break;
	}
	default:
		break;
	}
}

} // namespace nautilus::nir
