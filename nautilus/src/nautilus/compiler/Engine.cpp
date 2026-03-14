#include "nautilus/Engine.hpp"

namespace nautilus::engine {

NautilusEngine::NautilusEngine(const Options& options) : options(options) {
	auto backend = options.getOptionOrDefault<std::string>("engine.backend", "mlir");
	if (backend == "multi-tier") {
		jit_.emplace<compiler::MultiTierJitCompiler>(options);
	} else {
		jit_.emplace<compiler::JITCompiler>(options);
	}
}

std::string NautilusEngine::getNameOfBackend() const {
	return std::visit([](const auto& jit) { return jit.getName(); }, jit_);
}

std::unique_ptr<compiler::Executable>
NautilusEngine::compileWrapper(compiler::JITCompiler::wrapper_function wrapper) const {
	return std::visit([&wrapper](const auto& jit) { return jit.compile(std::move(wrapper)); }, jit_);
}

} // namespace nautilus::engine
