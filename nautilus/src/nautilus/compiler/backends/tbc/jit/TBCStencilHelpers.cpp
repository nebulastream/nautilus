
#include "nautilus/compiler/backends/tbc/TBCVm.hpp"
#include <cstddef>
#include <exception>

// Plain-ABI helpers called from stitched copy-and-patch code (declared in
// jit/TBCStencilSource.cpp). Stitched frames carry no unwind information, so
// a C++ exception must never propagate through them: every helper catches
// into VMContext::pendingException and reports failure through its return
// value; the calling stencil then exits via the @UNWIND stencil and the entry
// shim (invokeJit) rethrows.

namespace nautilus::compiler::tbc {

// The stencil source accesses VMContext through a prefix-compatible POD
// (VMContextPrefix { uint64_t* sp; }); pin the layout it assumes.
static_assert(offsetof(VMContext, sp) == 0, "stencil ABI: VMContext::sp must be the first member");
static_assert(offsetof(VMContext, stackEnd) == sizeof(void*), "stencil ABI: VMContext layout changed");

extern "C" uint64_t* tbcJitPushFrame(VMContext* ctx, const void* callee, const void* site, uint64_t* callerFp,
                                     void* nativeReturnAddress, uint64_t dstRegRaw) noexcept {
	try {
		const auto& fn = *static_cast<const TBCFunction*>(callee);
		const auto& callSite = *static_cast<const CallSite*>(site);
		uint64_t* fp = pushFrame(ctx, fn, callerFp, static_cast<const Instr*>(nativeReturnAddress),
		                         static_cast<uint16_t>(dstRegRaw));
		for (size_t i = 0; i < callSite.argRegs.size(); ++i) {
			fp[fn.argRegs[i]] = callerFp[callSite.argRegs[i]];
		}
		return fp;
	} catch (...) {
		ctx->pendingException = std::current_exception();
		return nullptr;
	}
}

extern "C" uint64_t tbcJitExtCall(VMContext* ctx, const void* site, uint64_t* fp, uint64_t dstRegRaw) noexcept {
	try {
		const auto& callSite = *static_cast<const CallSite*>(site);
		doExtCall(callSite, callSite.target, fp, static_cast<uint16_t>(dstRegRaw));
		return 1;
	} catch (...) {
		ctx->pendingException = std::current_exception();
		return 0;
	}
}

extern "C" uint64_t tbcJitIndCall(VMContext* ctx, const void* site, uint64_t* fp, uint64_t dstRegRaw,
                                  void* target) noexcept {
	try {
		const auto& callSite = *static_cast<const CallSite*>(site);
		doExtCall(callSite, target, fp, static_cast<uint16_t>(dstRegRaw));
		return 1;
	} catch (...) {
		ctx->pendingException = std::current_exception();
		return 0;
	}
}

} // namespace nautilus::compiler::tbc
