// Copy-and-patch stencil source for the tbc backend.
//
// This translation unit is NOT compiled into libnautilus. It is the input to
// tools/stencils/build_stencils.py, which compiles it per target with a
// pinned flag set (-O2 -fno-pic -mcmodel=large -fno-jump-tables
// -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables
// -fomit-frame-pointer -fno-stack-protector -ffunction-sections), then
// harvests each stencil_<NAME> function's bytes and relocations into
// jit/generated/stencils_<arch>_<fmt>.inc.
//
// Each opcode handler becomes a stencil: a self-contained code fragment whose
// operands are *holes* — relocations against the magic _JIT_* symbols below —
// patched per instruction by the stitcher (TBCStitcher.cpp). Handlers end in
// a musttail jump through _JIT_CONTINUE/_JIT_TARGET, so after patching, the
// dispatch of the interpreter loop disappears entirely: control falls (or
// directly jumps) from one instruction's code to the next.
//
// Value semantics must match TBCHandlers.hpp exactly (same write
// normalization: bools 0/1, narrow integers zero-extended, floats bit-exact
// into a zeroed slot). The bodies below mirror those handlers 1:1, driven by
// the same X-macro lists in TBCOpcodes.hpp; the differential test suite
// (interp vs jit on identical bytecode) guards against drift.
//
// Constraints on stencil bodies (enforced by the extractor):
//  - no calls except the tbcJit* helpers (plain ABI, defined in libnautilus)
//  - no references to globals other than the magic hole symbols
//  - no exceptions, no jump tables, no .data/.bss; read-only literal pools
//    are allowed (harvested into the stencil data image)

// Freestanding: compiled with -ffreestanding -nostdlibinc so it can be
// cross-compiled for any target without a sysroot. Only compiler-provided
// headers (via the <cstdint> shim in tools/stencils/include/) and builtins.
#include "nautilus/compiler/backends/tbc/TBCInstr.hpp"
#include "nautilus/compiler/backends/tbc/TBCOpcodes.hpp"

#if !defined(__clang__)
#error "stencils must be compiled with clang (musttail + preserve_none)"
#endif

#define STENCIL_CC __attribute__((preserve_none))

// Prefix-compatible view of TBCVm.hpp's VMContext: `sp` must be its first
// member (static_asserts in jit/TBCStencilHelpers.cpp pin this).
struct VMContextPrefix {
	uint64_t* sp;
};

using JitHandler = uint64_t(STENCIL_CC*)(uint64_t*, VMContextPrefix*);

// Magic hole symbols. Only their *addresses* matter: each use compiles to an
// absolute-address materialization carrying a relocation, which the extractor
// records as a hole and the stitcher patches per instruction. The stitcher
// decides the patch value per (opcode, symbol): register fields are patched
// pre-scaled as byte offsets into the frame, immediate fields as sign-extended
// 64-bit values, raw fields zero-extended.
extern "C" {
extern char _JIT_A;
extern char _JIT_B;
extern char _JIT_C;
extern char _JIT_D;           // 2nd-word field a (SELECT false register)
extern char _JIT_TARGET;      // stitched address of the branch-taken instruction
extern char _JIT_CONTINUE;    // stitched address of the next instruction
extern char _JIT_CALL_TARGET; // stitched entry of the callee (CALL)
extern char _JIT_SITE;        // &program->callsites[siteIdx]
extern char _JIT_FUNC;        // &program->functions[fnIdx]
extern char _JIT_UNWIND;      // program's @UNWIND stencil
}

// Plain-ABI helpers, defined in jit/TBCStencilHelpers.cpp. They never throw:
// C++ exceptions must not unwind through stitched frames (no unwind info), so
// failures are captured into VMContext::pendingException and signalled by the
// return value; the stencil then exits through _JIT_UNWIND.
extern "C" {
uint64_t* tbcJitPushFrame(VMContextPrefix* ctx, const void* callee, const void* site, uint64_t* callerFp,
                          void* nativeReturnAddress, uint64_t dstRegRaw);
uint64_t tbcJitExtCall(VMContextPrefix* ctx, const void* site, uint64_t* fp, uint64_t dstRegRaw);
uint64_t tbcJitIndCall(VMContextPrefix* ctx, const void* site, uint64_t* fp, uint64_t dstRegRaw, void* target);
}

