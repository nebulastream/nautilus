#include "nautilus/Engine.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/compiler/LegacyCompiler.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
namespace nautilus::engine {

/**
 * @brief Creates the appropriate IJITCompiler implementation based on options.
 *
 * If tiered compilation options are set (engine.tier0.backend and engine.tier1.backend),
 * returns a TieredJITCompiler. Otherwise returns a standard JITCompiler.
 */
inline std::unique_ptr<compiler::JITCompiler> createJITCompiler(const Options& options) {
	// If the user explicitly selected a backend, use the legacy single-tier compiler
	// so the requested backend is honoured directly.
	auto explicitBackend = options.getOptionOrDefault<std::string>("engine.backend", "");
	if (!explicitBackend.empty()) {
		return std::make_unique<compiler::LegacyCompiler>(options);
	}
	std::string compilerStrategy = options.getOptionOrDefault("engine.compilationStrategy", std::string("tiered"));
	if (compilerStrategy == "tiered") {
		return std::make_unique<compiler::TieredJITCompiler>(options);
	}
	return std::make_unique<compiler::LegacyCompiler>(options);
}

NautilusEngine::NautilusEngine(const Options& options) : jit_(createJITCompiler(options)), options(options) {
}

NautilusEngine::NautilusEngine(std::unique_ptr<compiler::JITCompiler> jit, const Options& options)
    : jit_(std::move(jit)), options(options) {
}

} // namespace nautilus::engine
