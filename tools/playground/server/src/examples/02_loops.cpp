// title: Loops
// description: Tracing follows loops symbolically — see how a for-loop becomes blocks and branches.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int64_t>(val<int32_t>)>("factorial", [](val<int32_t> n) {
		val<int64_t> result = 1;
		for (val<int32_t> i = 1; i <= n; i++) {
			result = result * i;
		}
		return result;
	});

	m.registerFunction<val<int64_t>(val<int32_t>)>("sumOfSquares", [](val<int32_t> n) {
		val<int64_t> sum = 0;
		for (val<int32_t> i = 0; i < n; i++) {
			sum = sum + val<int64_t>(i) * val<int64_t>(i);
		}
		return sum;
	});
}
