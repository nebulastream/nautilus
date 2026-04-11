// DemoInliningPlugin.cpp — showcase the `nautilus-inlining` plugin.
//
// The inlining plugin lets plain C++ helper functions marked with
// `NAUTILUS_INLINE` be inlined directly into JIT-compiled code. Without the
// plugin, a call from traced code through `invoke(helper, ...)` lowers to an
// opaque external C call in the generated LLVM IR. With the plugin enabled,
// the Clang pass plugin extracts the helper's LLVM bitcode at compile time
// and registers it; when the MLIR backend generates the JITed module, the
// plugin's `preOptModuleTransform` hook swaps the external call for the
// helper's body and re-runs LLVM's inliner.
//
// How it works:
//
//   1. `NAUTILUS_INLINE` (from `<nautilus/inline.hpp>`) annotates helper
//      functions so the Clang pass plugin recognizes them.
//   2. `nautilus_inline(demo_inlining_plugin)` in the example CMakeLists.txt
//      attaches `-fpass-plugin=<InliningPass>` to this demo's compile line so
//      the pass runs over this TU and populates `InlineFunctionRegistry`.
//   3. Linking against `nautilus-inlining` pulls the plugin's static
//      initializer in through the anchor trick in `<nautilus/inline.hpp>`,
//      which installs the LLVM backend hooks.
//   4. `options.setOption("mlir.inline_invoke_calls", true)` flips the MLIR
//      backend to consult those hooks during JIT compilation.
//
// This demo compiles the same traced function twice — once with the inliner
// off, once with it on — and dumps the generated LLVM IR for both. Diff the
// two dumps to see the difference: the "off" version contains `call @sq(...)`
// and `call @cube(...)` instructions, while the "on" version has the helper
// bodies folded into `execute` (and constant-folded by the LLVM optimizer).

#include <cstdint>
#include <iostream>
#include <nautilus/Engine.hpp>
#include <nautilus/function.hpp>
#include <nautilus/inline.hpp>
#include <nautilus/val.hpp>

using namespace nautilus;

// Two plain C++ helpers tagged as inlining candidates. The Clang pass plugin
// picks them up by the `NAUTILUS_INLINE` annotation and stashes their bitcode
// in the `InlineFunctionRegistry` at program startup. Keep them deliberately
// simple so the inlined form is easy to recognize in the LLVM IR dump.
NAUTILUS_INLINE int32_t sq(int32_t x) {
	return x * x;
}

NAUTILUS_INLINE int32_t cube(int32_t x) {
	return x * x * x;
}

// Traced function that delegates the per-element math to the runtime helpers
// via `invoke()`. Without the inlining plugin, each iteration of the loop
// emits two external calls. With the plugin on, both helper bodies fold into
// the loop body and LLVM's arithmetic simplifier kicks in.
val<int32_t> sumPolynomial(val<int32_t> n) {
	val<int32_t> acc = 0;
	for (val<int32_t> i = 0; i < n; i++) {
		acc += invoke(sq, i) + invoke(cube, i);
	}
	return acc;
}

static auto makeEngine(bool inlineInvokeCalls, const char* label) {
	engine::Options options;
	// The JIT-time inliner only runs under the MLIR backend, so pin it here.
	options.setOption("engine.backend", std::string("mlir"));
	// Single-threaded so the dumped IR is deterministic across runs.
	options.setOption("mlir.enableMultithreading", false);
	// This is the switch that tells the MLIR backend to consult the plugin's
	// `preOptModuleTransform` / `proxyCallNameOverride` hooks and inline
	// registered helper bodies at JIT time.
	options.setOption("mlir.inline_invoke_calls", inlineInvokeCalls);
	// Dump the generated LLVM IR so users can diff the two variants.
	options.setOption("dump.after_llvm_generation", true);

	std::cout << "=== Compiling sumPolynomial (" << label << ") ===\n";
	return engine::NautilusEngine(options);
}

int main(int, char*[]) {
	// Compile the same traced function twice under two different option sets.
	auto plainEngine = makeEngine(false, "mlir.inline_invoke_calls = false");
	auto plainFn = plainEngine.registerFunction(sumPolynomial);

	std::cout << "\n";

	auto inlinedEngine = makeEngine(true, "mlir.inline_invoke_calls = true");
	auto inlinedFn = inlinedEngine.registerFunction(sumPolynomial);

	// Execute both to confirm they produce identical results. For n = 5:
	//   sum of i*i  for i in [0, 5) = 0 + 1 + 4 + 9 + 16 = 30
	//   sum of i*i*i for i in [0, 5) = 0 + 1 + 8 + 27 + 64 = 100
	//   total = 130
	constexpr int32_t kN = 5;
	const int32_t plainResult = plainFn(kN);
	const int32_t inlinedResult = inlinedFn(kN);

	std::cout << "\n=== Results ===\n";
	std::cout << "sumPolynomial(" << kN << ") plain   = " << plainResult << "\n";
	std::cout << "sumPolynomial(" << kN << ") inlined = " << inlinedResult << "\n";
	std::cout << "(Both should be 130.)\n";
	return 0;
}
