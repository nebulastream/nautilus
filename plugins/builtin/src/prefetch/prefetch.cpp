#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include "prefetch_impl.hpp"
#include <nautilus/builtin/prefetch/prefetch.hpp>
#include <nautilus/function.hpp>

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

namespace nautilus::builtin::detail {

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

} // namespace nautilus::builtin::detail

namespace nautilus::builtin {

void prefetch(val<const void*> address, PrefetchRW rw, PrefetchLocality locality) {
	using PrefetchFn = void (*)(const void*);
	// Indexed by [PrefetchRW][PrefetchLocality]. The (rw, locality) pair is
	// resolved to a concrete function pointer at trace time; that pointer
	// identity is what the backend intrinsic plugins key their lowering on.
	static constexpr PrefetchFn table[2][4] = {
	    {detail::nautilus_prefetch_read_none, detail::nautilus_prefetch_read_low,
	     detail::nautilus_prefetch_read_moderate, detail::nautilus_prefetch_read_high},
	    {detail::nautilus_prefetch_write_none, detail::nautilus_prefetch_write_low,
	     detail::nautilus_prefetch_write_moderate, detail::nautilus_prefetch_write_high},
	};
	invoke<void, const void*>(table[static_cast<size_t>(rw)][static_cast<size_t>(locality)], address);
}

} // namespace nautilus::builtin

// The backend-neutral registration (04 §3.5): the intrinsics exist in every build, whether or not a backend lowers
// them. It lives in this file because the registrar is linked whenever `prefetch` is.
namespace {

struct PrefetchIntrinsicRegistrar {
	PrefetchIntrinsicRegistrar() {
		using namespace nautilus::ext;
		// a prefetch hint reads and writes no program-visible memory and cannot throw
		constexpr uint32_t attrs =
		    AttrNoArgRead | AttrNoArgWrite | AttrNoOtherRead | AttrNoOtherWrite | AttrNoUnwind | AttrWillReturn;
		using namespace nautilus::builtin::detail;
		registerIntrinsicFor("builtin.prefetch_read_none", &nautilus_prefetch_read_none, attrs);
		registerIntrinsicFor("builtin.prefetch_read_low", &nautilus_prefetch_read_low, attrs);
		registerIntrinsicFor("builtin.prefetch_read_moderate", &nautilus_prefetch_read_moderate, attrs);
		registerIntrinsicFor("builtin.prefetch_read_high", &nautilus_prefetch_read_high, attrs);
		registerIntrinsicFor("builtin.prefetch_write_none", &nautilus_prefetch_write_none, attrs);
		registerIntrinsicFor("builtin.prefetch_write_low", &nautilus_prefetch_write_low, attrs);
		registerIntrinsicFor("builtin.prefetch_write_moderate", &nautilus_prefetch_write_moderate, attrs);
		registerIntrinsicFor("builtin.prefetch_write_high", &nautilus_prefetch_write_high, attrs);
	}
};
const PrefetchIntrinsicRegistrar prefetchRegistrar_;

} // namespace
