#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/compiler/backends/tbc/TBCProgramSetup.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
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


void configureProgram(TBCProgram& program, const engine::Options& options) {
	program.dispatch =
	    clampDispatchMode(parseDispatchMode(options.getOptionOrDefault<std::string>("tbc.dispatch", "auto")));
	const auto stackSizeKb = options.getOptionOrDefault("tbc.stackSizeKb", 1024);
	program.minStackSlots = static_cast<uint64_t>(stackSizeKb) * 1024 / sizeof(uint64_t);
}

void finishProgram(TBCProgram& programRef, const engine::Options& options, CompilationStatistics* statistics,
                   int64_t totalInstructions, int64_t maxRegisters,
                   std::chrono::steady_clock::time_point backendStart) {
	auto* program = &programRef;
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

}

} // namespace nautilus::compiler::tbc
