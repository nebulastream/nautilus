
// AsmJit vector intrinsic plugin. Mirrors plugins/simd/src/MLIRVectorIntrinsics.cpp:
// intercepts vector_*_impl function-pointer ProxyCalls and replaces them with
// native 128-bit SIMD instructions (XMM on x86-64, NEON on AArch64).
//
// Memory model is the same as the MLIR plugin: vectors flow through the IR as
// opaque pointers (vector_data<T,N>*). Each handler loads the input vector(s)
// from the pointer arguments into SIMD registers, computes, stores the result
// into a fresh stack slot (`cc.newStack(16, 16)`), and binds the result
// identifier to a Gp/Xn pointer to that slot. This keeps the existing
// RegisterFrame variant (Gp+scalar XMM/Vec) unchanged — SIMD registers never
// escape a handler.
//
// Width scope: only 128-bit configurations (f32x4, f64x2, i32x4, i64x2) are
// registered. Wider widths (f32x8, …) are intentionally left unregistered so
// the manager misses on lookup and AsmJit's existing scalar invoke path
// transparently runs the C++ fallbacks in plugins/simd/src/vector.cpp.

#include "AsmJitVectorIntrinsics.hpp"
#include "nautilus/compiler/backends/amsjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "vector_impl.hpp"
#include <cstdint>
#include <type_traits>

