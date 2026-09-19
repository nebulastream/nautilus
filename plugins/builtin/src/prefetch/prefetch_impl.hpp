#pragma once

// Forward declarations for the extern "C" prefetch implementation functions.
// There is one function per (PrefetchRW, PrefetchLocality) combination since
// the combination must be resolved to a concrete function pointer at trace
// time -- that pointer identity is what the MLIR and AsmJit intrinsic
// plugins key their lowering on (see MLIRPrefetchIntrinsics.cpp and
// AsmJitPrefetchIntrinsics.cpp). Defined in prefetch.cpp.

namespace nautilus::detail {

extern "C" void nautilus_prefetch_read_none(const void* address);
extern "C" void nautilus_prefetch_read_low(const void* address);
extern "C" void nautilus_prefetch_read_moderate(const void* address);
extern "C" void nautilus_prefetch_read_high(const void* address);
extern "C" void nautilus_prefetch_write_none(const void* address);
extern "C" void nautilus_prefetch_write_low(const void* address);
extern "C" void nautilus_prefetch_write_moderate(const void* address);
extern "C" void nautilus_prefetch_write_high(const void* address);

} // namespace nautilus::detail
