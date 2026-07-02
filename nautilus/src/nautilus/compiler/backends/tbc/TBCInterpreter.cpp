
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/compiler/backends/tbc/TBCHandlers.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <dyncall.h>
#include <vector>

// Dispatch capability detection. The tail-call skin needs Clang's guaranteed
// [[clang::musttail]]; the computed-goto skin needs the GNU labels-as-values
// extension (GCC and Clang). The switch skin always exists as the portable
// fallback (e.g. MSVC).
#if defined(__clang__) && defined(__has_cpp_attribute)
#if __has_cpp_attribute(clang::musttail)
#define TBC_HAS_MUSTTAIL 1
#endif
#endif
#if defined(__GNUC__) || defined(__clang__)
#define TBC_HAS_COMPUTED_GOTO 1
#endif

namespace nautilus::compiler::tbc {

namespace {

/// Per-thread VM state: one contiguous stack shared by all frames of all
/// nested invocations on this thread (reentrancy through external calls that
/// call back into TBC code just pushes further frames at `sp`).
struct VMContext {
	std::vector<uint64_t> storage;
	uint64_t* sp = nullptr;
	uint64_t* stackEnd = nullptr;
	const TBCProgram* prog = nullptr;
};

VMContext& tlsContext() {
	static thread_local VMContext ctx;
	return ctx;
}

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

/// The single instruction an entry frame "returns" to; its handler terminates
/// the dispatch loop and yields the entry result slot.
constexpr Instr kHalt {opIndex(Op::HALT), 0, 0, 0};

uint64_t* pushFrame(VMContext* ctx, const TBCFunction& fn, uint64_t* callerFp, const Instr* returnIp, uint16_t dstReg) {
	uint64_t* base = ctx->sp;
	if (base + fn.frameSlots > ctx->stackEnd) {
		throw RuntimeException("tbc: VM stack overflow (increase tbc.stackSizeKb)");
	}
	ctx->sp = base + fn.frameSlots;
	base[0] = reinterpret_cast<uint64_t>(callerFp);
	base[1] = reinterpret_cast<uint64_t>(returnIp);
	base[2] = dstReg == kNoReg ? ~uint64_t {0} : dstReg;
	uint64_t* fp = base + 3;
	std::memcpy(fp, fn.initImage.data(), fn.initImage.size() * sizeof(uint64_t));
	if (!fn.allocaRegs.empty()) {
		auto area = reinterpret_cast<uintptr_t>(fp + fn.regSlots);
		area = (area + 15u) & ~uintptr_t {15};
		std::memset(reinterpret_cast<void*>(area), 0, fn.allocaBytes);
		for (const auto& [reg, offset] : fn.allocaRegs) {
			fp[reg] = static_cast<uint64_t>(area + offset);
		}
	}
	return fp;
}

/// External call through dyncall: marshal the argument registers per the call
/// site's signature, call, and write the (re-normalized) result. Outgoing
/// dyncall builds the call frame dynamically without any runtime code
/// generation, so this path is iOS-safe (unlike dyncallback thunks).
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
		case Type::ui8:
			dcArgChar(vm, readReg<int8_t>(fp, r));
			break;
		case Type::i16:
		case Type::ui16:
			dcArgShort(vm, readReg<int16_t>(fp, r));
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

/// Control transfer produced by call/return helpers, applied by each skin.
struct Transfer {
	const Instr* ip;
	uint64_t* fp;
};

inline Transfer doReturn(const Instr& inst, uint64_t* fp, VMContext* ctx) {
	uint64_t* base = fp - 3;
	auto* callerFp = reinterpret_cast<uint64_t*>(base[0]);
	const auto* returnIp = reinterpret_cast<const Instr*>(base[1]);
	const uint64_t dst = base[2];
	if (dst != ~uint64_t {0}) {
		callerFp[dst] = inst.a == kNoReg ? 0 : fp[inst.a];
	}
	ctx->sp = base;
	return {returnIp, callerFp};
}

inline Transfer enterFunction(const TBCFunction& fn, const CallSite& site, const Instr* ip, uint64_t* fp,
                              VMContext* ctx, uint16_t dstReg) {
	uint64_t* newFp = pushFrame(ctx, fn, fp, ip + 1, dstReg);
	for (size_t i = 0; i < site.argRegs.size(); ++i) {
		newFp[fn.argRegs[i]] = fp[site.argRegs[i]];
	}
	return {fn.code.data(), newFp};
}

inline Transfer doCall(const Instr& inst, const Instr* ip, uint64_t* fp, VMContext* ctx) {
	const TBCProgram& prog = *ctx->prog;
	return enterFunction(prog.functions[inst.b], prog.callsites[inst.c], ip, fp, ctx, inst.a);
}

inline void doIndirectCall(const Instr& inst, uint64_t* fp, VMContext* ctx) {
	// Indirect targets are always real native pointers: internal functions
	// hand out pre-compiled trampolines (TBCTrampoline.hpp), so no
	// internal/external discrimination is needed here.
	const CallSite& site = ctx->prog->callsites[inst.c];
	doExtCall(site, reinterpret_cast<void*>(fp[inst.b]), fp, inst.a);
}

// ── Skin 1: portable for(;;) switch ─────────────────────────────────────────

uint64_t runSwitch(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	for (;;) {
		const Instr& inst = *ip;
		switch (static_cast<Op>(inst.op)) {
#define TBC_CASE(name, fn)                                                                                             \
	case Op::name:                                                                                                     \
		(fn)(inst, fp);                                                                                                \
		++ip;                                                                                                          \
		break;
			TBC_VALUE_OPCODE_LIST(TBC_CASE)
#undef TBC_CASE
		case Op::SELECT:
			fp[inst.a] = readReg<bool>(fp, inst.b) ? fp[inst.c] : fp[ip[1].a];
			ip += 2;
			break;
		case Op::JMP:
			ip += packedOffset(inst);
			break;
		case Op::CJMP:
			ip = readReg<bool>(fp, inst.a) ? ip + packedOffset(inst) : ip + 1;
			break;
#define TBC_CASE_F(name, ctype, cmp)                                                                                   \
	case Op::name:                                                                                                     \
		ip += (readReg<ctype>(fp, inst.a) cmp readReg<ctype>(fp, inst.b)) ? packedOffsetLoHi(ip[1]) : 2;               \
		break;
			TBC_CJMP_FUSED_LIST(TBC_CASE_F)
#undef TBC_CASE_F
		case Op::RET: {
			const auto t = doReturn(inst, fp, ctx);
			ip = t.ip;
			fp = t.fp;
			break;
		}
		case Op::CALL: {
			const auto t = doCall(inst, ip, fp, ctx);
			ip = t.ip;
			fp = t.fp;
			break;
		}
		case Op::CALL_EXT: {
			const CallSite& site = ctx->prog->callsites[inst.b];
			doExtCall(site, site.target, fp, inst.a);
			++ip;
			break;
		}
		case Op::CALL_IND:
			doIndirectCall(inst, fp, ctx);
			++ip;
			break;
		case Op::HALT:
			return fp[0];
		default:
			throw RuntimeException("tbc: invalid opcode in instruction stream");
		}
	}
}

// ── Skin 2: token-threaded computed goto (GCC / Clang) ──────────────────────

#ifdef TBC_HAS_COMPUTED_GOTO
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpedantic"
#ifdef __clang__
#pragma clang diagnostic ignored "-Wgnu-label-as-value"
#endif
uint64_t runGoto(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	static const void* const labels[] = {
#define TBC_LADDR_V(name, fn) &&L_##name,
	    TBC_VALUE_OPCODE_LIST(TBC_LADDR_V)
#undef TBC_LADDR_V
#define TBC_LADDR_C(name) &&L_##name,
	        TBC_CONTROL_SIMPLE_LIST(TBC_LADDR_C)
#undef TBC_LADDR_C
#define TBC_LADDR_F(name, ctype, cmp) &&L_##name,
	            TBC_CJMP_FUSED_LIST(TBC_LADDR_F)
#undef TBC_LADDR_F
	};
	static_assert(sizeof(labels) / sizeof(labels[0]) == kOpCount);

#define TBC_NEXT() goto* labels[ip->op]
	TBC_NEXT();

#define TBC_LBL(name, fn)                                                                                              \
	L_##name : {                                                                                                       \
		(fn)(*ip, fp);                                                                                                 \
		++ip;                                                                                                          \
		TBC_NEXT();                                                                                                    \
	}
	TBC_VALUE_OPCODE_LIST(TBC_LBL)
#undef TBC_LBL

L_SELECT: {
	fp[ip->a] = readReg<bool>(fp, ip->b) ? fp[ip->c] : fp[ip[1].a];
	ip += 2;
	TBC_NEXT();
}
L_JMP: {
	ip += packedOffset(*ip);
	TBC_NEXT();
}
L_CJMP: {
	ip = readReg<bool>(fp, ip->a) ? ip + packedOffset(*ip) : ip + 1;
	TBC_NEXT();
}
#define TBC_LBL_F(name, ctype, cmp)                                                                                    \
	L_##name : {                                                                                                       \
		ip += (readReg<ctype>(fp, ip->a) cmp readReg<ctype>(fp, ip->b)) ? packedOffsetLoHi(ip[1]) : 2;                 \
		TBC_NEXT();                                                                                                    \
	}
	TBC_CJMP_FUSED_LIST(TBC_LBL_F)
#undef TBC_LBL_F
L_RET: {
	const auto t = doReturn(*ip, fp, ctx);
	ip = t.ip;
	fp = t.fp;
	TBC_NEXT();
}
L_CALL: {
	const auto t = doCall(*ip, ip, fp, ctx);
	ip = t.ip;
	fp = t.fp;
	TBC_NEXT();
}
L_CALL_EXT: {
	const CallSite& site = ctx->prog->callsites[ip->b];
	doExtCall(site, site.target, fp, ip->a);
	++ip;
	TBC_NEXT();
}
L_CALL_IND: {
	doIndirectCall(*ip, fp, ctx);
	++ip;
	TBC_NEXT();
}
L_HALT:
	return fp[0];
L_TRAP:
	throw RuntimeException("tbc: invalid opcode in instruction stream");
#undef TBC_NEXT
}
#pragma GCC diagnostic pop
#endif // TBC_HAS_COMPUTED_GOTO

