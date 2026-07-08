
#include "nautilus/compiler/backends/tbc/TBCBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/tbc/TBCExecutable.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/compiler/backends/tbc/TBCLoweringProvider.hpp"
#include "nautilus/compiler/backends/tbc/TBCThunkCall.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <chrono>

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

ExtCallMode parseExtCallMode(const std::string& name) {
	if (name == "dyncall") {
		return ExtCallMode::Dyncall;
	}
	if (name == "thunks") {
		return ExtCallMode::Thunks;
	}
	// "auto" (and anything unknown) selects the build's default: dyncall when
	// it is linked, the zero-dependency typed thunks otherwise.
	return defaultExtCallMode();
}

const char* extCallModeName(ExtCallMode mode) {
	switch (mode) {
	case ExtCallMode::Dyncall:
		return "dyncall";
	case ExtCallMode::Thunks:
		return "thunks";
	}
	return "unknown";
}

/// Bind every external call site (CALL_EXT and CALL_IND share the record
/// shape; internal CALL sites never reach the external path) to a typed
/// thunk. Sites whose signature exceeds the thunk caps degrade per-site to
/// dyncall when it is built; without dyncall they are rejected here, at
/// compile time, rather than at run time mid-execution.
void resolveExtCallThunks(TBCProgram& program) {
	for (auto& site : program.callsites) {
		if (site.internalFnIdx != ~0u) {
			continue;
		}
		resolveExtCallThunk(site);
#ifndef NAUTILUS_TBC_DYNCALL
		if (site.ext.fn == nullptr) {
			throw NotImplementedException(
			    "tbc: external call signature exceeds the typed-thunk limits (at most 8 integer-class and 4 "
			    "float-class arguments, x86-64/AArch64 only) and dyncall is not built; rebuild with "
			    "-DENABLE_TBC_DYNCALL=ON to marshal such calls through dyncall");
		}
#endif
	}
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
	program->extCall =
	    clampExtCallMode(parseExtCallMode(options.getOptionOrDefault<std::string>("tbc.externalCall", "auto")));
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

		if (funcOp->getName() == "execute") {
			dumpHandler.dump("after_tbc_generation", "tbc", [&function]() { return function.toString(); });
		}
		totalInstructions += static_cast<int64_t>(function.code.size());
		maxRegisters = std::max<int64_t>(maxRegisters, function.regSlots);
	}

	if (program->extCall == ExtCallMode::Thunks) {
		resolveExtCallThunks(*program);
	}

	if (statistics != nullptr) {
		statistics->set("tbc.instructions", totalInstructions);
		statistics->set("tbc.codeSize.bytes", totalInstructions * static_cast<int64_t>(sizeof(Instr)));
		statistics->set("tbc.registers.max", maxRegisters);
		statistics->set("tbc.dispatch", std::string(dispatchModeName(program->dispatch)));
		statistics->set("tbc.externalCall", std::string(extCallModeName(program->extCall)));
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}

	return std::make_unique<TBCExecutable>(std::move(program));
}

} // namespace nautilus::compiler::tbc