#define OFF_A ((uint64_t)(uintptr_t) & _JIT_A)
#define OFF_B ((uint64_t)(uintptr_t) & _JIT_B)
#define OFF_C ((uint64_t)(uintptr_t) & _JIT_C)
#define OFF_D ((uint64_t)(uintptr_t) & _JIT_D)

#define TAIL_NEXT() [[clang::musttail]] return ((JitHandler)&_JIT_CONTINUE)(fp, ctx)
#define TAIL_TARGET() [[clang::musttail]] return ((JitHandler)&_JIT_TARGET)(fp, ctx)
#define TAIL_UNWIND() [[clang::musttail]] return ((JitHandler)&_JIT_UNWIND)(fp, ctx)

namespace {

#define SINLINE inline __attribute__((always_inline))

// Minimal trait machinery (freestanding: no <type_traits>).
template <class T>
struct Unsigned {
	using type = T;
};
template <>
struct Unsigned<int8_t> {
	using type = uint8_t;
};
template <>
struct Unsigned<int16_t> {
	using type = uint16_t;
};
template <>
struct Unsigned<int32_t> {
	using type = uint32_t;
};
template <>
struct Unsigned<int64_t> {
	using type = uint64_t;
};

// Frame accessors on pre-scaled byte offsets (the patched _JIT_A/B/C values
// are reg*8). Semantics mirror TBCHandlers.hpp's readReg/writeReg.
template <class T>
SINLINE T rd(const uint64_t* fp, uint64_t off) {
	T value;
	__builtin_memcpy(&value, reinterpret_cast<const char*>(fp) + off, sizeof(T));
	return value;
}

template <class T>
SINLINE void wr(uint64_t* fp, uint64_t off, T value) {
	auto* slot = reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(fp) + off);
	if constexpr (__is_same(T, bool)) {
		*slot = value ? 1u : 0u;
	} else if constexpr (__is_integral(T)) {
		*slot = static_cast<uint64_t>(static_cast<typename Unsigned<T>::type>(value));
	} else {
		*slot = 0;
		__builtin_memcpy(slot, &value, sizeof(T));
	}
}

SINLINE uint64_t ld64(const uint64_t* fp, uint64_t off) {
	return *reinterpret_cast<const uint64_t*>(reinterpret_cast<const char*>(fp) + off);
}

SINLINE void st64(uint64_t* fp, uint64_t off, uint64_t value) {
	*reinterpret_cast<uint64_t*>(reinterpret_cast<char*>(fp) + off) = value;
}

// Value-op bodies. Same names and pairing as TBCHandlers.hpp so the shared
// X-macro lists in TBCOpcodes.hpp generate the stencil for every opcode from
// the same (name, handler) association. Signature: (fp, a, b, c) where a/b/c
// are the patched operand values (byte offsets, or the sign-extended
// immediate for the *_imm_* / *_off_* families' c).

SINLINE void opNop(uint64_t*, uint64_t, uint64_t, uint64_t) {
}

SINLINE void opMov(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	st64(fp, a, ld64(fp, b));
}

template <class T>
SINLINE void opMovImm(uint64_t* fp, uint64_t a, uint64_t, uint64_t imm) {
	wr<T>(fp, a, static_cast<T>(static_cast<int64_t>(imm)));
}

template <class T>
SINLINE void opAdd(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) + rd<T>(fp, c)));
}

template <class T>
SINLINE void opSub(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) - rd<T>(fp, c)));
}

template <class T>
SINLINE void opMul(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) * rd<T>(fp, c)));
}

template <class T>
SINLINE void opDiv(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) / rd<T>(fp, c)));
}

template <class T>
SINLINE void opMod(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) % rd<T>(fp, c)));
}

template <class T>
SINLINE void opAddImm(uint64_t* fp, uint64_t a, uint64_t b, uint64_t imm) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) + static_cast<T>(static_cast<int64_t>(imm))));
}

template <class T>
SINLINE void opSubImm(uint64_t* fp, uint64_t a, uint64_t b, uint64_t imm) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) - static_cast<T>(static_cast<int64_t>(imm))));
}

template <class T>
SINLINE void opMulImm(uint64_t* fp, uint64_t a, uint64_t b, uint64_t imm) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) * static_cast<T>(static_cast<int64_t>(imm))));
}