// ── Skin 3: tail-call threading (Clang musttail) ────────────────────────────
//
// Every opcode gets its own handler function; (ip, fp, ctx) stay pinned in
// argument registers across the whole execution, and each handler ends in a
// guaranteed tail call through the dispatch table — one indirect branch per
// instruction with a distinct, BTB-friendly branch site per opcode.

#ifdef TBC_HAS_MUSTTAIL
// preserve_none (Clang 19+, x86-64/AArch64) makes every general register
// caller-saved, which is exactly what a chain of mutually tail-calling
// handlers wants (CPython's 3.14 tail-call interpreter measured additional
// wins from it on top of musttail alone). It must be applied consistently to
// both the dispatch-table's function-pointer type and every handler stored
// in it -- a mismatch between a function's actual calling convention and the
// type used to call it through the table is undefined behavior.
#if defined(__has_attribute) && __has_attribute(preserve_none)
#define TBC_PRESERVE_NONE __attribute__((preserve_none))
#else
#define TBC_PRESERVE_NONE
#endif

using Handler = uint64_t(TBC_PRESERVE_NONE*)(const Instr*, uint64_t*, VMContext*);
extern const Handler kDispatchTable[kOpCount];

#define TBC_TAIL_NEXT() [[clang::musttail]] return kDispatchTable[ip->op](ip, fp, ctx)

