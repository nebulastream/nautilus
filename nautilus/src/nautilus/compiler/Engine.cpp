#include "nautilus/Engine.hpp"
#include "nautilus/JITCompiler.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/TieredCompiler.hpp"
#include "nautilus/logging.hpp"
#include <stdexcept>

#ifdef ENABLE_TRACING
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
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

CompiledModule NautilusEngine::compileIR(std::shared_ptr<compiler::ir::IRGraph> ir, ModuleOptions overrides) const {
#ifdef ENABLE_TRACING
	if (!isCompiled()) {
		throw std::logic_error("compileIR needs a compiling engine; this one interprets (engine.Compilation=false)");
	}
	if (!ir) {
		throw std::invalid_argument("compileIR needs a graph");
	}
	auto moduleOptions = options.deriveModuleOptions();
	moduleOptions.applyOverrides(overrides);

	auto state = std::make_shared<details::ModuleState>();
	for (const auto* function : ir->getFunctionOperations()) {
		details::FunctionTypeSignature signature;
		signature.result = function->getOutputArg();
		for (const auto* argument : function->getEntryBlock()->getArguments()) {
			signature.params.push_back(argument->getStamp());
		}
		state->signatures.emplace(function->getName(), std::move(signature));
	}
	jit_->compileModuleFromIR(std::move(ir), moduleOptions, state);
	return CompiledModule(std::move(state));
#else
	(void) ir;
	(void) overrides;
	throw std::logic_error("compileIR needs a build with ENABLE_TRACING");
#endif
}
NautilusEngine::NautilusEngine(NautilusEngine&&) noexcept = default;

} // namespace nautilus::engine
