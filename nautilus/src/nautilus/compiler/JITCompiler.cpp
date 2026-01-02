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
	auto dumpHandler = DumpHandler(options, compilationId);

	// Timing instrumentation
	auto start_total = std::chrono::high_resolution_clock::now();

	// derive trace from function
	auto start_tracing = std::chrono::high_resolution_clock::now();
	auto executionTrace = tracing::TraceContext::trace(function, options);
	auto end_tracing = std::chrono::high_resolution_clock::now();
	auto tracing_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_tracing - start_tracing);

	dumpHandler.dump("after_tracing", "trace", [&]() { return executionTrace->toString(); });

	// create ssa
	auto start_ssa = std::chrono::high_resolution_clock::now();
	auto ssaCreationPhase = tracing::SSACreationPhase();
	auto afterSSA = ssaCreationPhase.apply(std::move(executionTrace));
	auto end_ssa = std::chrono::high_resolution_clock::now();
	auto ssa_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_ssa - start_ssa);

	dumpHandler.dump("after_ssa", "trace", [&]() { return afterSSA->toString(); });

	// get nautilus ir from trace
	auto start_ir = std::chrono::high_resolution_clock::now();
	auto irGenerationPhase = tracing::TraceToIRConversionPhase();
	const auto ir = irGenerationPhase.apply(std::move(afterSSA), compilationId);
	auto end_ir = std::chrono::high_resolution_clock::now();
	auto ir_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_ir - start_ir);

	dumpHandler.dump("after_ir_creation", "ir", [&]() { return ir->toString(); });
	if (options.getOptionOrDefault("dump.graph", false)) {
		ir::createGraphVizFromIr(ir, options, dumpHandler);
	}

	// lower to backend
	auto start_backend = std::chrono::high_resolution_clock::now();
	const auto backendName = getName();
	const auto backend = backends->getBackend(backendName);
	auto executable = backend->compile(ir, dumpHandler, options);
	auto end_backend = std::chrono::high_resolution_clock::now();
	auto backend_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_backend - start_backend);

	auto end_total = std::chrono::high_resolution_clock::now();
	auto total_duration = std::chrono::duration_cast<std::chrono::microseconds>(end_total - start_total);

	// Display latency breakdown
	fmt::println(stderr, "\n=== Compilation Latency Breakdown ===");
	fmt::println(stderr, "Tracing:      {:>10} us ({:>5.1f}%)", tracing_duration.count(),
	             100.0 * tracing_duration.count() / total_duration.count());
	fmt::println(stderr, "SSA Creation: {:>10} us ({:>5.1f}%)", ssa_duration.count(),
	             100.0 * ssa_duration.count() / total_duration.count());
	fmt::println(stderr, "IR Generation:{:>10} us ({:>5.1f}%)", ir_duration.count(),
	             100.0 * ir_duration.count() / total_duration.count());
	fmt::println(stderr, "Backend:      {:>10} us ({:>5.1f}%)", backend_duration.count(),
	             100.0 * backend_duration.count() / total_duration.count());
	fmt::println(stderr, "-------------------------------------");
	fmt::println(stderr, "Total:        {:>10} us", total_duration.count());
	fmt::println(stderr, "=====================================\n");

	executable->setGeneratedFiles(dumpHandler.getGeneratedFiles());
	return executable;
}

#else

std::unique_ptr<Executable> JITCompiler::compile(JITCompiler::wrapper_function) const {
	throw RuntimeException("Jit not initialised");
}

#endif
} // namespace nautilus::compiler
