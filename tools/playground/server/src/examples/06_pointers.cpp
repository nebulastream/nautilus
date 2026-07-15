// title: Pointers & memory
// description: val<T*> traces loads and stores — the classic conditional aggregation kernel from the Nautilus paper.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_ptr.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<bool*>, val<int32_t*>)>(
	    "conditionalSum", [](val<int32_t> size, val<bool*> mask, val<int32_t*> array) {
		    val<int32_t> sum = 0;
		    for (val<int32_t> i = 0; i < size; i++) {
			    if (mask[i]) {
				    sum += array[i];
			    }
		    }
		    return sum;
	    });
}