namespace nautilus::compiler::asmjit {

namespace {

// ============================================================================
// x86-64 / SSE4.1 path
// ============================================================================
#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)

using namespace ::asmjit;
using namespace ::asmjit::x86;

// --- argument / result helpers ----------------------------------------------

static Gp argGp(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Gp>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

static Xmm argXmm(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Xmm>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

// Load a 128-bit vector from a Gp address into a fresh Xmm.
static Xmm loadVec(Compiler& cc, const Gp& ptr) {
	auto v = cc.newXmm();
	cc.movdqu(v, x86::ptr(ptr));
	return v;
}

// Store a 128-bit vector into a fresh 16-byte aligned stack slot; return a Gp
// holding the slot's address. The slot is valid for the rest of the function.
static Gp storeVecToStack(Compiler& cc, const Xmm& v) {
	auto slot = cc.newStack(16, 16); // returns Mem (sp-relative)
	cc.movdqu(slot, v);
	auto p = cc.newIntPtr();
	cc.lea(p, slot);
	return p;
}

// Bind a pointer result into the frame.
static void bindPtr(IntrinsicCallContext& ctx, const Gp& p) {
	ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {p});
}

// ── Load / Store ────────────────────────────────────────────────────────────

template <typename ElemT, int N>
static bool handleLoad(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto srcPtr = argGp(ctx, 0);
	auto v = loadVec(cc, srcPtr);
	bindPtr(ctx, storeVecToStack(cc, v));
	return true;
}

template <typename ElemT, int N>
static bool handleStore(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto destPtr = argGp(ctx, 0);
	auto srcVecPtr = argGp(ctx, 1);
	auto v = loadVec(cc, srcVecPtr);
	cc.movdqu(x86::ptr(destPtr), v);
	// store returns void: no frame.setValue needed.
	return true;
}

// ── Binary arithmetic (op selector via tag dispatch) ────────────────────────

enum class FOp { Add, Sub, Mul, Div, Min, Max };
enum class IOp { Add, Sub, Mul, And, Or, Xor, Min, Max, Shl, Shr };

template <typename ElemT, int N, FOp Op>
static bool handleFloatBinary(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	if constexpr (std::is_same_v<ElemT, float>) {
		if constexpr (Op == FOp::Add)
			cc.addps(a, b);
		else if constexpr (Op == FOp::Sub)
			cc.subps(a, b);
		else if constexpr (Op == FOp::Mul)
			cc.mulps(a, b);
		else if constexpr (Op == FOp::Div)
			cc.divps(a, b);
		else if constexpr (Op == FOp::Min)
			cc.minps(a, b);
		else if constexpr (Op == FOp::Max)
			cc.maxps(a, b);
	} else {
		if constexpr (Op == FOp::Add)
			cc.addpd(a, b);
		else if constexpr (Op == FOp::Sub)
			cc.subpd(a, b);
		else if constexpr (Op == FOp::Mul)
			cc.mulpd(a, b);
		else if constexpr (Op == FOp::Div)
			cc.divpd(a, b);
		else if constexpr (Op == FOp::Min)
			cc.minpd(a, b);
		else if constexpr (Op == FOp::Max)
			cc.maxpd(a, b);
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

template <typename ElemT, int N, IOp Op>
static bool handleIntBinary(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		if constexpr (Op == IOp::Add)
			cc.paddd(a, b);
		else if constexpr (Op == IOp::Sub)
			cc.psubd(a, b);
		else if constexpr (Op == IOp::Mul)
			cc.pmulld(a, b); // SSE4.1
		else if constexpr (Op == IOp::And)
			cc.pand(a, b);
		else if constexpr (Op == IOp::Or)
			cc.por(a, b);
		else if constexpr (Op == IOp::Xor)
			cc.pxor(a, b);
		else if constexpr (Op == IOp::Min)
			cc.pminsd(a, b); // SSE4.1
		else if constexpr (Op == IOp::Max)
			cc.pmaxsd(a, b); // SSE4.1
	} else {
		// int64_t
		if constexpr (Op == IOp::Add)
			cc.paddq(a, b);
		else if constexpr (Op == IOp::Sub)
			cc.psubq(a, b);
		else if constexpr (Op == IOp::And)
			cc.pand(a, b);
		else if constexpr (Op == IOp::Or)
			cc.por(a, b);
		else if constexpr (Op == IOp::Xor)
			cc.pxor(a, b);
		// Mul/Min/Max for i64x2 intentionally not registered → scalar fallback.
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

// Variable shift: shift count comes as the lane-0 element of the second
// operand vector. We use psll/psrl/psra with the count in an XMM register
// (interpreted as a 64-bit shift amount in the low qword), matching the
// "shift each lane by the same amount" semantics of the scalar fallback.
template <typename ElemT, int N, IOp Op>
static bool handleIntShift(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto count = loadVec(cc, argGp(ctx, 1)); // psll uses low 64 bits as count
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		if constexpr (Op == IOp::Shl)
			cc.pslld(a, count);
		else if constexpr (Op == IOp::Shr)
			cc.psrad(a, count);
	} else {
		if constexpr (Op == IOp::Shl)
			cc.psllq(a, count);
		// Arithmetic shift right on 64-bit lanes (psraq) doesn't exist on
		// SSE2/4 → not registered for i64; falls back to scalar.
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

// ── Unary ───────────────────────────────────────────────────────────────────

// Float abs: clear sign bit using AND with mask (0x7FFFFFFF for f32, 0x7FFF…F
// for f64). Embed the mask via a stack slot rather than memory operand.
template <typename ElemT, int N>
static bool handleAbsFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	// Build the mask vector in a fresh xmm via two-step: load 64-bit immediate
	// to Gp → movq → pshufd to splat.
	auto maskScratch = cc.newXmm();
	auto tmpGp = cc.newUInt64();
	if constexpr (std::is_same_v<ElemT, float>) {
		cc.mov(tmpGp, uint64_t(0x7FFFFFFF7FFFFFFFULL));
		cc.movq(maskScratch, tmpGp);
		cc.punpcklqdq(maskScratch, maskScratch);
		cc.andps(a, maskScratch);
	} else {
		cc.mov(tmpGp, uint64_t(0x7FFFFFFFFFFFFFFFULL));
		cc.movq(maskScratch, tmpGp);
		cc.punpcklqdq(maskScratch, maskScratch);
		cc.andpd(a, maskScratch);
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

template <typename ElemT, int N>
static bool handleNegFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto signMask = cc.newXmm();
	auto tmpGp = cc.newUInt64();
	if constexpr (std::is_same_v<ElemT, float>) {
		cc.mov(tmpGp, uint64_t(0x8000000080000000ULL));
		cc.movq(signMask, tmpGp);
		cc.punpcklqdq(signMask, signMask);
		cc.xorps(a, signMask);
	} else {
		cc.mov(tmpGp, uint64_t(0x8000000000000000ULL));
		cc.movq(signMask, tmpGp);
		cc.punpcklqdq(signMask, signMask);
		cc.xorpd(a, signMask);
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

template <typename ElemT, int N>
static bool handleAbsInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		cc.pabsd(a, a); // SSSE3
		bindPtr(ctx, storeVecToStack(cc, a));
		return true;
	}
	// i64x2 abs: scalar fallback (no SSE2/4 pabsq).
	return false;
}

template <typename ElemT, int N>
static bool handleNegInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto zero = cc.newXmm();
	cc.pxor(zero, zero);
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		cc.psubd(zero, a);
	} else {
		cc.psubq(zero, a);
	}
	bindPtr(ctx, storeVecToStack(cc, zero));
	return true;
}

// ── FMA (3-operand) — emit mul+add for portability ──────────────────────────

template <typename ElemT, int N>
static bool handleFma(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto c = loadVec(cc, argGp(ctx, 2));
	if constexpr (std::is_same_v<ElemT, float>) {
		cc.mulps(a, b);
		cc.addps(a, c);
	} else {
		cc.mulpd(a, b);
		cc.addpd(a, c);
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

// ── Reductions (return scalar, not pointer) ─────────────────────────────────

// Float reduce_add: horizontal pairwise add. SSE2-safe (no haddps dep).
template <typename ElemT, int N>
static bool handleReduceAddFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, float>) {
		// v = [a, b, c, d]
		auto t = cc.newXmm();
		cc.movaps(t, v);
		cc.shufps(t, t, ::asmjit::Imm(0b10'11'00'01)); // [b, a, d, c]
		cc.addps(v, t);                                // [a+b, a+b, c+d, c+d]
		cc.movaps(t, v);
		cc.shufps(t, t, ::asmjit::Imm(0b01'00'11'10)); // [c+d, c+d, a+b, a+b]
		cc.addps(v, t);                                // total in every lane
		// scalar result lives in xmm[0]
		auto out = cc.newXmmSs();
		cc.movss(out, v);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else {
		// v = [a, b]
		auto t = cc.newXmm();
		cc.movapd(t, v);
		cc.shufpd(t, t, ::asmjit::Imm(0b01)); // [b, a]
		cc.addpd(v, t);                       // [a+b, a+b]
		auto out = cc.newXmmSd();
		cc.movsd(out, v);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	}
	return true;
}

template <typename ElemT, int N, bool IsMin>
static bool handleReduceMinMaxFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, float>) {
		auto t = cc.newXmm();
		cc.movaps(t, v);
		cc.shufps(t, t, ::asmjit::Imm(0b10'11'00'01));
		if constexpr (IsMin)
			cc.minps(v, t);
		else
			cc.maxps(v, t);
		cc.movaps(t, v);
		cc.shufps(t, t, ::asmjit::Imm(0b01'00'11'10));
		if constexpr (IsMin)
			cc.minps(v, t);
		else
			cc.maxps(v, t);
		auto out = cc.newXmmSs();
		cc.movss(out, v);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else {
		auto t = cc.newXmm();
		cc.movapd(t, v);
		cc.shufpd(t, t, ::asmjit::Imm(0b01));
		if constexpr (IsMin)
			cc.minpd(v, t);
		else
			cc.maxpd(v, t);
		auto out = cc.newXmmSd();
		cc.movsd(out, v);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	}
	return true;
}

// Int reduce: spill to stack and sum scalar — simple and correct.
template <typename ElemT, int N>
static bool handleReduceAddInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	auto slot = cc.newStack(16, 16);
	cc.movdqu(slot, v);
	auto acc = cc.newInt64();
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto base = cc.newIntPtr();
		cc.lea(base, slot);
		auto e = cc.newInt32();
		cc.movsxd(acc, x86::dword_ptr(base, 0));
		cc.movsxd(e.r64(), x86::dword_ptr(base, 4));
		cc.add(acc, e.r64());
		cc.movsxd(e.r64(), x86::dword_ptr(base, 8));
		cc.add(acc, e.r64());
		cc.movsxd(e.r64(), x86::dword_ptr(base, 12));
		cc.add(acc, e.r64());
	} else {
		auto base = cc.newIntPtr();
		cc.lea(base, slot);
		auto e = cc.newInt64();
		cc.mov(acc, x86::qword_ptr(base, 0));
		cc.mov(e, x86::qword_ptr(base, 8));
		cc.add(acc, e);
	}
	ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {acc});
	return true;
}

template <typename ElemT, int N, bool IsMin>
static bool handleReduceMinMaxInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		// Two-step shuffle reduction with SSE4.1 pmins/pmaxs.
		auto t = cc.newXmm();
		cc.movdqa(t, v);
		cc.pshufd(t, t, ::asmjit::Imm(0b10'11'00'01));
		if constexpr (IsMin)
			cc.pminsd(v, t);
		else
			cc.pmaxsd(v, t);
		cc.movdqa(t, v);
		cc.pshufd(t, t, ::asmjit::Imm(0b01'00'11'10));
		if constexpr (IsMin)
			cc.pminsd(v, t);
		else
			cc.pmaxsd(v, t);
		// Result is in lane 0.
		auto out = cc.newInt64();
		auto out32 = cc.newInt32();
		cc.movd(out32, v);
		cc.movsxd(out, out32);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
		return true;
	}
	(void) v;
	// i64 min/max: scalar fallback.
	return false;
}

// ── Comparisons (return mask vector pointer) ────────────────────────────────
//
// Match scalar semantics: per-lane all-1s on true, all-0s on false (vector.cpp
// uses memset 0xFF for mask_true, zero for mask_false).

enum class FCmp { Eq, Ne, Lt, Le, Gt, Ge };

template <typename ElemT, int N, FCmp Pred>
static bool handleCmpFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	// cmpps/cmppd predicate: 0=EQ, 1=LT, 2=LE, 3=UNORD, 4=NE, 5=NLT, 6=NLE
	int imm = 0;
	switch (Pred) {
	case FCmp::Eq:
		imm = 0;
		break;
	case FCmp::Lt:
		imm = 1;
		break;
	case FCmp::Le:
		imm = 2;
		break;
	case FCmp::Ne:
		imm = 4;
		break;
	case FCmp::Ge:
		imm = 5;
		break;
	case FCmp::Gt:
		imm = 6;
		break;
	}
	if constexpr (std::is_same_v<ElemT, float>) {
		cc.cmpps(a, b, ::asmjit::Imm(imm));
	} else {
		cc.cmppd(a, b, ::asmjit::Imm(imm));
	}
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

// Integer comparisons: combine pcmpeq and pcmpgt to synthesise every predicate.
template <int Width, FCmp Pred>
static bool handleCmpIntImpl(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto ones = cc.newXmm();
	cc.pcmpeqd(ones, ones); // all-1s for invert
	auto result = cc.newXmm();
	cc.movdqa(result, a);
	if constexpr (Width == 32) {
		switch (Pred) {
		case FCmp::Eq:
			cc.pcmpeqd(result, b);
			break;
		case FCmp::Ne:
			cc.pcmpeqd(result, b);
			cc.pxor(result, ones);
			break;
		case FCmp::Gt:
			cc.pcmpgtd(result, b);
			break;
		case FCmp::Le: { // !(a > b)
			cc.pcmpgtd(result, b);
			cc.pxor(result, ones);
			break;
		}
		case FCmp::Lt: { // b > a
			cc.movdqa(result, b);
			cc.pcmpgtd(result, a);
			break;
		}
		case FCmp::Ge: { // !(b > a)
			auto tmp = cc.newXmm();
			cc.movdqa(tmp, b);
			cc.pcmpgtd(tmp, a);
			cc.movdqa(result, tmp);
			cc.pxor(result, ones);
			break;
		}
		}
	} else {
		// 64-bit. pcmpeqq is SSE4.1, pcmpgtq is SSE4.2.
		switch (Pred) {
		case FCmp::Eq:
			cc.pcmpeqq(result, b);
			break;
		case FCmp::Ne:
			cc.pcmpeqq(result, b);
			cc.pxor(result, ones);
			break;
		case FCmp::Gt:
			cc.pcmpgtq(result, b);
			break;
		case FCmp::Le:
			cc.pcmpgtq(result, b);
			cc.pxor(result, ones);
			break;
		case FCmp::Lt: {
			cc.movdqa(result, b);
			cc.pcmpgtq(result, a);
			break;
		}
		case FCmp::Ge: {
			auto tmp = cc.newXmm();
			cc.movdqa(tmp, b);
			cc.pcmpgtq(tmp, a);
			cc.movdqa(result, tmp);
			cc.pxor(result, ones);
			break;
		}
		}
	}
	bindPtr(ctx, storeVecToStack(cc, result));
	return true;
}

template <typename ElemT, int N, FCmp Pred>
static bool handleCmpInt(IntrinsicCallContext& ctx) {
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		return handleCmpIntImpl<32, Pred>(ctx);
	} else {
		return handleCmpIntImpl<64, Pred>(ctx);
	}
}

// ── Bitwise on float (operate on the integer bit pattern) ───────────────────

enum class BitwiseOp { And, Or, Xor };

template <typename ElemT, int N, BitwiseOp Op>
static bool handleBitwise(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	if constexpr (Op == BitwiseOp::And)
		cc.pand(a, b);
	else if constexpr (Op == BitwiseOp::Or)
		cc.por(a, b);
	else
		cc.pxor(a, b);
	bindPtr(ctx, storeVecToStack(cc, a));
	return true;
}

// ── Broadcast (scalar → splat vector) ───────────────────────────────────────

template <typename ElemT, int N>
static bool handleBroadcast(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = cc.newXmm();
	if constexpr (std::is_same_v<ElemT, float>) {
		auto scalar = argXmm(ctx, 0);
		cc.movaps(v, scalar);
		cc.shufps(v, v, ::asmjit::Imm(0));
	} else if constexpr (std::is_same_v<ElemT, double>) {
		auto scalar = argXmm(ctx, 0);
		cc.movapd(v, scalar);
		cc.unpcklpd(v, v);
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto scalar = argGp(ctx, 0);
		cc.movd(v, scalar.r32());
		cc.pshufd(v, v, ::asmjit::Imm(0));
	} else {
		auto scalar = argGp(ctx, 0);
		cc.movq(v, scalar.r64());
		cc.punpcklqdq(v, v);
	}
	bindPtr(ctx, storeVecToStack(cc, v));
	return true;
}

// ── Extract (vector pointer, index) → scalar T ──────────────────────────────
//
// Index is a runtime i32 register, so we use the standard "spill to stack and
// index by element size" pattern. SSE has fixed-immediate pextr* only.

template <typename ElemT, int N>
static bool handleExtract(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto vecPtr = argGp(ctx, 0);
	auto idxGp = argGp(ctx, 1);
	auto idx64 = cc.newInt64();
	cc.movsxd(idx64, idxGp.r32());
	if constexpr (std::is_same_v<ElemT, float>) {
		auto out = cc.newXmmSs();
		cc.movss(out, x86::dword_ptr(vecPtr, idx64, 2)); // shift=2: *4 bytes
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else if constexpr (std::is_same_v<ElemT, double>) {
		auto out = cc.newXmmSd();
		cc.movsd(out, x86::qword_ptr(vecPtr, idx64, 3)); // shift=3: *8 bytes
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto out = cc.newInt64();
		auto tmp32 = cc.newInt32();
		cc.mov(tmp32, x86::dword_ptr(vecPtr, idx64, 2));
		cc.movsxd(out, tmp32);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else {
		auto out = cc.newInt64();
		cc.mov(out, x86::qword_ptr(vecPtr, idx64, 3));
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	}
	return true;
}

// ── Insert (vec ptr, scalar, index) → new vec ptr ───────────────────────────

template <typename ElemT, int N>
static bool handleInsert(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto srcVecPtr = argGp(ctx, 0);
	auto idxGp = argGp(ctx, 2);
	auto idx64 = cc.newInt64();
	cc.movsxd(idx64, idxGp.r32());

	// Copy source vector to a fresh stack slot, mutate the slot, return its
	// address. Same memory model as every other op (each call has its own
	// result slot).
	auto slot = cc.newStack(16, 16);
	auto vec = loadVec(cc, srcVecPtr);
	cc.movdqu(slot, vec);
	auto base = cc.newIntPtr();
	cc.lea(base, slot);

	if constexpr (std::is_same_v<ElemT, float>) {
		auto val = argXmm(ctx, 1);
		cc.movss(x86::dword_ptr(base, idx64, 2), val);
	} else if constexpr (std::is_same_v<ElemT, double>) {
		auto val = argXmm(ctx, 1);
		cc.movsd(x86::qword_ptr(base, idx64, 3), val);
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto val = argGp(ctx, 1);
		cc.mov(x86::dword_ptr(base, idx64, 2), val.r32());
	} else {
		auto val = argGp(ctx, 1);
		cc.mov(x86::qword_ptr(base, idx64, 3), val.r64());
	}
	bindPtr(ctx, base);
	return true;
}

// ── Gather (base ptr, indices ptr) → vec ptr ────────────────────────────────
//
// Unroll N lane-loads: load lane index → load element from base+idx*sizeof(T).
// SSE2-safe; AVX2's vpgatherdd would be faster but we stay 128-bit baseline.

template <typename ElemT, int N>
static bool handleGather(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto base = argGp(ctx, 0);
	auto indicesPtr = argGp(ctx, 1);

	auto slot = cc.newStack(16, 16);
	auto slotBase = cc.newIntPtr();
	cc.lea(slotBase, slot);

	constexpr int Shift = (sizeof(ElemT) == 1) ? 0 : (sizeof(ElemT) == 2) ? 1 : (sizeof(ElemT) == 4) ? 2 : 3;
	for (int i = 0; i < N; ++i) {
		auto idx = cc.newInt32();
		cc.mov(idx, x86::dword_ptr(indicesPtr, i * 4));
		auto idx64 = cc.newInt64();
		cc.movsxd(idx64, idx);
		if constexpr (std::is_same_v<ElemT, float>) {
			auto tmp = cc.newInt32();
			cc.mov(tmp, x86::dword_ptr(base, idx64, Shift));
			cc.mov(x86::dword_ptr(slotBase, i * 4), tmp);
		} else if constexpr (std::is_same_v<ElemT, double>) {
			auto tmp = cc.newInt64();
			cc.mov(tmp, x86::qword_ptr(base, idx64, Shift));
			cc.mov(x86::qword_ptr(slotBase, i * 8), tmp);
		} else if constexpr (std::is_same_v<ElemT, int32_t>) {
			auto tmp = cc.newInt32();
			cc.mov(tmp, x86::dword_ptr(base, idx64, Shift));
			cc.mov(x86::dword_ptr(slotBase, i * 4), tmp);
		} else {
			auto tmp = cc.newInt64();
			cc.mov(tmp, x86::qword_ptr(base, idx64, Shift));
			cc.mov(x86::qword_ptr(slotBase, i * 8), tmp);
		}
	}
	bindPtr(ctx, slotBase);
	return true;
}

// ── Scatter (dest ptr, indices ptr, vec ptr) → void ─────────────────────────

template <typename ElemT, int N>
static bool handleScatter(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto dest = argGp(ctx, 0);
	auto indicesPtr = argGp(ctx, 1);
	auto vecPtr = argGp(ctx, 2);

	constexpr int Shift = (sizeof(ElemT) == 1) ? 0 : (sizeof(ElemT) == 2) ? 1 : (sizeof(ElemT) == 4) ? 2 : 3;
	for (int i = 0; i < N; ++i) {
		auto idx = cc.newInt32();
		cc.mov(idx, x86::dword_ptr(indicesPtr, i * 4));
		auto idx64 = cc.newInt64();
		cc.movsxd(idx64, idx);
		if constexpr (sizeof(ElemT) == 4) {
			auto tmp = cc.newInt32();
			cc.mov(tmp, x86::dword_ptr(vecPtr, i * 4));
			cc.mov(x86::dword_ptr(dest, idx64, Shift), tmp);
		} else {
			auto tmp = cc.newInt64();
			cc.mov(tmp, x86::qword_ptr(vecPtr, i * 8));
			cc.mov(x86::qword_ptr(dest, idx64, Shift), tmp);
		}
	}
	return true;
}

// ── Blend (mask, a, b) → vec_ptr ────────────────────────────────────────────
//
// Scalar semantics: per lane `(mask[i] != 0) ? a[i] : b[i]`. We synthesise
// is_nonzero via `cmp = pcmpeq*(mask, 0)`, then result = (a & ~cmp) | (b & cmp).

template <typename ElemT, int N>
static bool handleBlend(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto mask = loadVec(cc, argGp(ctx, 0));
	auto a = loadVec(cc, argGp(ctx, 1));
	auto b = loadVec(cc, argGp(ctx, 2));
	auto zero = cc.newXmm();
	cc.pxor(zero, zero);
	auto cmp = cc.newXmm();
	cc.movdqa(cmp, mask);
	// pcmpeqd on 32-bit lanes works for any element width as long as zero
	// detection per element is consistent: if any 32-bit half of a 64-bit
	// lane is non-zero, the lane is non-zero. For our usage (masks come from
	// vector comparisons), every lane is all-0 or all-1s, so this is exact.
	cc.pcmpeqd(cmp, zero);
	// cmp = 1s where mask==0 → pick b; 0s where mask!=0 → pick a.
	auto tmp = cc.newXmm();
	cc.movdqa(tmp, cmp);
	cc.pandn(tmp, a); // tmp = (~cmp) & a
	cc.pand(cmp, b);  // cmp  = cmp & b
	cc.por(tmp, cmp); // tmp = result
	bindPtr(ctx, storeVecToStack(cc, tmp));
	return true;
}

#else // !x86_64

// ============================================================================
// AArch64 / NEON path
// ============================================================================

using namespace ::asmjit;
using namespace ::asmjit::a64;

// --- argument / result helpers ----------------------------------------------

static Gp argGp(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Gp>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

static Vec argVec(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Vec>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

// Returns a fresh full 128-bit Vec viewed as Q (vN.16b for raw access).
static Vec newQ(Compiler& cc) {
	return cc.newVecQ();
}

// View helpers — NEON instructions need lane-typed views (.4s, .2d, .16b).
template <typename ElemT, int N>
static Vec asView(const Vec& v) {
	if constexpr (std::is_same_v<ElemT, float>) {
		return v.s4();
	} else if constexpr (std::is_same_v<ElemT, double>) {
		return v.d2();
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		return v.s4();
	} else {
		return v.d2();
	}
}

static Vec loadVec(Compiler& cc, const Gp& ptr) {
	auto v = newQ(cc);
	cc.ldr(v, a64::ptr(ptr));
	return v;
}

static Gp storeVecToStack(Compiler& cc, const Vec& v) {
	auto slot = cc.newStack(16, 16);
	cc.str(v, slot);
	auto p = cc.newIntPtr();
	cc.loadAddressOf(p, slot);
	return p;
}

static void bindPtr(IntrinsicCallContext& ctx, const Gp& p) {
	ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {p});
}

// NEON load/store
template <typename ElemT, int N>
static bool handleLoad(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto srcPtr = argGp(ctx, 0);
	auto v = loadVec(cc, srcPtr);
	bindPtr(ctx, storeVecToStack(cc, v));
	return true;
}

template <typename ElemT, int N>
static bool handleStore(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto destPtr = argGp(ctx, 0);
	auto srcVecPtr = argGp(ctx, 1);
	auto v = loadVec(cc, srcVecPtr);
	cc.str(v, a64::ptr(destPtr));
	return true;
}

enum class FOp { Add, Sub, Mul, Div, Min, Max };
enum class IOp { Add, Sub, Mul, And, Or, Xor, Min, Max, Shl, Shr };

template <typename ElemT, int N, FOp Op>
static bool handleFloatBinary(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto r = newQ(cc);
	auto av = asView<ElemT, N>(a);
	auto bv = asView<ElemT, N>(b);
	auto rv = asView<ElemT, N>(r);
	if constexpr (Op == FOp::Add)
		cc.fadd(rv, av, bv);
	else if constexpr (Op == FOp::Sub)
		cc.fsub(rv, av, bv);
	else if constexpr (Op == FOp::Mul)
		cc.fmul(rv, av, bv);
	else if constexpr (Op == FOp::Div)
		cc.fdiv(rv, av, bv);
	else if constexpr (Op == FOp::Min)
		cc.fmin(rv, av, bv);
	else if constexpr (Op == FOp::Max)
		cc.fmax(rv, av, bv);
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N, IOp Op>
static bool handleIntBinary(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto r = newQ(cc);
	auto av = asView<ElemT, N>(a);
	auto bv = asView<ElemT, N>(b);
	auto rv = asView<ElemT, N>(r);
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		if constexpr (Op == IOp::Add)
			cc.add(rv, av, bv);
		else if constexpr (Op == IOp::Sub)
			cc.sub(rv, av, bv);
		else if constexpr (Op == IOp::Mul)
			cc.mul(rv, av, bv);
		else if constexpr (Op == IOp::And)
			cc.and_(r.b16(), a.b16(), b.b16());
		else if constexpr (Op == IOp::Or)
			cc.orr(r.b16(), a.b16(), b.b16());
		else if constexpr (Op == IOp::Xor)
			cc.eor(r.b16(), a.b16(), b.b16());
		else if constexpr (Op == IOp::Min)
			cc.smin(rv, av, bv);
		else if constexpr (Op == IOp::Max)
			cc.smax(rv, av, bv);
	} else {
		// int64
		if constexpr (Op == IOp::Add)
			cc.add(rv, av, bv);
		else if constexpr (Op == IOp::Sub)
			cc.sub(rv, av, bv);
		// NEON has no .2d mul / smin / smax → not registered for i64.
		else if constexpr (Op == IOp::And)
			cc.and_(r.b16(), a.b16(), b.b16());
		else if constexpr (Op == IOp::Or)
			cc.orr(r.b16(), a.b16(), b.b16());
		else if constexpr (Op == IOp::Xor)
			cc.eor(r.b16(), a.b16(), b.b16());
	}
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

// Variable shift: count comes as the lane-0 element of the second vector.
// NEON's by-vector shift treats the second operand element-wise; since the
// scalar fallback shifts every lane by the same amount, we broadcast lane 0
// of the count vector to all lanes first.
template <typename ElemT, int N, IOp Op>
static bool handleIntShift(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto count = loadVec(cc, argGp(ctx, 1));
	auto r = newQ(cc);
	auto rv = asView<ElemT, N>(r);
	auto av = asView<ElemT, N>(a);
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		// Broadcast lane 0 of count.4s across all lanes via dup.
		auto cnt = newQ(cc);
		cc.dup(cnt.s4(), count.s(0));
		if constexpr (Op == IOp::Shl)
			cc.sshl(rv, av, cnt.s4());
		else if constexpr (Op == IOp::Shr) {
			// Arithmetic right shift = negate count, then sshl.
			cc.neg(cnt.s4(), cnt.s4());
			cc.sshl(rv, av, cnt.s4());
		}
	} else {
		auto cnt = newQ(cc);
		cc.dup(cnt.d2(), count.d(0));
		if constexpr (Op == IOp::Shl)
			cc.sshl(rv, av, cnt.d2());
		else if constexpr (Op == IOp::Shr) {
			cc.neg(cnt.d2(), cnt.d2());
			cc.sshl(rv, av, cnt.d2());
		}
	}
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleAbsFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto r = newQ(cc);
	cc.fabs(asView<ElemT, N>(r), asView<ElemT, N>(a));
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleNegFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto r = newQ(cc);
	cc.fneg(asView<ElemT, N>(r), asView<ElemT, N>(a));
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleAbsInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto r = newQ(cc);
	cc.abs(asView<ElemT, N>(r), asView<ElemT, N>(a));
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleNegInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto r = newQ(cc);
	cc.neg(asView<ElemT, N>(r), asView<ElemT, N>(a));
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleFma(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto c = loadVec(cc, argGp(ctx, 2));
	// fmla: c += a * b. We want a*b + c, so copy c → r and fmla in-place.
	auto r = newQ(cc);
	cc.mov(r.b16(), c.b16());
	cc.fmla(asView<ElemT, N>(r), asView<ElemT, N>(a), asView<ElemT, N>(b));
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleReduceAddFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, float>) {
		// NEON has no `faddv` across-reduction; canonical pattern is two
		// pairwise adds: faddp .4s → .4s, then 2-operand faddp (scalar form).
		auto tmp = newQ(cc);
		cc.faddp(tmp.s4(), v.s4(), v.s4()); // [a+b, c+d, a+b, c+d]
		auto out = cc.newVecS();
		cc.faddp(out, tmp.s2()); // (a+b)+(c+d) — 2-operand scalar form
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else {
		auto out = cc.newVecD();
		cc.faddp(out, v.d2());
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	}
	return true;
}

template <typename ElemT, int N, bool IsMin>
static bool handleReduceMinMaxFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, float>) {
		auto out = cc.newVecS();
		if constexpr (IsMin)
			cc.fminv(out, v.s4());
		else
			cc.fmaxv(out, v.s4());
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
		return true;
	}
	// f64 has no fminv.2d → scalar fallback.
	(void) v;
	return false;
}

template <typename ElemT, int N>
static bool handleReduceAddInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	auto out = cc.newIntPtr(); // 64-bit Xn
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto acc = cc.newVecS();
		cc.addv(acc, v.s4());
		auto w = cc.newInt32();
		cc.fmov(w, acc); // umov w, acc.s[0]; fmov is the GP variant
		cc.sxtw(out, w);
	} else {
		auto e0 = cc.newInt64();
		auto e1 = cc.newInt64();
		cc.umov(e0, v.d(0));
		cc.umov(e1, v.d(1));
		cc.add(out, e0, e1);
	}
	ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	return true;
}

template <typename ElemT, int N, bool IsMin>
static bool handleReduceMinMaxInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto v = loadVec(cc, argGp(ctx, 0));
	if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto acc = cc.newVecS();
		if constexpr (IsMin)
			cc.sminv(acc, v.s4());
		else
			cc.smaxv(acc, v.s4());
		auto out = cc.newIntPtr();
		auto w = cc.newInt32();
		cc.fmov(w, acc);
		cc.sxtw(out, w);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
		return true;
	}
	// i64 has no sminv.2d → scalar fallback.
	(void) v;
	return false;
}

enum class FCmp { Eq, Ne, Lt, Le, Gt, Ge };

template <typename ElemT, int N, FCmp Pred>
static bool handleCmpFloat(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto r = newQ(cc);
	auto av = asView<ElemT, N>(a);
	auto bv = asView<ElemT, N>(b);
	auto rv = asView<ElemT, N>(r);
	switch (Pred) {
	case FCmp::Eq:
		cc.fcmeq(rv, av, bv);
		break;
	case FCmp::Ne:
		cc.fcmeq(rv, av, bv);
		cc.mvn(r.b16(), r.b16());
		break;
	case FCmp::Gt:
		cc.fcmgt(rv, av, bv);
		break;
	case FCmp::Ge:
		cc.fcmge(rv, av, bv);
		break;
	case FCmp::Lt:
		cc.fcmgt(rv, bv, av);
		break;
	case FCmp::Le:
		cc.fcmge(rv, bv, av);
		break;
	}
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N, FCmp Pred>
static bool handleCmpInt(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto r = newQ(cc);
	auto av = asView<ElemT, N>(a);
	auto bv = asView<ElemT, N>(b);
	auto rv = asView<ElemT, N>(r);
	switch (Pred) {
	case FCmp::Eq:
		cc.cmeq(rv, av, bv);
		break;
	case FCmp::Ne:
		cc.cmeq(rv, av, bv);
		cc.mvn(r.b16(), r.b16());
		break;
	case FCmp::Gt:
		cc.cmgt(rv, av, bv);
		break;
	case FCmp::Ge:
		cc.cmge(rv, av, bv);
		break;
	case FCmp::Lt:
		cc.cmgt(rv, bv, av);
		break;
	case FCmp::Le:
		cc.cmge(rv, bv, av);
		break;
	}
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

enum class BitwiseOp { And, Or, Xor };

template <typename ElemT, int N, BitwiseOp Op>
static bool handleBitwise(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto a = loadVec(cc, argGp(ctx, 0));
	auto b = loadVec(cc, argGp(ctx, 1));
	auto r = newQ(cc);
	if constexpr (Op == BitwiseOp::And)
		cc.and_(r.b16(), a.b16(), b.b16());
	else if constexpr (Op == BitwiseOp::Or)
		cc.orr(r.b16(), a.b16(), b.b16());
	else
		cc.eor(r.b16(), a.b16(), b.b16());
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleBroadcast(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto r = newQ(cc);
	if constexpr (std::is_same_v<ElemT, float>) {
		auto s = argVec(ctx, 0);
		cc.dup(r.s4(), s.s(0));
	} else if constexpr (std::is_same_v<ElemT, double>) {
		auto s = argVec(ctx, 0);
		cc.dup(r.d2(), s.d(0));
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto s = argGp(ctx, 0);
		cc.dup(r.s4(), s.w());
	} else {
		auto s = argGp(ctx, 0);
		cc.dup(r.d2(), s.x());
	}
	bindPtr(ctx, storeVecToStack(cc, r));
	return true;
}

template <typename ElemT, int N>
static bool handleExtract(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto vecPtr = argGp(ctx, 0);
	auto idxGp = argGp(ctx, 1);
	auto idx64 = cc.newIntPtr();
	cc.sxtw(idx64, idxGp.w());
	// Element-size shift for `[base, idx, LSL #shift]` mem operand.
	constexpr uint32_t Shift = (sizeof(ElemT) == 4) ? 2u : 3u;
	if constexpr (std::is_same_v<ElemT, float>) {
		auto out = cc.newVecS();
		cc.ldr(out, a64::ptr(vecPtr, idx64, ::asmjit::arm::lsl(Shift)));
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else if constexpr (std::is_same_v<ElemT, double>) {
		auto out = cc.newVecD();
		cc.ldr(out, a64::ptr(vecPtr, idx64, ::asmjit::arm::lsl(Shift)));
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto out = cc.newIntPtr();
		auto w = cc.newInt32();
		cc.ldr(w, a64::ptr(vecPtr, idx64, ::asmjit::arm::lsl(Shift)));
		cc.sxtw(out, w);
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	} else {
		auto out = cc.newIntPtr();
		cc.ldr(out, a64::ptr(vecPtr, idx64, ::asmjit::arm::lsl(Shift)));
		ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {out});
	}
	return true;
}

template <typename ElemT, int N>
static bool handleInsert(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto srcVecPtr = argGp(ctx, 0);
	auto idxGp = argGp(ctx, 2);
	auto idx64 = cc.newIntPtr();
	cc.sxtw(idx64, idxGp.w());

	auto slot = cc.newStack(16, 16);
	auto vec = loadVec(cc, srcVecPtr);
	cc.str(vec, slot);
	auto base = cc.newIntPtr();
	cc.loadAddressOf(base, slot);

	constexpr uint32_t Shift = (sizeof(ElemT) == 4) ? 2u : 3u;
	auto memAt = a64::ptr(base, idx64, ::asmjit::arm::lsl(Shift));
	if constexpr (std::is_same_v<ElemT, float>) {
		auto val = argVec(ctx, 1);
		cc.str(val.s(), memAt);
	} else if constexpr (std::is_same_v<ElemT, double>) {
		auto val = argVec(ctx, 1);
		cc.str(val.d(), memAt);
	} else if constexpr (std::is_same_v<ElemT, int32_t>) {
		auto val = argGp(ctx, 1);
		cc.str(val.w(), memAt);
	} else {
		auto val = argGp(ctx, 1);
		cc.str(val.x(), memAt);
	}
	bindPtr(ctx, base);
	return true;
}

template <typename ElemT, int N>
static bool handleGather(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto base = argGp(ctx, 0);
	auto indicesPtr = argGp(ctx, 1);

	auto slot = cc.newStack(16, 16);
	auto slotBase = cc.newIntPtr();
	cc.loadAddressOf(slotBase, slot);

	constexpr uint32_t Shift = (sizeof(ElemT) == 4) ? 2u : 3u;
	for (int i = 0; i < N; ++i) {
		auto idx = cc.newInt32();
		cc.ldr(idx, a64::ptr(indicesPtr, i * 4));
		auto idx64 = cc.newIntPtr();
		cc.sxtw(idx64, idx);
		auto srcMem = a64::ptr(base, idx64, ::asmjit::arm::lsl(Shift));
		if constexpr (sizeof(ElemT) == 4) {
			auto tmp = cc.newInt32();
			cc.ldr(tmp, srcMem);
			cc.str(tmp, a64::ptr(slotBase, i * 4));
		} else {
			auto tmp = cc.newInt64();
			cc.ldr(tmp, srcMem);
			cc.str(tmp, a64::ptr(slotBase, i * 8));
		}
	}
	bindPtr(ctx, slotBase);
	return true;
}

template <typename ElemT, int N>
static bool handleScatter(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto dest = argGp(ctx, 0);
	auto indicesPtr = argGp(ctx, 1);
	auto vecPtr = argGp(ctx, 2);

	constexpr uint32_t Shift = (sizeof(ElemT) == 4) ? 2u : 3u;
	for (int i = 0; i < N; ++i) {
		auto idx = cc.newInt32();
		cc.ldr(idx, a64::ptr(indicesPtr, i * 4));
		auto idx64 = cc.newIntPtr();
		cc.sxtw(idx64, idx);
		auto destMem = a64::ptr(dest, idx64, ::asmjit::arm::lsl(Shift));
		if constexpr (sizeof(ElemT) == 4) {
			auto tmp = cc.newInt32();
			cc.ldr(tmp, a64::ptr(vecPtr, i * 4));
			cc.str(tmp, destMem);
		} else {
			auto tmp = cc.newInt64();
			cc.ldr(tmp, a64::ptr(vecPtr, i * 8));
			cc.str(tmp, destMem);
		}
	}
	return true;
}

// Blend: scalar semantics (mask[i] != 0) ? a[i] : b[i]. Synthesise via
// cmeq-to-zero + BSL.
template <typename ElemT, int N>
static bool handleBlend(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto mask = loadVec(cc, argGp(ctx, 0));
	auto a = loadVec(cc, argGp(ctx, 1));
	auto b = loadVec(cc, argGp(ctx, 2));
	auto sel = newQ(cc);
	// sel = (mask != 0): cmeq mask vs 0 → 1s where ==0; mvn → 1s where !=0.
	if constexpr (std::is_same_v<ElemT, int32_t> || std::is_same_v<ElemT, float>) {
		cc.cmeq(sel.s4(), mask.s4(), 0);
	} else {
		cc.cmeq(sel.d2(), mask.d2(), 0);
	}
	cc.mvn(sel.b16(), sel.b16()); // 1s where mask != 0
	// BSL semantics: dst = (dst & a) | (~dst & b). Pick a where sel == 1s.
	cc.bsl(sel.b16(), a.b16(), b.b16());
	bindPtr(ctx, storeVecToStack(cc, sel));
	return true;
}

#endif // x86_64 / aarch64

// ============================================================================
// Plugin & registration (shared across arches)
// ============================================================================

class AsmJitVectorIntrinsicPlugin : public AsmJitIntrinsicPlugin {
public:
	void registerIntrinsics(AsmJitIntrinsicManager& manager) override;
	~AsmJitVectorIntrinsicPlugin() override = default;
};

// Macros to keep registration compact. Mirrors plugins/simd/src/MLIRVectorIntrinsics.cpp.
// clang-format off
#define REG_LOAD_STORE(MGR, T, N, SUFFIX)                                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_load_##SUFFIX##_impl), handleLoad<T, N>);              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_store_##SUFFIX##_impl), handleStore<T, N>);

#define REG_FLOAT_BINARY(MGR, T, N, SUFFIX)                                                                            \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_add_##SUFFIX##_impl),                                  \
	                 (handleFloatBinary<T, N, FOp::Add>));                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_sub_##SUFFIX##_impl),                                  \
	                 (handleFloatBinary<T, N, FOp::Sub>));                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_mul_##SUFFIX##_impl),                                  \
	                 (handleFloatBinary<T, N, FOp::Mul>));                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_div_##SUFFIX##_impl),                                  \
	                 (handleFloatBinary<T, N, FOp::Div>));

#define REG_INT_BINARY(MGR, T, N, SUFFIX, REG_MUL)                                                                     \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_add_##SUFFIX##_impl),                                  \
	                 (handleIntBinary<T, N, IOp::Add>));                                                                \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_sub_##SUFFIX##_impl),                                  \
	                 (handleIntBinary<T, N, IOp::Sub>));                                                                \
	REG_MUL  // i32 registers mul; i64 leaves it unregistered → scalar fallback.

#define REG_MUL_I32(MGR, T, N, SUFFIX)                                                                                 \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_mul_##SUFFIX##_impl),                                  \
	                 (handleIntBinary<T, N, IOp::Mul>));

#define REG_MINMAX_FLOAT(MGR, T, N, SUFFIX)                                                                            \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_min_##SUFFIX##_impl),                                  \
	                 (handleFloatBinary<T, N, FOp::Min>));                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_max_##SUFFIX##_impl),                                  \
	                 (handleFloatBinary<T, N, FOp::Max>));

#define REG_MINMAX_I32(MGR, T, N, SUFFIX)                                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_min_##SUFFIX##_impl),                                  \
	                 (handleIntBinary<T, N, IOp::Min>));                                                                \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_max_##SUFFIX##_impl),                                  \
	                 (handleIntBinary<T, N, IOp::Max>));

#define REG_ABS_NEG_FLOAT(MGR, T, N, SUFFIX)                                                                           \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_abs_##SUFFIX##_impl), handleAbsFloat<T, N>);           \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_neg_##SUFFIX##_impl), handleNegFloat<T, N>);

#define REG_ABS_NEG_INT(MGR, T, N, SUFFIX)                                                                             \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_abs_##SUFFIX##_impl), handleAbsInt<T, N>);             \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_neg_##SUFFIX##_impl), handleNegInt<T, N>);

#define REG_FMA(MGR, T, N, SUFFIX)                                                                                     \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_fma_##SUFFIX##_impl), handleFma<T, N>);

#define REG_REDUCE_FLOAT(MGR, T, N, SUFFIX)                                                                            \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_reduce_add_##SUFFIX##_impl),                           \
	                 handleReduceAddFloat<T, N>);                                                                       \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_reduce_min_##SUFFIX##_impl),                           \
	                 (handleReduceMinMaxFloat<T, N, true>));                                                            \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_reduce_max_##SUFFIX##_impl),                           \
	                 (handleReduceMinMaxFloat<T, N, false>));

