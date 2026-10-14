// AsmJit popcount intrinsic plugin. Mirrors prefetch/AsmJitPrefetchIntrinsics.cpp:
// intercepts nautilus_popcount_* function-pointer ProxyCalls and replaces them
// with a native population count instead of a real call.
//
// Intrinsic handlers receive canonical operands: a narrow integer is sign- or
// zero-extended to 64 bits according to its stamp. Unsigned operands can
// therefore be counted at full width, while signed operands narrower than 64
// bits are first zero-extended from their own width so the sign-extension bits
// are not counted. The result lies in [0, 64], so its 64-bit zero-extension is
// canonical for every result type.
//
// x86-64: POPCNT, when the host supports it; otherwise the handler declines and
// the call falls back to the portable implementation.
//
// AArch64: the base ISA has no scalar population count, so the value is moved
// to a SIMD register, counted per byte with CNT, summed with ADDV, and moved
// back -- the same sequence clang emits for `__builtin_popcountll`.

#include "AsmJitPopcountIntrinsics.hpp"
#include "nautilus/compiler/backends/asmjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "popcount_impl.hpp"

namespace nautilus::compiler::asmjit {

namespace {

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)

using namespace ::asmjit;
using namespace ::asmjit::x86;

Gp argGp(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Gp>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

/// POPCNT is part of x86-64-v2 and present on every x86-64 CPU since ~2008, but
/// it is not baseline x86-64; executing it on a host without it would fault.
bool hostHasPopcnt() {
	static const bool has = CpuInfo::host().features().x86().hasPOPCNT();
	return has;
}

template <uint32_t Bits, bool Signed>
bool handlePopcount(IntrinsicCallContext& ctx) {
	if (!hostHasPopcnt()) {
		return false;
	}
	auto& cc = ctx.cc;
	auto src = argGp(ctx, 0);
	auto dst = cc.newUInt64();
	if constexpr (!Signed || Bits == 64) {
		// Zero-extended (or full-width) operand: the bits above Bits are all 0.
		cc.popcnt(dst, src.r64());
	} else if constexpr (Bits == 32) {
		// The 32-bit form writes the count zero-extended to 64 bits.
		cc.popcnt(dst.r32(), src.r32());
	} else {
		auto narrow = cc.newUInt32();
		if constexpr (Bits == 8) {
			cc.movzx(narrow, src.r8());
		} else {
			cc.movzx(narrow, src.r16());
		}
		cc.popcnt(dst.r32(), narrow);
	}
	ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {dst});
	return true;
}

#else // AArch64

using namespace ::asmjit;
using namespace ::asmjit::a64;

Gp argGp(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Gp>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

template <uint32_t Bits, bool Signed>
bool handlePopcount(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto src = argGp(ctx, 0);
	auto in = src;
	if constexpr (Signed && Bits < 64) {
		// Writing a W register zeroes the upper 32 bits of the X register.
		in = cc.newUInt64();
		if constexpr (Bits == 8) {
			cc.uxtb(in.w(), src.w());
		} else if constexpr (Bits == 16) {
			cc.uxth(in.w(), src.w());
		} else {
			cc.mov(in.w(), src.w());
		}
	}
	auto v = cc.newVecD();
	cc.fmov(v.d(), in.x());
	cc.cnt(v.b8(), v.b8());
	// A scalar write to B<n> zeroes the rest of the vector register, so the S
	// view holds the zero-extended byte sum.
	cc.addv(v.b(), v.b8());
	auto dst = cc.newUInt64();
	cc.fmov(dst.w(), v.s());
	ctx.frame.setValue(ctx.call->getIdentifier(), AsmReg {dst});
	return true;
}

#endif

} // namespace

class AsmJitPopcountIntrinsicPlugin : public AsmJitIntrinsicPlugin {
public:
	void registerIntrinsics(AsmJitIntrinsicManager& manager) override;
	~AsmJitPopcountIntrinsicPlugin() override = default;
};

void AsmJitPopcountIntrinsicPlugin::registerIntrinsics(AsmJitIntrinsicManager& manager) {
	using namespace nautilus::builtin::detail;
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i8), handlePopcount<8, true>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i16), handlePopcount<16, true>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i32), handlePopcount<32, true>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_i64), handlePopcount<64, true>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u8), handlePopcount<8, false>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u16), handlePopcount<16, false>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u32), handlePopcount<32, false>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_popcount_u64), handlePopcount<64, false>);
}

void RegisterAsmJitPopcountIntrinsicPlugin() {
	AsmJitIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<AsmJitPopcountIntrinsicPlugin>());
}

} // namespace nautilus::compiler::asmjit
