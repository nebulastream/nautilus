// Smoke fixture: exception handling — a call that is not noexcept must reach
// the backends as an exception-handling call site with a landing pad.
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/val.hpp>
#include <nautilus/val_std.hpp>
#include <stdexcept>

using namespace nautilus;

struct Guard {
	int32_t value = 0;
	~Guard() noexcept {
	}
};

void store(Guard* guard, int32_t value) noexcept {
	guard->value = value;
}

int32_t checkedDivide(int32_t a, int32_t b) {
	if (b == 0) {
		throw std::domain_error("division by zero");
	}
	return a / b;
}

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("divideWithCleanup",
	                                                             [](val<int32_t> a, val<int32_t> b) {
		                                                             val<Guard> guard;
		                                                             invoke(store, &guard, a);
		                                                             val<int32_t> q = invoke(checkedDivide, a, b);
		                                                             return q + guard.get(&Guard::value);
	                                                             });
}
