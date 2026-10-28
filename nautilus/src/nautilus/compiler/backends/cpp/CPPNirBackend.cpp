#include "nautilus/compiler/backends/cpp/CPPNirBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/cpp/CPPCompiler.hpp"
#include "nautilus/compiler/backends/cpp/CPPExecutable.hpp"
#include "nautilus/compiler/backends/cpp/CPPNirLowering.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <atomic>
#include <chrono>
#include <unistd.h>

namespace nautilus::compiler::cpp {

std::unique_ptr<Executable> CPPNirBackend::compile(std::shared_ptr<const nir::Module> module,
                                                   const nirbackend::CompileContext& context) const {
	static std::atomic<uint64_t> counter {0};
	return compileNamed(std::move(module), context,
	                    "nir_" + std::to_string(::getpid()) + "_" + std::to_string(counter.fetch_add(1)));
}

std::unique_ptr<Executable> CPPNirBackend::compileNamed(std::shared_ptr<const nir::Module> module,
                                                        const nirbackend::CompileContext& context,
                                                        const std::string& unit) const {
	(void) module;
	(void) context;
	(void) unit;
	throw NotImplementedException("cpp on NIR is not implemented yet");
}

} // namespace nautilus::compiler::cpp
