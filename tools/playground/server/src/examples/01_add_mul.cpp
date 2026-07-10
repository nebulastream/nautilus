// title: Add & multiply
// description: The smallest possible module: two arithmetic functions traced into one compilation unit.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

// Register one or more functions on the module. Lambdas need an explicit
// val<>-typed signature: registerFunction<val<R>(val<Args>...)>(name, lambda).
void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("add",
	                                                             [](val<int32_t> a, val<int32_t> b) { return a + b; });

	m.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>("mul",
	                                                             [](val<int32_t> a, val<int32_t> b) { return a * b; });
}
