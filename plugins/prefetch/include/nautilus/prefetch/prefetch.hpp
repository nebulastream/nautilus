#pragma once

#include <nautilus/val.hpp>
#include <nautilus/val_ptr.hpp>

namespace nautilus {

/// Whether a prefetched cache line is expected to be read or written next.
/// Mirrors the `rw` argument of GCC/Clang's `__builtin_prefetch`.
enum class PrefetchRW {
	Read,
	Write,
};

/// Temporal locality hint: how soon the prefetched address is expected to be
/// reused, from "touched once" (None) to "reused heavily" (High). Mirrors the
/// `locality` argument of `__builtin_prefetch` (0-3).
enum class PrefetchLocality {
	None,
	Low,
	Moderate,
	High,
};

/// Issues a memory-prefetch hint for `address`, asking the CPU to start
/// pulling the cache line containing it closer to the core before it is
/// actually accessed.
///
/// This is purely a performance hint: it never affects program correctness,
/// never faults (the x86 PREFETCH* and AArch64 PRFM instructions are defined
/// to behave as no-ops for an address the CPU cannot or should not prefetch,
/// including unmapped memory), and every nautilus backend has a way to
/// execute it. On the MLIR and AsmJit backends it lowers to a native
/// prefetch instruction; every other backend calls a portable
/// `__builtin_prefetch`-based fallback.
void prefetch(val<const void*> address, PrefetchRW rw = PrefetchRW::Read,
             PrefetchLocality locality = PrefetchLocality::High);

} // namespace nautilus
