#include "nautilus/Engine.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/LegacyCompiler.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"

namespace nautilus::engine {

/**
 * @brief Creates the appropriate JITCompiler implementation based on options.
 *
 * The engine-owned @p arena is injected into the compiler so every trace
 * allocation across every compile() call shares a single pool of chunks.
 */
inline std::unique_ptr<compiler::JITCompiler> createJITCompiler(const Options& options, common::Arena& arena) {
	// If the user explicitly selected a backend, use the legacy single-tier compiler
	// so the requested backend is honoured directly.
	auto explicitBackend = options.getOptionOrDefault<std::string>("engine.backend", "");
	if (!explicitBackend.empty()) {
		return std::make_unique<compiler::LegacyCompiler>(options, arena);
	}
	std::string compilerStrategy = options.getOptionOrDefault("engine.compilationStrategy", std::string("tiered"));
	if (compilerStrategy == "tiered") {
		return std::make_unique<compiler::TieredJITCompiler>(options, arena);
	}
	return std::make_unique<compiler::LegacyCompiler>(options, arena);
}

NautilusEngine::NautilusEngine(const Options& options)
    : arena_(std::make_unique<common::Arena>()), jit_(createJITCompiler(options, *arena_)), options(options) {
}

NautilusEngine::NautilusEngine(std::unique_ptr<compiler::JITCompiler> jit, const Options& options)
    : arena_(std::make_unique<common::Arena>()), jit_(std::move(jit)), options(options) {
}

NautilusEngine::~NautilusEngine() = default;
NautilusEngine::NautilusEngine(NautilusEngine&&) noexcept = default;

} // namespace nautilus::engine
