// Smoke fixture: minimal multi-function module.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("add",
	                                                             [](val<int32_t> a, val<int32_t> b) { return a + b; });

	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("mul",
	                                                             [](val<int32_t> a, val<int32_t> b) { return a * b; });
}
