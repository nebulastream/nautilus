// Negative smoke fixture: tracing executes the function body once, and this
// loop never terminates — the run-phase timeout must kill it (exit 12).
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

void playground_register(engine::NautilusModule& m) {
	m.registerFunction<val<int32_t>(val<int32_t>)>("spin", [](val<int32_t> x) {
		volatile bool forever = true;
		while (forever) {
		}
		return x;
	});
}
