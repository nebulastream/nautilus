#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <cstring>
#include <exception>
#include <vector>

// Core VM state and frame handling shared by the interpreter dispatch skins
// (TBCInterpreter.cpp) and the copy-and-patch JIT helpers (jit/). Hoisted out
// of TBCInterpreter.cpp's anonymous namespace so stitched code's plain-ABI
// helpers can reuse the exact same frame logic.

namespace nautilus::compiler::tbc {

/// Per-thread VM state: one contiguous stack shared by all frames of all
/// nested invocations on this thread (reentrancy through external calls that
/// call back into TBC code just pushes further frames at `sp`).
///
/// Layout note: the three hot members come first and their offsets are part
/// of the stencil ABI — copy-and-patch stencils access `sp` through a
/// prefix-compatible POD mirror (jit/TBCStencilSource.cpp), verified by
/// static_asserts in jit/TBCStencilHelpers.cpp. Do not reorder.
struct VMContext {
	uint64_t* sp = nullptr;
	uint64_t* stackEnd = nullptr;
	const TBCProgram* prog = nullptr;
	std::vector<uint64_t> storage;
	/// Set by JIT helpers when an exception must not unwind through stitched
	/// frames (which carry no unwind info); rethrown by the entry shim.
	std::exception_ptr pendingException;
};

VMContext& tlsContext();

/// Push a frame for `fn` onto the VM stack: header (caller fp, return ip,
/// caller dst register), constant image, zeroed 16-aligned alloca area.
/// Returns the new frame pointer. Throws RuntimeException on stack overflow.
/// The `returnIp` header slot is opaque to this function: the interpreter
/// stores a bytecode ip, stitched code a native continuation address.
inline uint64_t* pushFrame(VMContext* ctx, const TBCFunction& fn, uint64_t* callerFp, const Instr* returnIp,
                           uint16_t dstReg) {
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
void doExtCall(const CallSite& site, void* target, uint64_t* fp, uint16_t dstReg);

/// Indirect call: the target register always holds a real native pointer
/// (internal functions hand out pre-compiled trampolines, TBCTrampoline.hpp),
/// so no internal/external discrimination is needed.
void doIndirectCall(const Instr& inst, uint64_t* fp, VMContext* ctx);

} // namespace nautilus::compiler::tbc
