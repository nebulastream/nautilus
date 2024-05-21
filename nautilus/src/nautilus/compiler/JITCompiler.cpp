#include "nautilus/JITCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <utility>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"

#endif

namespace nautilus::compiler {

JITCompiler::JITCompiler() : options(), backends(std::make_unique<CompilationBackendRegistry>()) {
}

JITCompiler::JITCompiler(engine::Options options)
    : options(std::move(options)), backends(std::make_unique<CompilationBackendRegistry>()) {
}

JITCompiler::~JITCompiler() = default;

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function function) const {
	// derive trace from function
	auto executionTrace = tracing::TraceContext::trace(function);
	// create ssa
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
	// get nautilus ir from trace
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	auto ir = irGenerationPhase.apply(std::move(afterSSA));
	// lower to backend
	auto backendName = options.getOptionOrDefault<std::string>("engine.backend", "mlir");
	auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir);
	return executable;
}

#else

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
