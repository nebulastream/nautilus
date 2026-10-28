#include "nautilus/compiler/backends/mlir/MLIRNirBackend.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <atomic>
#include <unistd.h>

namespace nautilus::compiler::mlir {

MLIRNirBackend::MLIRNirBackend() = default;

std::unique_ptr<Executable> MLIRNirBackend::compile(std::shared_ptr<const nir::Module> module,
                                                    const nirbackend::CompileContext& context) const {
	static std::atomic<uint64_t> counter {0};
	return compileNamed(std::move(module), context,
	                    "nir_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)));
}

std::unique_ptr<Executable> MLIRNirBackend::compileNamed(std::shared_ptr<const nir::Module>,
                                                         const nirbackend::CompileContext&, const std::string&) const {
	throw NotImplementedException("mlir: the lowering of NIR is written in the next commit");
}

} // namespace nautilus::compiler::mlir
