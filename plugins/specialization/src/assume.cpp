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

void nautilus_assume_noalias_stub(void*, void*) {
	// This function is a stub for optimization purposes.
	// It does nothing at runtime.
}

void nautilus_assume_noalias(val<void*> a, val<void*> b) {
	invoke(nautilus_assume_noalias_stub, a, b);
}

} // namespace nautilus
