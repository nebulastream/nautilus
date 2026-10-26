#include "nautilus/specialization/assume.hpp"
#include "nautilus/extensions/IntrinsicRegistration.hpp"
#include "nautilus/function.hpp"

namespace nautilus {

void nautlis_assume_stub(bool) {
	// This function is a stub for optimization purposes.
	// It does nothing at runtime.
}

void nautilus_assume(val<bool> condition) {
	invoke(nautlis_assume_stub, condition);
}

void nautilus_assume_aligned_stub(void*, int) {
	// This function is a stub for optimization purposes.
	// It does nothing at runtime.
}

void nautilus_assume_aligned(val<void*> ptr, int alignment) {
	invoke(nautilus_assume_aligned_stub, ptr, val<int8_t>(alignment));
}

void nautilus_assume_constant_stub(int64_t, int64_t) {
	// Stub for optimization purposes; does nothing at runtime.
}

void nautilus_assume_constant(val<int64_t> value, int64_t expected_value) {
	invoke(nautilus_assume_constant_stub, value, val<int64_t>(expected_value));
}

void nautilus_assume_range_stub(int64_t, int64_t, int64_t) {
	// Stub for optimization purposes; does nothing at runtime.
}

void nautilus_assume_range(val<int64_t> value, int64_t lo, int64_t hi) {
	invoke(nautilus_assume_range_stub, value, val<int64_t>(lo), val<int64_t>(hi));
}

void nautilus_assume_nonzero_stub(int64_t) {
	// Stub for optimization purposes; does nothing at runtime.
}

void nautilus_assume_nonzero(val<int64_t> value) {
	invoke(nautilus_assume_nonzero_stub, value);
}

void nautilus_assume_separate_storage_stub(void*, void*) {
	// This function is a stub for optimization purposes.
	// It does nothing at runtime.
}

void nautilus_assume_noalias(val<void*> a, val<void*> b) {
	invoke(nautilus_assume_separate_storage_stub, a, b);
}

} // namespace nautilus

// The backend-neutral registration (04 §3.5). An assumption is not removable: it has no memory effect but its
// condition is information for the optimizer, so the attributes promise only that it returns and does not throw.
namespace {

struct AssumeIntrinsicRegistrar {
	AssumeIntrinsicRegistrar() {
		using namespace nautilus::ext;
		constexpr uint32_t attrs = AttrNoUnwind | AttrWillReturn;
		registerIntrinsicFor("specialization.assume", &nautilus::nautlis_assume_stub, attrs);
		registerIntrinsicFor("specialization.assume_aligned", &nautilus::nautilus_assume_aligned_stub, attrs);
		registerIntrinsicFor("specialization.assume_constant", &nautilus::nautilus_assume_constant_stub, attrs);
		registerIntrinsicFor("specialization.assume_range", &nautilus::nautilus_assume_range_stub, attrs);
		registerIntrinsicFor("specialization.assume_nonzero", &nautilus::nautilus_assume_nonzero_stub, attrs);
		registerIntrinsicFor("specialization.assume_separate_storage", &nautilus::nautilus_assume_separate_storage_stub,
		                     attrs);
	}
};
const AssumeIntrinsicRegistrar assumeRegistrar_;

} // namespace