#define REG_REDUCE_INT(MGR, T, N, SUFFIX)                                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_reduce_add_##SUFFIX##_impl), handleReduceAddInt<T, N>); \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_reduce_min_##SUFFIX##_impl),                           \
	                 (handleReduceMinMaxInt<T, N, true>));                                                              \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_reduce_max_##SUFFIX##_impl),                           \
	                 (handleReduceMinMaxInt<T, N, false>));

#define REG_CMP(MGR, T, N, SUFFIX, IS_FLOAT)                                                                           \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_eq_##SUFFIX##_impl),                                   \
	                 (handle##IS_FLOAT<T, N, FCmp::Eq>));                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_ne_##SUFFIX##_impl),                                   \
	                 (handle##IS_FLOAT<T, N, FCmp::Ne>));                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_lt_##SUFFIX##_impl),                                   \
	                 (handle##IS_FLOAT<T, N, FCmp::Lt>));                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_le_##SUFFIX##_impl),                                   \
	                 (handle##IS_FLOAT<T, N, FCmp::Le>));                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_gt_##SUFFIX##_impl),                                   \
	                 (handle##IS_FLOAT<T, N, FCmp::Gt>));                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_ge_##SUFFIX##_impl),                                   \
	                 (handle##IS_FLOAT<T, N, FCmp::Ge>));

#define REG_BITWISE(MGR, T, N, SUFFIX)                                                                                 \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_and_##SUFFIX##_impl),                                  \
	                 (handleBitwise<T, N, BitwiseOp::And>));                                                            \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_or_##SUFFIX##_impl),                                   \
	                 (handleBitwise<T, N, BitwiseOp::Or>));                                                             \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_xor_##SUFFIX##_impl),                                  \
	                 (handleBitwise<T, N, BitwiseOp::Xor>));

#define REG_BROADCAST(MGR, T, N, SUFFIX)                                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_broadcast_##SUFFIX##_impl), handleBroadcast<T, N>);

#define REG_EXTRACT_INSERT(MGR, T, N, SUFFIX)                                                                          \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_extract_##SUFFIX##_impl), handleExtract<T, N>);        \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_insert_##SUFFIX##_impl), handleInsert<T, N>);

#define REG_GATHER_SCATTER(MGR, T, N, SUFFIX)                                                                          \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_gather_##SUFFIX##_impl), handleGather<T, N>);          \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_scatter_##SUFFIX##_impl), handleScatter<T, N>);

#define REG_BLEND(MGR, T, N, SUFFIX)                                                                                   \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_blend_##SUFFIX##_impl), handleBlend<T, N>);

#define REG_SHIFT_I32(MGR, T, N, SUFFIX)                                                                               \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_shl_##SUFFIX##_impl),                                  \
	                 (handleIntShift<T, N, IOp::Shl>));                                                                 \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_shr_##SUFFIX##_impl),                                  \
	                 (handleIntShift<T, N, IOp::Shr>));

#define REG_SHIFT_I64_SHL(MGR, T, N, SUFFIX)                                                                           \
	MGR.addIntrinsic(reinterpret_cast<void*>(&::nautilus::vector_shl_##SUFFIX##_impl),                                  \
	                 (handleIntShift<T, N, IOp::Shl>));
// clang-format on

void AsmJitVectorIntrinsicPlugin::registerIntrinsics(AsmJitIntrinsicManager& m) {
	// f32x4 — full coverage.
	REG_LOAD_STORE(m, float, 4, f32x4);
	REG_FLOAT_BINARY(m, float, 4, f32x4);
	REG_MINMAX_FLOAT(m, float, 4, f32x4);
	REG_ABS_NEG_FLOAT(m, float, 4, f32x4);
	REG_FMA(m, float, 4, f32x4);
	REG_REDUCE_FLOAT(m, float, 4, f32x4);
	REG_CMP(m, float, 4, f32x4, CmpFloat);
	REG_BITWISE(m, float, 4, f32x4);
	REG_BROADCAST(m, float, 4, f32x4);
	REG_EXTRACT_INSERT(m, float, 4, f32x4);
	REG_GATHER_SCATTER(m, float, 4, f32x4);
	REG_BLEND(m, float, 4, f32x4);

	// f64x2.
	REG_LOAD_STORE(m, double, 2, f64x2);
	REG_FLOAT_BINARY(m, double, 2, f64x2);
	REG_MINMAX_FLOAT(m, double, 2, f64x2);
	REG_ABS_NEG_FLOAT(m, double, 2, f64x2);
	REG_FMA(m, double, 2, f64x2);
	REG_REDUCE_FLOAT(m, double, 2, f64x2);
	REG_CMP(m, double, 2, f64x2, CmpFloat);
	REG_BITWISE(m, double, 2, f64x2);
	REG_BROADCAST(m, double, 2, f64x2);
	REG_EXTRACT_INSERT(m, double, 2, f64x2);
	REG_GATHER_SCATTER(m, double, 2, f64x2);
	REG_BLEND(m, double, 2, f64x2);

	// i32x4 — full integer coverage (div left scalar).
	REG_LOAD_STORE(m, int32_t, 4, i32x4);
	REG_INT_BINARY(m, int32_t, 4, i32x4, REG_MUL_I32(m, int32_t, 4, i32x4));
	REG_MINMAX_I32(m, int32_t, 4, i32x4);
	REG_ABS_NEG_INT(m, int32_t, 4, i32x4);
	REG_REDUCE_INT(m, int32_t, 4, i32x4);
	REG_CMP(m, int32_t, 4, i32x4, CmpInt);
	REG_BITWISE(m, int32_t, 4, i32x4);
	REG_BROADCAST(m, int32_t, 4, i32x4);
	REG_EXTRACT_INSERT(m, int32_t, 4, i32x4);
	REG_GATHER_SCATTER(m, int32_t, 4, i32x4);
	REG_BLEND(m, int32_t, 4, i32x4);
	REG_SHIFT_I32(m, int32_t, 4, i32x4);

	// i64x2 — add/sub/bitwise/shl/cmp/broadcast/load/store/extract/insert.
	// mul/div/min/max/abs/reduce_min_max intentionally not registered.
	REG_LOAD_STORE(m, int64_t, 2, i64x2);
	REG_INT_BINARY(m, int64_t, 2, i64x2, /*no mul*/);
	REG_ABS_NEG_INT(m, int64_t, 2, i64x2);
	REG_REDUCE_INT(m, int64_t, 2, i64x2); // reduce_add only emits SIMD; min/max return false.
	REG_CMP(m, int64_t, 2, i64x2, CmpInt);
	REG_BITWISE(m, int64_t, 2, i64x2);
	REG_BROADCAST(m, int64_t, 2, i64x2);
	REG_EXTRACT_INSERT(m, int64_t, 2, i64x2);
	REG_GATHER_SCATTER(m, int64_t, 2, i64x2);
	REG_BLEND(m, int64_t, 2, i64x2);
	REG_SHIFT_I64_SHL(m, int64_t, 2, i64x2);
}

} // namespace

void RegisterAsmJitVectorIntrinsicPlugin() {
	AsmJitIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<AsmJitVectorIntrinsicPlugin>());
}

} // namespace nautilus::compiler::asmjit