#define TBC_TH(name, fn)                                                                                               \
	static uint64_t TBC_PRESERVE_NONE th_##name(const Instr* ip, uint64_t* fp, VMContext* ctx) {                       \
		(fn)(*ip, fp);                                                                                                 \
		++ip;                                                                                                          \
		TBC_TAIL_NEXT();                                                                                               \
	}
TBC_VALUE_OPCODE_LIST(TBC_TH)
#undef TBC_TH

static uint64_t TBC_PRESERVE_NONE th_SELECT(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	fp[ip->a] = readReg<bool>(fp, ip->b) ? fp[ip->c] : fp[ip[1].a];
	ip += 2;
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_JMP(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	ip += packedOffset(*ip);
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_CJMP(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	ip = readReg<bool>(fp, ip->a) ? ip + packedOffset(*ip) : ip + 1;
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_RET(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	const auto t = doReturn(*ip, fp, ctx);
	ip = t.ip;
	fp = t.fp;
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_CALL(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	const auto t = doCall(*ip, ip, fp, ctx);
	ip = t.ip;
	fp = t.fp;
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_CALL_EXT(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	const CallSite& site = ctx->prog->callsites[ip->b];
	doExtCall(site, site.target, fp, ip->a);
	++ip;
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_CALL_IND(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	doIndirectCall(*ip, fp, ctx);
	++ip;
	TBC_TAIL_NEXT();
}
static uint64_t TBC_PRESERVE_NONE th_HALT(const Instr*, uint64_t* fp, VMContext*) {
	return fp[0];
}
static uint64_t TBC_PRESERVE_NONE th_TRAP(const Instr*, uint64_t*, VMContext*) {
	throw RuntimeException("tbc: invalid opcode in instruction stream");
}
#define TBC_TH_F(name, ctype, cmp)                                                                                     \
	static uint64_t TBC_PRESERVE_NONE th_##name(const Instr* ip, uint64_t* fp, VMContext* ctx) {                       \
		ip += (readReg<ctype>(fp, ip->a) cmp readReg<ctype>(fp, ip->b)) ? packedOffsetLoHi(ip[1]) : 2;                 \
		TBC_TAIL_NEXT();                                                                                               \
	}
TBC_CJMP_FUSED_LIST(TBC_TH_F)
#undef TBC_TH_F
#undef TBC_TAIL_NEXT

const Handler kDispatchTable[kOpCount] = {
#define TBC_TADDR_V(name, fn) th_##name,
    TBC_VALUE_OPCODE_LIST(TBC_TADDR_V)
#undef TBC_TADDR_V
#define TBC_TADDR_C(name) th_##name,
        TBC_CONTROL_SIMPLE_LIST(TBC_TADDR_C)
#undef TBC_TADDR_C
#define TBC_TADDR_F(name, ctype, cmp) th_##name,
            TBC_CJMP_FUSED_LIST(TBC_TADDR_F)
#undef TBC_TADDR_F
};

uint64_t runTailcall(const Instr* ip, uint64_t* fp, VMContext* ctx) {
	return kDispatchTable[ip->op](ip, fp, ctx);
}
#undef TBC_PRESERVE_NONE
#endif // TBC_HAS_MUSTTAIL

} // namespace

DispatchMode bestAvailableDispatchMode() {
#if defined(TBC_HAS_MUSTTAIL)
	return DispatchMode::Tailcall;
#elif defined(TBC_HAS_COMPUTED_GOTO)
	return DispatchMode::Goto;
#else
	return DispatchMode::Switch;
#endif
}

DispatchMode clampDispatchMode(DispatchMode requested) {
	const auto best = bestAvailableDispatchMode();
	// Modes are ordered strongest-first in the enum; a request the build
	// cannot execute silently degrades to the best available.
	return static_cast<uint8_t>(requested) < static_cast<uint8_t>(best) ? best : requested;
}

uint64_t invoke(const TBCProgram& program, uint32_t functionIndex, const uint64_t* args, size_t argCount) {
	auto& ctx = tlsContext();

	// Size (or grow) the per-thread stack. Growth is only safe while no frame
	// is live, because frames hold interior pointers into the storage.
	constexpr size_t kDefaultStackSlots = 128 * 1024; // 1 MiB
	const size_t wantSlots = std::max<size_t>(static_cast<size_t>(program.minStackSlots), kDefaultStackSlots);
	const bool idle = ctx.storage.empty() || ctx.sp == ctx.storage.data();
	if (idle && ctx.storage.size() < wantSlots) {
		ctx.storage.assign(wantSlots, 0);
		ctx.sp = ctx.storage.data();
		ctx.stackEnd = ctx.storage.data() + ctx.storage.size();
	}

	const TBCFunction& fn = program.functions[functionIndex];

	// Save/restore around the run so reentrant invocations (an external call
	// that calls back into another TBC executable) and exceptions leave the
	// context exactly as they found it.
	struct Restore {
		VMContext& ctx;
		const TBCProgram* prog;
		uint64_t* sp;
		~Restore() {
			ctx.prog = prog;
			ctx.sp = sp;
		}
	} restore {ctx, ctx.prog, ctx.sp};
	ctx.prog = &program;

	uint64_t resultSlot = 0;
	uint64_t* fp = pushFrame(&ctx, fn, &resultSlot, &kHalt, 0);
	const size_t argsToCopy = std::min(argCount, fn.argRegs.size());
	for (size_t i = 0; i < argsToCopy; ++i) {
		fp[fn.argRegs[i]] = args[i];
	}
	const Instr* ip = fn.code.data();

	switch (program.dispatch) {
#ifdef TBC_HAS_MUSTTAIL
	case DispatchMode::Tailcall:
		return runTailcall(ip, fp, &ctx);
#endif
#ifdef TBC_HAS_COMPUTED_GOTO
	case DispatchMode::Goto:
		return runGoto(ip, fp, &ctx);
#endif
	default:
		return runSwitch(ip, fp, &ctx);
	}
}

} // namespace nautilus::compiler::tbc
