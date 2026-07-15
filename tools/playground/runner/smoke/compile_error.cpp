// Negative smoke fixture: must fail to compile (exit 10) with a clang
// diagnostic pointing at user_module.cpp.
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>)>("broken", [](val<int32_t> x) {
		return x + undeclared_variable; // error: use of undeclared identifier
	});
}
