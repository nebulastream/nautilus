#pragma once

#include "nautilus/compiler/backends/tbc/TBCCode.hpp"
#include "nautilus/compiler/backends/tbc/TBCHandlers.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <cstdint>

namespace nautilus::compiler::tbc {

/// Zero-dependency typed call thunks for outgoing external calls.
///
/// dyncall's forward-call VM is iOS-safe (it builds call frames on a data
/// stack, no executable memory), but it is the tbc backend's last third-party
/// dependency and needs hand-written assembly per target. This path replaces
/// it with pre-compiled C++ callers: one tiny template instantiation per
/// signature shape (return kind x integer-arg count x fp-arg pattern), bound
/// to each call site once at backend-compile time.
///
/// The thunks pass all integer-class arguments (bool/ints/pointers) as
/// uint64_t and reorder them ahead of the fp-class arguments (f32/f64, kept
/// in their original relative order). That is ABI-sound on SysV x86-64 and
/// AAPCS64 (incl. Apple) because integer-class and fp-class register
/// assignment are independent sequences, and the caps in TBCCode.hpp keep
/// every reordered argument out of ABI-visible stack-layout differences (see
/// the comment on kThunkMaxIntArgs). Other ABIs (e.g. Windows x64, whose four
/// argument slots are shared between classes) are not supported: resolution
/// leaves the site on the dyncall path there.

/// True when the typed-thunk call path is ABI-sound on this target
/// (x86-64 SysV or AArch64 AAPCS64, incl. Apple).
bool thunkCallsSupported();

/// Bind `site.ext` to a pre-compiled thunk matching the site's signature.
/// Leaves `site.ext.fn == nullptr` when the signature exceeds the caps
/// (more than kThunkMaxIntArgs integer-class or kThunkMaxFpArgs fp-class
/// arguments) or the target ABI is unsupported.
void resolveExtCallThunk(CallSite& site);

/// Hot-path marshaller for a resolved site: load the argument slots per the
/// precomputed recipes, call the thunk, and normalize the result into the
/// destination register with the same conventions as the dyncall path.
inline void doThunkExtCall(const CallSite& site, void* target, uint64_t* fp, uint16_t dstReg) {
	const ResolvedThunk& thunk = site.ext;
	uint64_t intArgs[kThunkMaxIntArgs];
	uint64_t fpArgs[kThunkMaxFpArgs];
	for (uint8_t i = 0; i < thunk.numInt; ++i) {
		const auto [reg, load] = thunk.intLoads[i];
		switch (load) {
		case ArgLoad::Raw:
			intArgs[i] = fp[reg];
			break;
		case ArgLoad::SExt8:
			intArgs[i] = static_cast<uint64_t>(static_cast<int64_t>(readReg<int8_t>(fp, reg)));
			break;
		case ArgLoad::SExt16:
			intArgs[i] = static_cast<uint64_t>(static_cast<int64_t>(readReg<int16_t>(fp, reg)));
			break;
		case ArgLoad::SExt32:
			intArgs[i] = static_cast<uint64_t>(static_cast<int64_t>(readReg<int32_t>(fp, reg)));
			break;
		}
	}
	for (uint8_t i = 0; i < thunk.numFp; ++i) {
		fpArgs[i] = fp[thunk.fpRegs[i]];
	}
	const uint64_t raw = thunk.fn(target, intArgs, fpArgs);
	switch (site.returnType) {
	case Type::v:
		return;
	case Type::b:
		// Mask to the low byte: callees are only required to set the low byte
		// of a bool return register (same rule as the dyncall path).
		writeReg<bool>(fp, dstReg, (raw & 0xFF) != 0);
		return;
	case Type::i8:
	case Type::ui8:
		// Truncate to the declared width, then renormalize (zero-extend) into
		// the slot: the callee only guarantees the low bits of the return
		// register.
		writeReg<uint8_t>(fp, dstReg, static_cast<uint8_t>(raw));
		return;
	case Type::i16:
	case Type::ui16:
		writeReg<uint16_t>(fp, dstReg, static_cast<uint16_t>(raw));
		return;
	case Type::i32:
	case Type::ui32:
		writeReg<uint32_t>(fp, dstReg, static_cast<uint32_t>(raw));
		return;
	case Type::i64:
	case Type::ui64:
	case Type::ptr:
	case Type::f32:
	case Type::f64:
		// Full 64-bit slots; f32/f64 already arrive as normalized slot bit
		// patterns from the thunk.
		fp[dstReg] = raw;
		return;
	}
	throw RuntimeException("tbc: unsupported external call return type");
}

} // namespace nautilus::compiler::tbc
