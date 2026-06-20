#pragma once

#include <nautilus/Engine.hpp>
#include <nautilus/specialization/assume.hpp>

namespace nautilus::engine {

// Test function that uses nautilus_assume
val<int32_t> assumeFunction(val<int32_t> x) {
	val<bool> condition = x > 0;
	nautilus_assume(condition);
	return x + 1;
}

// Test function that uses nautilus_assume with a more complex condition
val<int32_t> assumeComplexCondition(val<int32_t> x, val<int32_t> y) {
	val<bool> condition = (x > 0) && (y > 0);
	nautilus_assume(condition);
	return x + y;
}

// Test function that uses nautilus_assume_aligned
val<int32_t> assumeAlignedFunction(val<int32_t*> ptr) {
	nautilus_assume_aligned((val<void*>) ptr, 16);
	return *ptr;
}

// Test function that uses nautilus_assume_noalias. Because `a` and `b` are
// assumed not to alias, the store through `b` cannot clobber the value stored
// through `a`, so the final load of `*a` can be folded to the just-stored
// constant instead of being reloaded from memory.
val<int32_t> assumeNoaliasFunction(val<int32_t*> a, val<int32_t*> b) {
	nautilus_assume_noalias(a, b);
	*a = 1;
	*b = 2;
	return *a;
}

} // namespace nautilus::engine
