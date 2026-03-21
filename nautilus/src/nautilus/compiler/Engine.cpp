#include "nautilus/Engine.hpp"

namespace nautilus::engine {

NautilusEngine::NautilusEngine(const Options& options)
    : jit_(std::make_unique<compiler::JITCompiler>(options)), options(options) {
#ifdef ENABLE_TRACING
	// Check if tiered compilation is requested via options
	auto tier0 = options.getOptionOrDefault<std::string>("engine.tier0.backend", "");
	auto tier1 = options.getOptionOrDefault<std::string>("engine.tier1.backend", "");
	if (!tier0.empty() && !tier1.empty()) {
		TieredCompilationConfig config;
		config.tier0.backend = tier0;
		config.tier1.backend = tier1;
		jit_ = std::make_unique<compiler::TieredJITCompiler>(options, config);
	}
#endif
}

NautilusEngine::NautilusEngine(std::unique_ptr<compiler::IJITCompiler> jit, const Options& options)
    : jit_(std::move(jit)), options(options) {
}

} // namespace nautilus::engine
