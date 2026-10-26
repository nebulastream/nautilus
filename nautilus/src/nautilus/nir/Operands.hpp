#pragma once

#include "nautilus/extensions/ExtensionRegistry.hpp"
#include "nautilus/nir/Inst.hpp"
#include "nautilus/nir/Records.hpp"
#include <cstdint>

namespace nautilus::nir {

/// Calls `f(Ref&)` for every value reference an instruction holds, in operand order: the fields a and b, then the
/// words of its `extra` lists (call and branch arguments, and at the pre-SSA level the objects of cleanup lists).
/// Constants are visited too; callers filter with isValueRef. NONE slots are skipped.
/// InstT is `Inst` or `const Inst`, ExtraT is `uint32_t` or `const uint32_t`. Core opcodes only.
template <class InstT, class ExtraT, class F>
void forEachExtensionRef(InstT& inst, ExtraT* extra, F&& f) {
	auto* info = ext::ExtensionRegistry::instance().op(inst.op);
	if (info == nullptr) {
		return;
	}
	auto visit = [&](auto& slot) {
		if (slot != NONE) {
			f(slot);
		}
	};
	auto isRef = [](ext::FieldKind k) {
		return k == ext::FieldKind::Value || k == ext::FieldKind::Const;
	};
	if (isRef(info->shape.a)) {
		visit(inst.a);
	}
	if (isRef(info->shape.b)) {
		visit(inst.b);
	}
	if (isRef(info->shape.c)) {
		visit(inst.c);
	} else if (info->shape.c == ext::FieldKind::List) {
		ExtraT* list = extra + inst.c;
		uint32_t n = list[0];
		auto& shape = info->shape;
		for (uint32_t k = 0; k < n; ++k) {
			ext::WordKind kind = k < shape.prefix.size() ? shape.prefix[k]
			                     : shape.group.empty()   ? ext::WordKind::Imm
			                                             : shape.group[(k - shape.prefix.size()) % shape.group.size()];
			if (kind == ext::WordKind::Value || kind == ext::WordKind::Const) {
				visit(list[1 + k]);
			}
		}
	}
}

template <class InstT, class ExtraT, class F>
void forEachRef(InstT& inst, ExtraT* extra, Level level, F&& f) {
	auto* info = coreOpInfo(inst.op);
	if (info == nullptr) {
		if (inst.op >= FIRST_EXTENSION_OP) {
			forEachExtensionRef(inst, extra, f);
		}
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
