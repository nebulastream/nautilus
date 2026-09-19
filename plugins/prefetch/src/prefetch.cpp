#include "prefetch_impl.hpp"
#include <nautilus/function.hpp>
#include <nautilus/prefetch/prefetch.hpp>

// ============================================================================
// Portable fallback implementations.
//
// These are the bodies that run whenever a backend does not intercept the
// call with a native instruction (MLIR/AsmJit with intrinsics registered do;
// the C++, bytecode and interpreter paths all fall through to these). Each
// one just forwards to `__builtin_prefetch`, which GCC and Clang both lower
// to the target's native prefetch instruction (or drop entirely on targets
// without one) -- this is the same compiler builtin nautilus's supported
// toolchains (GCC 14+, Clang 19+) already guarantee.
// ============================================================================

namespace nautilus::detail {

extern "C" void nautilus_prefetch_read_none(const void* address) {
	__builtin_prefetch(address, 0, 0);
}

extern "C" void nautilus_prefetch_read_low(const void* address) {
	__builtin_prefetch(address, 0, 1);
}

extern "C" void nautilus_prefetch_read_moderate(const void* address) {
	__builtin_prefetch(address, 0, 2);
}

extern "C" void nautilus_prefetch_read_high(const void* address) {
	__builtin_prefetch(address, 0, 3);
}

extern "C" void nautilus_prefetch_write_none(const void* address) {
	__builtin_prefetch(address, 1, 0);
}

extern "C" void nautilus_prefetch_write_low(const void* address) {
	__builtin_prefetch(address, 1, 1);
}

extern "C" void nautilus_prefetch_write_moderate(const void* address) {
	__builtin_prefetch(address, 1, 2);
}

extern "C" void nautilus_prefetch_write_high(const void* address) {
	__builtin_prefetch(address, 1, 3);
}

} // namespace nautilus::detail

namespace nautilus {

void prefetch(val<const void*> address, PrefetchRW rw, PrefetchLocality locality) {
	using namespace detail;
	if (rw == PrefetchRW::Read) {
		switch (locality) {
		case PrefetchLocality::None:
			invoke<void, const void*>(nautilus_prefetch_read_none, address);
			return;
		case PrefetchLocality::Low:
			invoke<void, const void*>(nautilus_prefetch_read_low, address);
			return;
		case PrefetchLocality::Moderate:
			invoke<void, const void*>(nautilus_prefetch_read_moderate, address);
			return;
		case PrefetchLocality::High:
			invoke<void, const void*>(nautilus_prefetch_read_high, address);
			return;
		}
		return;
	}
	switch (locality) {
	case PrefetchLocality::None:
		invoke<void, const void*>(nautilus_prefetch_write_none, address);
		return;
	case PrefetchLocality::Low:
		invoke<void, const void*>(nautilus_prefetch_write_low, address);
		return;
	case PrefetchLocality::Moderate:
		invoke<void, const void*>(nautilus_prefetch_write_moderate, address);
		return;
	case PrefetchLocality::High:
		invoke<void, const void*>(nautilus_prefetch_write_high, address);
		return;
	}
}

} // namespace nautilus
