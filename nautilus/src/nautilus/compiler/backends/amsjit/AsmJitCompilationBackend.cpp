
#include "nautilus/compiler/backends/amsjit/AsmJitCompilationBackend.hpp"
#include <asmjit/a64.h>
#include <iostream>

using namespace ::asmjit;
namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitCompilationBackend::compile(std::shared_ptr<ir::IRGraph>) {
	JitRuntime rt;   // Runtime specialized for JIT code execution.
	CodeHolder code; // Holds code and relocation information.

	code.init(rt.environment(), // Initialize code to match the JIT environment.
	          rt.cpuFeatures());
	a64::Compiler cc(&code); // Create and attach x86::Compiler to code.

	cc.addFunc(FuncSignature::build<int>()); // Begin a function of `int fn(void)` signature.

	a64::Gp vReg = cc.newInt32(); // Create a 32-bit general purpose register.
	cc.mov(vReg, 1);              // Move one to our virtual register `vReg`.
	cc.ret(vReg);                 // Return `vReg` from the function.

	cc.endFunc();  // End of the function body.
	cc.finalize(); // Translate and assemble the whole 'cc' content.
	// ----> x86::Compiler is no longer needed from here and can be destroyed <----
	typedef int (*Func)(void);
	Func fn;
	auto err = rt.add(&fn, &code); // Add the generated code to the runtime.
	if (err) {
		return nullptr;
	};
	// Handle a possible error returned by AsmJit.
	// ----> CodeHolder is no longer needed from here and can be destroyed <----

	int result = fn();      // Execute the generated code.
	printf("%d\n", result); // Print the resulting "1".

	rt.release(fn);
	return nullptr;
}

} // namespace nautilus::compiler::asmjit
