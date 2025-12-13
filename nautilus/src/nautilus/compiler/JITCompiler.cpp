#include "nautilus/JITCompiler.hpp"
#include "nautilus/Executable.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CompilationBackend.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include <chrono>
#include <fmt/chrono.h>
#include <fmt/core.h>
#include <iomanip>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

#ifdef ENABLE_COMPILER

#include "nautilus/compiler/ir/util/GraphVizUtil.hpp"
#include "nautilus/tracing/TraceContext.hpp"
#include "nautilus/tracing/phases/SSACreationPhase.hpp"
#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#endif

namespace nautilus::compiler {

JITCompiler::JITCompiler() : options(), backends(CompilationBackendRegistry::getInstance()) {
}

JITCompiler::JITCompiler(engine::Options options)
    : options(std::move(options)), backends(CompilationBackendRegistry::getInstance()) {
}

JITCompiler::~JITCompiler() = default;

#if defined(ENABLE_COMPILER) && defined(ENABLE_TRACING)

std::string createCompilationUnitID() {
	// Get the current time point
	// Create a timestamp string from the current time

	auto now = std::chrono::system_clock::now();
	std::string timestamp = fmt::format(fmt::runtime("{:%Y-%m-%d_%H-%M-%S}"), now);

	// Create a random device and generator
	std::random_device rd;
	std::mt19937 generator(rd());
	std::uniform_int_distribution<> distribution(0, 15);

	// Generate a 7-character UUID
	std::string uuid;
	for (int i = 0; i < 7; ++i) {
		int random_number = distribution(generator);
		if (random_number < 10)
			uuid += std::to_string(random_number);
		else
			uuid += char('A' + random_number - 10);
	}

	// Concatenate timestamp and UUID
	return timestamp + "_#" + uuid;
}

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function function) const {
	const CompilationUnitID compilationId = createCompilationUnitID();
	const auto dumpHandler = DumpHandler(options, compilationId);
	// derive trace from function
	auto executionTrace = tracing::TraceContext::trace(function, options);
	dumpHandler.dump("after_tracing", "trace", [&]() { return executionTrace->toString(); });

	// create ssa
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
	dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSA->toString(); });
	// get nautilus ir from trace
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	const auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
	dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });
	if (options.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, options, dumpHandler);
	}
	// lower to backend
	const auto backendName = getName();
	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, options);
	return executable;
}

#else

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
