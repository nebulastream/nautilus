
#include "nautilus/compiler/backends/tbc/TBCVm.hpp"
#include "nautilus/compiler/backends/tbc/TBCHandlers.hpp"
#include <dyncall.h>

namespace nautilus::compiler::tbc {

VMContext& tlsContext() {
	static thread_local VMContext ctx;
	return ctx;
}

namespace {
DCCallVM* tlsDyncallVM() {
	struct Holder {
		DCCallVM* vm = dcNewCallVM(4096);
		~Holder() {
			dcFree(vm);
		}
	};
	static thread_local Holder holder;
	return holder.vm;
}
} // namespace

void doExtCall(const CallSite& site, void* target, uint64_t* fp, uint16_t dstReg) {
	DCCallVM* vm = tlsDyncallVM();
	dcReset(vm);
	for (size_t i = 0; i < site.argRegs.size(); ++i) {
		const uint16_t r = site.argRegs[i];
		switch (site.argTypes[i]) {
		case Type::b:
			dcArgBool(vm, readReg<bool>(fp, r));
			break;
		case Type::i8:
			dcArgChar(vm, readReg<int8_t>(fp, r));
			break;
		case Type::i16:
			dcArgShort(vm, readReg<int16_t>(fp, r));
			break;
		// dyncall widens DCchar/DCshort arguments with sign extension, so a
		// uint8_t 255 would reach the callee as -1. Where every argument takes
		// a full register or 8-byte stack slot, passing the zero-extended value
		// as an int is the same call. Apple arm64 packs stack arguments by
		// size, so it keeps the narrow form there.
		case Type::ui8:
#if defined(__APPLE__) && defined(__aarch64__)
			dcArgChar(vm, static_cast<DCchar>(readReg<uint8_t>(fp, r)));
#else
			dcArgInt(vm, static_cast<DCint>(readReg<uint8_t>(fp, r)));
#endif
			break;
		case Type::ui16:
#if defined(__APPLE__) && defined(__aarch64__)
			dcArgShort(vm, static_cast<DCshort>(readReg<uint16_t>(fp, r)));
#else
			dcArgInt(vm, static_cast<DCint>(readReg<uint16_t>(fp, r)));
#endif
			break;
		case Type::i32:
		case Type::ui32:
			dcArgInt(vm, readReg<int32_t>(fp, r));
			break;
		case Type::i64:
		case Type::ui64:
			dcArgLongLong(vm, readReg<int64_t>(fp, r));
			break;
		case Type::f32:
			dcArgFloat(vm, readReg<float>(fp, r));
			break;
		case Type::f64:
			dcArgDouble(vm, readReg<double>(fp, r));
			break;
		case Type::ptr:
			dcArgPointer(vm, reinterpret_cast<void*>(fp[r]));
			break;
		default:
			throw RuntimeException("tbc: unsupported external call argument type");
		}
	}
	switch (site.returnType) {
	case Type::v:
		dcCallVoid(vm, target);
		return;
	case Type::b:
		// Mask to the low byte: callees are only required to set the low byte
		// of a bool return register (same rationale as bc's Dyncall::callB).
		writeReg<bool>(fp, dstReg, (dcCallBool(vm, target) & 0xFF) != 0);
		return;
	case Type::i8:
		writeReg<int8_t>(fp, dstReg, dcCallChar(vm, target));
		return;
	case Type::ui8:
		writeReg<uint8_t>(fp, dstReg, static_cast<uint8_t>(dcCallChar(vm, target)));
		return;
	case Type::i16:
		writeReg<int16_t>(fp, dstReg, dcCallShort(vm, target));
		return;
	case Type::ui16:
		writeReg<uint16_t>(fp, dstReg, static_cast<uint16_t>(dcCallShort(vm, target)));
		return;
	case Type::i32:
		writeReg<int32_t>(fp, dstReg, dcCallInt(vm, target));
		return;
	case Type::ui32:
		writeReg<uint32_t>(fp, dstReg, static_cast<uint32_t>(dcCallInt(vm, target)));
		return;
	case Type::i64:
		writeReg<int64_t>(fp, dstReg, dcCallLongLong(vm, target));
		return;
	case Type::ui64:
		writeReg<uint64_t>(fp, dstReg, static_cast<uint64_t>(dcCallLongLong(vm, target)));
		return;
	case Type::f32:
		writeReg<float>(fp, dstReg, dcCallFloat(vm, target));
		return;
	case Type::f64:
		writeReg<double>(fp, dstReg, dcCallDouble(vm, target));
		return;
	case Type::ptr:
		writeReg<uint64_t>(fp, dstReg, reinterpret_cast<uint64_t>(dcCallPointer(vm, target)));
		return;
	}
	throw RuntimeException("tbc: unsupported external call return type");
}

void doIndirectCall(const Instr& inst, uint64_t* fp, VMContext* ctx) {
	const CallSite& site = ctx->prog->callsites[inst.c];
	doExtCall(site, reinterpret_cast<void*>(fp[inst.b]), fp, inst.a);
}

} // namespace nautilus::compiler::tbc
