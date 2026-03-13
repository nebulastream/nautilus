#include "TPDECompilationBackend.hpp"

#include "NautilusCompiler.hpp"
#include "NautilusIRAdaptor.hpp"
#include "TPDEExecutable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "tpde/AssemblerElf.hpp"

namespace nautilus::compiler::tpde {

std::unique_ptr<Executable> TPDECompilationBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                             const DumpHandler& /*dumpHandler*/,
                                                             const engine::Options& /*options*/) const {
	// 1. Build the IR adaptor (pre-computes dense indices, phi maps, etc.)
	NautilusIRAdaptor adaptor(ir);

	// 2. Create the compiler (contains the x64 assembler)
	NautilusCompiler compiler(&adaptor);

	// 3. Run single-pass compilation
	if (!compiler.compile()) {
		throw RuntimeException("TPDE: compilation failed");
	}

	// 4. Retrieve the symbol reference for the compiled function.
	//    func_syms[0] is the symbol for the single function in the IR graph.
	if (compiler.func_syms.empty()) {
		throw RuntimeException("TPDE: no function symbols produced after compilation");
	}
	::tpde::SymRef func_sym = compiler.func_syms[0];

	// 5. Load ELF output into executable memory and return wrapped executable.
	return std::make_unique<TPDEExecutable>(compiler.assembler, func_sym, compiler.external_symbol_addresses);
}

} // namespace nautilus::compiler::tpde
