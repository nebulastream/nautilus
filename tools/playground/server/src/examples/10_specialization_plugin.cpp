// title: Specialization plugin: assume hints
// description: nautilus_assume() plants optimizer invariants — compare the plain and hinted kernels in the LLVM IR tabs.
#include <nautilus/Engine.hpp>
#include <nautilus/specialization/assume.hpp> // nautilus-specialization plugin
#include <nautilus/val.hpp>

using namespace nautilus;

// The same kernel twice: once plain, once with promises to the optimizer.
//
//   nautilus_assume(cond)                 cond is true here (UB otherwise)
//   nautilus_assume_aligned(ptr, align)   ptr is align-byte aligned
//
// Both functions land in one module, so the backend tabs contain both —
// scroll the "LLVM IR (optimized)" tab and compare how the hinted variant
// is laid out (the hints only take effect with the MLIR backend).
void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<bool*>, val<int32_t*>)>(
	    "sumPlain", [](val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
		    val<int32_t> sum = 0;
		    for (val<int32_t> i = 0; i < size; i++) {
			    if (mask[i]) {
				    sum += array[i];
			    }
		    }
		    return sum;
	    });

	m.registerFunction<val<int32_t>(val<int32_t>, val<bool*>, val<int32_t*>)>(
	    "sumHinted", [](val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
		    // Promise: the loop body executes at least once.
		    nautilus_assume(size > 0);
		    // Promise: both buffers are 32-byte aligned (the caller must
		    // uphold this, e.g. via std::aligned_alloc(32, ...)).
		    nautilus_assume_aligned((val<void*>) mask, 32);
		    nautilus_assume_aligned((val<void*>) array, 32);

		    val<int32_t> sum = 0;
		    for (val<int32_t> i = 0; i < size; i++) {
			    if (mask[i]) {
				    sum += array[i];
			    }
		    }
		    return sum;
	    });
}
