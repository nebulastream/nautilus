#include "nautilus/Engine.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/LegacyCompiler.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"

namespace nautilus::engine {

/**
 * @brief Creates the appropriate JITCompiler implementation based on options.
 *
 * The engine-owned @p traceArenaPool and @p irArenaPool are injected into the
 * compiler so every compile() acquires a fresh, recycled trace arena (trace
 * pool), and every IR-graph allocation likewise recycles its arena across
 * compiles (IR pool).  Both pools are internally synchronized, so a single
 * compiler may serve concurrent compile() calls from multiple threads.
 */
inline std::unique_ptr<compiler::JITCompiler>
createJITCompiler(const Options& options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool) {
	// If the user explicitly selected a backend, use the legacy single-tier compiler
	// so the requested backend is honoured directly.
	auto explicitBackend = options.getOptionOrDefault<std::string>("engine.backend", "");
	if (!explicitBackend.empty()) {
		return std::make_unique<compiler::LegacyCompiler>(options, traceArenaPool, irArenaPool);
	}
	std::string compilerStrategy = options.getOptionOrDefault("engine.compilationStrategy", std::string("tiered"));
	if (compilerStrategy == "tiered") {
		return std::make_unique<compiler::TieredJITCompiler>(options, traceArenaPool, irArenaPool);
	}
	return std::make_unique<compiler::LegacyCompiler>(options, traceArenaPool, irArenaPool);
}

NautilusEngine::NautilusEngine(const Options& options)
    : traceArenaPool_(std::make_unique<common::ArenaPool>()), irArenaPool_(std::make_unique<common::ArenaPool>()),
      jit_(createJITCompiler(options, *traceArenaPool_, *irArenaPool_)), options(options) {
}

NautilusEngine::NautilusEngine(std::unique_ptr<compiler::JITCompiler> jit, const Options& options)
    : traceArenaPool_(std::make_unique<common::ArenaPool>()), irArenaPool_(std::make_unique<common::ArenaPool>()),
      jit_(std::move(jit)), options(options) {
}

NautilusEngine::~NautilusEngine() = default;
NautilusEngine::NautilusEngine(NautilusEngine&&) noexcept = default;

} // namespace nautilus::engine