template <class T>
SINLINE void opEq(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<T>(fp, b) == rd<T>(fp, c));
}

template <class T>
SINLINE void opNe(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<T>(fp, b) != rd<T>(fp, c));
}

template <class T>
SINLINE void opLt(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<T>(fp, b) < rd<T>(fp, c));
}

template <class T>
SINLINE void opLe(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<T>(fp, b) <= rd<T>(fp, c));
}

template <class T>
SINLINE void opGt(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<T>(fp, b) > rd<T>(fp, c));
}

template <class T>
SINLINE void opGe(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<T>(fp, b) >= rd<T>(fp, c));
}

SINLINE void opAnd(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<bool>(fp, b) && rd<bool>(fp, c));
}

SINLINE void opOr(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<bool>(fp, a, rd<bool>(fp, b) || rd<bool>(fp, c));
}

SINLINE void opNot(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	wr<bool>(fp, a, !rd<bool>(fp, b));
}

template <class T>
SINLINE void opLoad(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	T value;
	__builtin_memcpy(&value, reinterpret_cast<const void*>(ld64(fp, b)), sizeof(T));
	wr<T>(fp, a, value);
}

template <class T>
SINLINE void opStore(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	T value = rd<T>(fp, b);
	__builtin_memcpy(reinterpret_cast<void*>(ld64(fp, a)), &value, sizeof(T));
}

template <class T>
SINLINE void opLoadOff(uint64_t* fp, uint64_t a, uint64_t b, uint64_t imm) {
	T value;
	const uint64_t addr = ld64(fp, b) + imm;
	__builtin_memcpy(&value, reinterpret_cast<const void*>(addr), sizeof(T));
	wr<T>(fp, a, value);
}

template <class T>
SINLINE void opStoreOff(uint64_t* fp, uint64_t a, uint64_t b, uint64_t imm) {
	T value = rd<T>(fp, b);
	const uint64_t addr = ld64(fp, a) + imm;
	__builtin_memcpy(reinterpret_cast<void*>(addr), &value, sizeof(T));
}

template <class T>
SINLINE void opBand(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) & rd<T>(fp, c)));
}

template <class T>
SINLINE void opBor(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) | rd<T>(fp, c)));
}

template <class T>
SINLINE void opBxor(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) ^ rd<T>(fp, c)));
}

template <class T>
SINLINE void opShl(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) << rd<T>(fp, c)));
}

template <class T>
SINLINE void opShr(uint64_t* fp, uint64_t a, uint64_t b, uint64_t c) {
	wr<T>(fp, a, static_cast<T>(rd<T>(fp, b) >> rd<T>(fp, c)));
}

SINLINE void opBnot(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	wr<int64_t>(fp, a, ~rd<int64_t>(fp, b));
}

template <class T>
SINLINE void opNeg(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	wr<T>(fp, a, -rd<T>(fp, b));
}

template <class S, class D>
SINLINE void opCast(uint64_t* fp, uint64_t a, uint64_t b, uint64_t) {
	wr<D>(fp, a, static_cast<D>(rd<S>(fp, b)));
}

} // namespace

// ── Value-op stencils: one per TBC_VALUE_OPCODE_LIST entry ──────────────────

#define TBC_STENCIL_V(name, fn)                                                                                        \
	extern "C" uint64_t STENCIL_CC stencil_##name(uint64_t* fp, VMContextPrefix* ctx) {                                \
		(fn)(fp, OFF_A, OFF_B, OFF_C);                                                                                 \
		TAIL_NEXT();                                                                                                   \
	}
TBC_VALUE_OPCODE_LIST(TBC_STENCIL_V)
#undef TBC_STENCIL_V

// ── Control stencils ────────────────────────────────────────────────────────

extern "C" uint64_t STENCIL_CC stencil_SELECT(uint64_t* fp, VMContextPrefix* ctx) {
	st64(fp, OFF_A, rd<bool>(fp, OFF_B) ? ld64(fp, OFF_C) : ld64(fp, OFF_D));
	TAIL_NEXT();
}

extern "C" uint64_t STENCIL_CC stencil_JMP(uint64_t* fp, VMContextPrefix* ctx) {
	TAIL_TARGET();
}

