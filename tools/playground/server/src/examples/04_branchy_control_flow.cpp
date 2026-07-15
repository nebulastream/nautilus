// title: Branchy control flow
// description: Data-dependent branches and nested conditions — a good input for the CFG graph view.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("clampedDiff", [](val<int32_t> a, val<int32_t> b) {
		val<int32_t> diff = 0;
		if (a > b) {
			diff = a - b;
		} else {
			diff = b - a;
		}
		if (diff > 100) {
			diff = 100;
		}
		return diff;
	});

	m.registerFunction<val<int32_t>(val<int32_t>)>("collatzSteps", [](val<int32_t> n) {
		val<int32_t> steps = 0;
		while (n > 1) {
			if (n % 2 == 0) {
				n = n / 2;
			} else {
				n = 3 * n + 1;
			}
			steps = steps + 1;
		}
		return steps;
	});
}
