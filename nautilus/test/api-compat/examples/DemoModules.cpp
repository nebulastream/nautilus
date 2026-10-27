// DemoModules.cpp — showcase the NautilusModule API.
//
// A NautilusModule groups several named functions into one compilation unit
// so they share a single IR graph, a single optimization run, and a single
// backend invocation. The compiled result is a CompiledModule, from which
// callers fetch stable ModuleFunction handles by name.
//
// Compare with DemoJit.cpp, which uses the one-shot `registerFunction`
// convenience that wraps a module with a single "execute" function.

#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

int main(int, char*[]) {
	// Default options → tiered JIT compiler, MLIR backend (if available).
	engine::Options options;
	auto engine = engine::NautilusEngine(options);

	// Build a module that holds three independent functions.
	auto module = engine.createModule();

	// `registerFunction<Sig>(name, lambda)` — explicit val-typed signature is
	// required for lambdas so the template can deduce argument types.
	module.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>(
	    "add", [](val<int32_t> a, val<int32_t> b) { return a + b; });

	module.registerFunction<val<int32_t>(val<int32_t>, val<int32_t>)>(
	    "mul", [](val<int32_t> a, val<int32_t> b) { return a * b; });

	// Iterative factorial — exercises tracing through a loop.
	module.registerFunction<val<int64_t>(val<int32_t>)>("factorial", [](val<int32_t> n) {
		val<int64_t> result = 1;
		for (val<int32_t> i = 1; i <= n; i++) {
			result = result * i;
		}
		return result;
	});

	// Compile all three into a single compilation unit.
	auto compiled = module.compile();

	// Fetch typed handles. The raw signature (no `val<>`) is used here.
	auto add = compiled.getFunction<int32_t(int32_t, int32_t)>("add");
	auto mul = compiled.getFunction<int32_t(int32_t, int32_t)>("mul");
	auto factorial = compiled.getFunction<int64_t(int32_t)>("factorial");

	std::cout << "Backend:         " << engine.getNameOfBackend() << "\n";
	std::cout << "add(3, 4)      = " << add(3, 4) << "\n";
	std::cout << "mul(3, 4)      = " << mul(3, 4) << "\n";
	std::cout << "factorial(5)   = " << factorial(5) << "\n";
	std::cout << "factorial(10)  = " << factorial(10) << "\n";

	return 0;
}