extern "C" uint64_t STENCIL_CC stencil_CJMP(uint64_t* fp, VMContextPrefix* ctx) {
	if (rd<bool>(fp, OFF_A)) {
		TAIL_TARGET();
	}
	TAIL_NEXT();
}

#define TBC_STENCIL_F(name, ctype, cmp)                                                                                \
	extern "C" uint64_t STENCIL_CC stencil_##name(uint64_t* fp, VMContextPrefix* ctx) {                                \
		if (rd<ctype>(fp, OFF_A) cmp rd<ctype>(fp, OFF_B)) {                                                           \
			TAIL_TARGET();                                                                                             \
		}                                                                                                              \
		TAIL_NEXT();                                                                                                   \
	}
TBC_CJMP_FUSED_LIST(TBC_STENCIL_F)
#undef TBC_STENCIL_F

// RET pops the frame inline (mirrors TBCInterpreter.cpp's doReturn) and
// tail-jumps to the native continuation the caller's CALL stencil stored in
// the frame header. Two variants: the stitcher picks by `a == kNoReg`.
extern "C" uint64_t STENCIL_CC stencil_RET_VALUE(uint64_t* fp, VMContextPrefix* ctx) {
	uint64_t* base = fp - 3;
	auto* callerFp = reinterpret_cast<uint64_t*>(base[0]);
	const uint64_t returnAddress = base[1];
	const uint64_t dst = base[2];
	if (dst != ~uint64_t {0}) {
		callerFp[dst] = ld64(fp, OFF_A);
	}
	ctx->sp = base;
	[[clang::musttail]] return (reinterpret_cast<JitHandler>(returnAddress))(callerFp, ctx);
}

extern "C" uint64_t STENCIL_CC stencil_RET_VOID(uint64_t* fp, VMContextPrefix* ctx) {
	uint64_t* base = fp - 3;
	auto* callerFp = reinterpret_cast<uint64_t*>(base[0]);
	const uint64_t returnAddress = base[1];
	const uint64_t dst = base[2];
	if (dst != ~uint64_t {0}) {
		callerFp[dst] = 0;
	}
	ctx->sp = base;
	[[clang::musttail]] return (reinterpret_cast<JitHandler>(returnAddress))(callerFp, ctx);
}

// Internal call: the helper pushes the callee frame (with _JIT_CONTINUE as
// the native return address) and copies the arguments; then jump into the
// callee's stitched entry.
extern "C" uint64_t STENCIL_CC stencil_CALL(uint64_t* fp, VMContextPrefix* ctx) {
	uint64_t* newFp = tbcJitPushFrame(ctx, &_JIT_FUNC, &_JIT_SITE, fp, &_JIT_CONTINUE, OFF_A);
	if (newFp == nullptr) {
		TAIL_UNWIND();
	}
	[[clang::musttail]] return ((JitHandler)&_JIT_CALL_TARGET)(newFp, ctx);
}

extern "C" uint64_t STENCIL_CC stencil_CALL_EXT(uint64_t* fp, VMContextPrefix* ctx) {
	if (tbcJitExtCall(ctx, &_JIT_SITE, fp, OFF_A) == 0) {
		TAIL_UNWIND();
	}
	TAIL_NEXT();
}

extern "C" uint64_t STENCIL_CC stencil_CALL_IND(uint64_t* fp, VMContextPrefix* ctx) {
	if (tbcJitIndCall(ctx, &_JIT_SITE, fp, OFF_A, reinterpret_cast<void*>(ld64(fp, OFF_B))) == 0) {
		TAIL_UNWIND();
	}
	TAIL_NEXT();
}

// ── Per-program synthetic stencils ──────────────────────────────────────────

// Entry-frame landing pad (the HALT analog): after RET pops the entry frame,
// callerFp points at the entry shim's result slot; plain-return it.
extern "C" uint64_t STENCIL_CC stencil_EPILOGUE(uint64_t* fp, VMContextPrefix*) {
	return fp[0];
}

// Pending-exception exit: a helper recorded the exception in the VM context;
// return to the entry shim, which restores the VM stack and rethrows.
extern "C" uint64_t STENCIL_CC stencil_UNWIND(uint64_t*, VMContextPrefix*) {
	return 0;
}
