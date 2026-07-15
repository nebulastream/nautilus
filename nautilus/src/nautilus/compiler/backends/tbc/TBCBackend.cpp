
#include "nautilus/compiler/backends/tbc/TBCBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/tbc/TBCExecutable.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/compiler/backends/tbc/TBCLoweringProvider.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>
#ifdef ENABLE_TBC_JIT
#include "nautilus/compiler/backends/tbc/jit/TBCJit.hpp"
#endif

namespace nautilus::compiler::tbc {

namespace {
DispatchMode parseDispatchMode(const std::string& name) {
	if (name == "tailcall") {
		return DispatchMode::Tailcall;
	}
	if (name == "goto") {
		return DispatchMode::Goto;
	}
	if (name == "switch") {
		return DispatchMode::Switch;
	}
	// "auto" (and anything unknown) selects the strongest mode this build
	// supports: tail-call on Clang, computed goto on GCC, switch otherwise.
	return bestAvailableDispatchMode();
}

const char* dispatchModeName(DispatchMode mode) {
	switch (mode) {
	case DispatchMode::Tailcall:
		return "tailcall";
	case DispatchMode::Goto:
		return "goto";
	case DispatchMode::Switch:
		return "switch";
	}
	return "unknown";
}

/// Execution mode: `interp` (default) runs the dispatch-loop interpreter;
/// `jit` stitches copy-and-patch code and fails fast when this build cannot
/// (so tests never silently pass on the wrong engine); `auto` uses the JIT
/// when available and degrades to the interpreter otherwise.
enum class ExecutionMode : uint8_t { Interp, Jit, Auto };

ExecutionMode parseExecutionMode(const std::string& name) {
	if (name == "jit") {
		return ExecutionMode::Jit;
	}
	if (name == "auto") {
		return ExecutionMode::Auto;
	}
	return ExecutionMode::Interp;
}
} // namespace

std::unique_ptr<Executable> TBCBackend::compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
                                                const engine::Options& options,
                                                CompilationStatistics* statistics) const {
	const auto backendStart = std::chrono::steady_clock::now();
	const auto& functionOperations = ir->getFunctionOperations();

	// All optimizations default to ON; the options exist for A/B comparisons.
	TBCLoweringOptions loweringOptions;
	loweringOptions.enableRegisterAllocator = options.getOptionOrDefault("tbc.registerAllocator", true);
	loweringOptions.enableRegisterCoalescing = options.getOptionOrDefault("tbc.coalescing", true);
	loweringOptions.enableSuperinstructions = options.getOptionOrDefault("tbc.superinstructions", true);
	loweringOptions.enableImmediates = options.getOptionOrDefault("tbc.immediates", true);

	auto program = std::make_shared<TBCProgram>();
	program->dispatch =
	    clampDispatchMode(parseDispatchMode(options.getOptionOrDefault<std::string>("tbc.dispatch", "auto")));
	const auto stackSizeKb = options.getOptionOrDefault("tbc.stackSizeKb", 1024);
	program->minStackSlots = static_cast<uint64_t>(stackSizeKb) * 1024 / sizeof(uint64_t);

	// The function index and every function's signature must exist before any
	// lowering runs: calls and FunctionAddressOf between module functions
	// resolve to interpreter-native targets (or trampolines) regardless of
	// lowering order.
	program->functions.resize(functionOperations.size());
	for (size_t i = 0; i < functionOperations.size(); i++) {
		const auto& funcOp = functionOperations[i];
		program->functionIndex[funcOp->getName()] = static_cast<uint32_t>(i);
		auto& function = program->functions[i];
		function.name = funcOp->getName();
		function.returnType = funcOp->getOutputArg();
		for (const auto* argument : funcOp->getFunctionBasicBlock().getArguments()) {
			function.argTypes.push_back(argument->getStamp());
		}
	}

	int64_t totalInstructions = 0;
	int64_t maxRegisters = 0;
	for (size_t i = 0; i < functionOperations.size(); i++) {
		const auto& funcOp = functionOperations[i];
		auto& function = program->functions[i];
		TBCLoweringProvider::lower(funcOp, *program, function, loweringOptions);

		totalInstructions += static_cast<int64_t>(function.code.size());
		maxRegisters = std::max<int64_t>(maxRegisters, function.regSlots);
	}

	// Dump every module function, not just a fixed "execute" entry point
	// (module-API functions carry user-chosen names).
	dumpHandler.dump("after_tbc_generation", "tbc", [&program]() {
		std::string out;
		for (const auto& function : program->functions) {
			out += function.toString();
			out += "\n";
		}
		return out;
	});

	// Copy-and-patch JIT (tbc.mode): stitching is the LAST compile step — it
	// patches &functions[i]/&callsites[i] into the code, so both vectors must
	// be final. `jit` is strict (throws when this build cannot execute
	// stitched code, so nothing silently tests the wrong engine); `auto`
	// degrades to the interpreter.
	std::string jitMode = "interp";
	std::string jitSkipReason;
	const auto mode = parseExecutionMode(options.getOptionOrDefault<std::string>("tbc.mode", "interp"));
	if (mode != ExecutionMode::Interp) {
#ifdef ENABLE_TBC_JIT
		if (jit::jitRuntimeAvailable()) {
			program->jit = jit::stitchProgram(*program, &jitSkipReason);
		} else {
			jitSkipReason = "jit runtime unavailable on this build (needs Clang 19+ stencil ABI support, no ASan)";
		}
#else
		jitSkipReason = "built without ENABLE_TBC_JIT";
#endif
		if (program->jit != nullptr) {
			jitMode = "jit";
		} else if (mode == ExecutionMode::Jit) {
			throw RuntimeException("tbc: tbc.mode=jit requested but " + jitSkipReason);
		}
	}

	if (statistics != nullptr) {
		statistics->set("tbc.instructions", totalInstructions);
		statistics->set("tbc.codeSize.bytes", totalInstructions * static_cast<int64_t>(sizeof(Instr)));
		statistics->set("tbc.registers.max", maxRegisters);
		statistics->set("tbc.dispatch", std::string(dispatchModeName(program->dispatch)));
		statistics->set("tbc.mode", jitMode);
		if (program->jit != nullptr) {
			statistics->set("tbc.jit.codeSize.bytes", static_cast<int64_t>(program->jit->codeBytes));
		}
		if (!jitSkipReason.empty()) {
			statistics->set("tbc.jit.skipReason", jitSkipReason);
		}
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}

	return std::make_unique<TBCExecutable>(std::move(program));
}

} // namespace nautilus::compiler::tbc
