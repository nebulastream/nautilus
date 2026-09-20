// AsmJit prefetch intrinsic plugin. Mirrors plugins/simd/src/AsmJitVectorIntrinsics.cpp:
// intercepts nautilus_prefetch_* function-pointer ProxyCalls and
// replaces them with a single native prefetch instruction instead of a real
// call.
//
// x86-64: locality is mapped onto the cache-level prefetch hints
// (PREFETCHT0..2 / PREFETCHNTA for reads). Writes use PREFETCHW, which has no
// locality variants, when the host supports it and otherwise fall back to the
// read hint of the same locality. Prefetch opcodes never fault, so the
// mapping is safe on any host.
//
// AArch64: locality maps onto the PRFM instruction's cache-level/policy hint
// (PLDL1..3KEEP for reads expected to be reused, PLDL1STRM for a read used
// once, and the PST* variants for the write-intent equivalents).

#include "AsmJitPrefetchIntrinsics.hpp"
#include "nautilus/compiler/backends/amsjit/intrinsics/AsmJitBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "prefetch_impl.hpp"

namespace nautilus::compiler::asmjit {

enum class PrefetchMode {
	ReadNone,
	ReadLow,
	ReadModerate,
	ReadHigh,
	WriteNone,
	WriteLow,
	WriteModerate,
	WriteHigh,
};

namespace {

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__)

using namespace ::asmjit;
using namespace ::asmjit::x86;

Gp argGp(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Gp>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

/// PREFETCHW is missing on pre-Broadwell Intel cores, where the opcode decodes as a NOP and the hint would be
/// silently lost; on those hosts a write prefetch falls back to the read hint of the same locality (which still
/// pulls the line into cache, just not in an exclusive state). This mirrors what clang does for
/// `__builtin_prefetch(addr, 1, n)` without `-mprfchw`.
bool hostHasPrefetchW() {
	static const bool has = CpuInfo::host().features().x86().hasPREFETCHW();
	return has;
}

template <PrefetchMode Mode>
bool handlePrefetch(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto mem = x86::ptr(argGp(ctx, 0));
	constexpr bool is_write = Mode >= PrefetchMode::WriteNone;
	if constexpr (is_write) {
		if (hostHasPrefetchW()) {
			cc.prefetchw(mem);
			return true;
		}
	}
	// Read hints (also the write fallback). Locality maps onto the cache level: 3 -> T0 (all levels), 2 -> T1,
	// 1 -> T2, 0 -> NTA (minimise cache pollution).
	constexpr int locality = static_cast<int>(Mode) % 4;
	if constexpr (locality == 0) {
		cc.prefetchnta(mem);
	} else if constexpr (locality == 1) {
		cc.prefetcht2(mem);
	} else if constexpr (locality == 2) {
		cc.prefetcht1(mem);
	} else {
		cc.prefetcht0(mem);
	}
	return true;
}

#else // AArch64

using namespace ::asmjit;
using namespace ::asmjit::a64;

Gp argGp(IntrinsicCallContext& ctx, size_t idx) {
	return std::get<Gp>(ctx.frame.getValue(ctx.call->getInputArguments()[idx]->getIdentifier()));
}

template <PrefetchMode Mode>
bool handlePrefetch(IntrinsicCallContext& ctx) {
	auto& cc = ctx.cc;
	auto mem = a64::ptr(argGp(ctx, 0));
	if constexpr (Mode == PrefetchMode::ReadNone) {
		cc.prfm(Imm(Predicate::PRFOp::kPLDL1STRM), mem);
	} else if constexpr (Mode == PrefetchMode::ReadLow) {
		cc.prfm(Imm(Predicate::PRFOp::kPLDL3KEEP), mem);
	} else if constexpr (Mode == PrefetchMode::ReadModerate) {
		cc.prfm(Imm(Predicate::PRFOp::kPLDL2KEEP), mem);
	} else if constexpr (Mode == PrefetchMode::ReadHigh) {
		cc.prfm(Imm(Predicate::PRFOp::kPLDL1KEEP), mem);
	} else if constexpr (Mode == PrefetchMode::WriteNone) {
		cc.prfm(Imm(Predicate::PRFOp::kPSTL1STRM), mem);
	} else if constexpr (Mode == PrefetchMode::WriteLow) {
		cc.prfm(Imm(Predicate::PRFOp::kPSTL3KEEP), mem);
	} else if constexpr (Mode == PrefetchMode::WriteModerate) {
		cc.prfm(Imm(Predicate::PRFOp::kPSTL2KEEP), mem);
	} else {
		cc.prfm(Imm(Predicate::PRFOp::kPSTL1KEEP), mem);
	}
	return true;
}

#endif

} // namespace

class AsmJitPrefetchIntrinsicPlugin : public AsmJitIntrinsicPlugin {
public:
	void registerIntrinsics(AsmJitIntrinsicManager& manager) override;
	~AsmJitPrefetchIntrinsicPlugin() override = default;
};

void AsmJitPrefetchIntrinsicPlugin::registerIntrinsics(AsmJitIntrinsicManager& manager) {
	using namespace nautilus::builtin::detail;
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_none), handlePrefetch<PrefetchMode::ReadNone>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_low), handlePrefetch<PrefetchMode::ReadLow>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_moderate),
	                     handlePrefetch<PrefetchMode::ReadModerate>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_read_high), handlePrefetch<PrefetchMode::ReadHigh>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_none),
	                     handlePrefetch<PrefetchMode::WriteNone>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_low), handlePrefetch<PrefetchMode::WriteLow>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_moderate),
	                     handlePrefetch<PrefetchMode::WriteModerate>);
	manager.addIntrinsic(reinterpret_cast<void*>(&nautilus_prefetch_write_high),
	                     handlePrefetch<PrefetchMode::WriteHigh>);
}

void RegisterAsmJitPrefetchIntrinsicPlugin() {
	AsmJitIntrinsicPluginRegistry::instance().addPlugin(std::make_shared<AsmJitPrefetchIntrinsicPlugin>());
}

} // namespace nautilus::compiler::asmjit
