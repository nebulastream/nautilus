#include "nautilus/Engine.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <fstream>
#include <sstream>

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
#include "nautilus/compiler/CompilationPipeline.hpp"
#include "nautilus/compiler/ir/util/IRParser.hpp"
#endif

namespace nautilus::engine {

/**
 * @brief Creates the JITCompiler for the engine.
 *
 * The TieredJITCompiler is the only compilation strategy. Single-tier
 * compilation is configured through it: `engine.backend=<x>` pins a single
 * backend (synchronous, no promotion) and `engine.tiered.backgroundPromotion=false`
 * compiles single-tier with the tier-1 backend.
 *
 * The engine-owned @p traceArenaPool and @p irArenaPool are injected into the
 * compiler so every compile() acquires a fresh, recycled trace arena (trace
 * pool), and every IR-graph allocation likewise recycles its arena across
 * compiles (IR pool).  Both pools are internally synchronized, so a single
 * compiler may serve concurrent compile() calls from multiple threads.
 */
inline std::unique_ptr<compiler::JITCompiler>
createJITCompiler(const Options& options, common::ArenaPool& traceArenaPool, common::ArenaPool& irArenaPool) {
	if (!options.getOptionOrDefault<std::string>("engine.compilationStrategy", "").empty()) {
		log::warn("The option 'engine.compilationStrategy' was removed and is ignored; the tiered compiler is always "
		          "used. Set 'engine.backend' or 'engine.tiered.backgroundPromotion=false' for single-tier "
		          "compilation.");
	}
	return std::make_unique<compiler::TieredJITCompiler>(options, traceArenaPool, irArenaPool);
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

CompiledModule NautilusEngine::loadModuleFromIR(const std::string& irText,
                                                const IRSymbolResolver& symbolResolver) const {
#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)
	if (!isCompiled()) {
		throw RuntimeException("loadModuleFromIR requires compilation to be enabled (engine.Compilation): a module "
		                       "loaded from IR has no original callables to interpret");
	}
	auto ir = compiler::ir::parseIR(irText, *irArenaPool_, compiler::createCompilationUnitID(), symbolResolver);
	auto state = std::make_shared<details::ModuleState>();
	jit_->compileIRModule(std::move(ir), options.deriveModuleOptions(), state);
	return CompiledModule(std::move(state));
#else
	(void) irText;
	(void) symbolResolver;
	throw RuntimeException("loadModuleFromIR requires a build with ENABLE_COMPILER and ENABLE_TRACING");
#endif
}

CompiledModule NautilusEngine::loadModuleFromIRFile(const std::string& path,
                                                    const IRSymbolResolver& symbolResolver) const {
	std::ifstream file(path);
	if (!file) {
		throw RuntimeException("Could not open IR file '" + path + "'");
	}
	std::stringstream content;
	content << file.rdbuf();
	return loadModuleFromIR(content.str(), symbolResolver);
}

} // namespace nautilus::engine
