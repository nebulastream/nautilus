// title: Runtime calls with invoke()
// description: invoke() calls a plain C++ function from traced code as an opaque call op.
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

// Plain C++ — NOT traced. The compiled code calls it through the C ABI.
static bool isPrime(int32_t n) {
	if (n < 2) {
		return false;
	}
	for (int32_t d = 2; d * d <= n; d++) {
		if (n % d == 0) {
			return false;
		}
	}
	return true;
}

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>)>("sumOfPrimesBelow", [](val<int32_t> n) {
		val<int32_t> sum = 0;
		for (val<int32_t> i = 2; i < n; i++) {
			if (invoke(isPrime, i)) {
				sum += i;
			}
		}
		return sum;
	});
}
