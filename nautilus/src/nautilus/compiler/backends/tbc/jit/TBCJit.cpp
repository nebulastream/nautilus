
#include "nautilus/compiler/backends/tbc/jit/TBCJit.hpp"
#include "nautilus/compiler/backends/tbc/jit/TBCStencils.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>

// Entry path into stitched copy-and-patch code. Mirrors tbc::invoke
// (TBCInterpreter.cpp): same per-thread VM stack setup and save/restore; the
// difference is the entry frame's return slot holds the @EPILOGUE stencil's
// native address instead of the HALT bytecode ip, and control enters the
// function's stitched entry through a preserve_none call instead of a
// dispatch loop.

// Executing stitched code requires calling through a preserve_none function
// pointer: Clang 19+ only, and not under ASan (same restriction as the
// interpreter's tail-call skin — LLVM issue #95928).
#if defined(__has_feature)
#if __has_feature(address_sanitizer)
#define TBC_JIT_ASAN 1
#endif
#endif
#if !defined(TBC_JIT_ASAN) && defined(__SANITIZE_ADDRESS__)
#define TBC_JIT_ASAN 1
#endif

#if defined(__clang__) && defined(__has_attribute) && !defined(TBC_JIT_ASAN)
#if __has_attribute(preserve_none)
#define TBC_JIT_CAN_EXECUTE 1
#endif
#endif

namespace nautilus::compiler::tbc::jit {

bool jitRuntimeAvailable() {
#ifdef TBC_JIT_CAN_EXECUTE
	return resolvedStencils().valid;
#else
	return false;
#endif
}

#ifdef TBC_JIT_CAN_EXECUTE

namespace {
using JitHandler = uint64_t(__attribute__((preserve_none))*)(uint64_t*, VMContext*);
}

uint64_t invokeJit(const TBCProgram& program, uint32_t functionIndex, const uint64_t* args, size_t argCount) {
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
	uint64_t* fp = pushFrame(&ctx, fn, &resultSlot, static_cast<const Instr*>(program.jit->epilogue), 0);
	const size_t argsToCopy = std::min(argCount, fn.argRegs.size());
	for (size_t i = 0; i < argsToCopy; ++i) {
		fp[fn.argRegs[i]] = args[i];
	}

	const auto entry = reinterpret_cast<JitHandler>(program.jit->entries[functionIndex]);
	const uint64_t result = entry(fp, &ctx);
	if (ctx.pendingException) {
		auto pending = ctx.pendingException;
		ctx.pendingException = nullptr;
		std::rethrow_exception(pending);
	}
	return result;
}

#else // !TBC_JIT_CAN_EXECUTE

uint64_t invokeJit(const TBCProgram&, uint32_t, const uint64_t*, size_t) {
	throw RuntimeException("tbc-jit: this build cannot execute stitched code");
}

#endif

} // namespace nautilus::compiler::tbc::jit
