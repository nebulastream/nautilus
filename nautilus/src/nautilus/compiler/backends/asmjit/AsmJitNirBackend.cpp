#include "nautilus/compiler/backends/asmjit/AsmJitNirBackend.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"

namespace nautilus::compiler::asmjit {

std::unique_ptr<Executable> AsmJitNirBackend::compile(std::shared_ptr<const nir::Module> module,
                                                      const nirbackend::CompileContext& context) const {
	(void) module;
	(void) context;
	throw NotImplementedException("asmjit on NIR is not implemented yet");
}

} // namespace nautilus::compiler::asmjit
