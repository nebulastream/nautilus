#include "nautilus/JITCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>
#include <utility>

#ifdef ENABLE_COMPILER

#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"

#endif

namespace nautilus::compiler {

JITCompiler::JITCompiler() : options(), backends(std::make_unique<CompilationBackendRegistry>()) {
}

JITCompiler::JITCompiler(engine::Options options) : options(std::move(options)), backends(std::make_unique<CompilationBackendRegistry>()) {
}

JITCompiler::~JITCompiler() = default;

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)

std::string createCompilationUnitID() {
	// Get the current time point
	auto now = std::chrono::system_clock::now();
	auto time = std::chrono::system_clock::to_time_t(now);
	auto local_time = *std::localtime(&time);

	// Create a timestamp string from the current time
	std::ostringstream timestamp;
	timestamp << std::put_time(&local_time, "%Y-%m-%d_%H-%M-%S");

	// Create a random device and generator
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, 15);

	// Generate a 4-character UUID
	std::string uuid;
	for (int i = 0; i < 4; ++i) {
		int random_number = distribution(generator);
		if (random_number < 10)
			uuid += std::to_string(random_number);
		else
			uuid += char('A' + random_number - 10);
	}

	// Concatenate timestamp and UUID
	return timestamp.str() + "_#" + uuid;
}

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function function) const {
	CompilationUnitID compilationId = createCompilationUnitID();
	auto dumpHandler = DumpHandler(options, compilationId);
	// derive trace from function
	auto executionTrace = tracing::TraceContext::trace(function);
	dumpHandler.dump("after_tracing", [&]() { return executionTrace->toString(); });

	// create ssa
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
	dumpHandler.dump("after_ssa", [&]() { return afterSSA->toString(); });
	// get nautilus ir from trace
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
	dumpHandler.dump("after_ir_creation", [&]() { return ir->toString(); });
	// lower to backend
	auto backendName = options.getOptionOrDefault<std::string>("engine.backend", "mlir");
	auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, options);
	return executable;
}

#else

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
