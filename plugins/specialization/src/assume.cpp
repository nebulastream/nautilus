#include "nautilus/specialization/assume.hpp"
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

} // namespace nautilus
