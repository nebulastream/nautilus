
#include "nautilus/compiler/backends/tbc/TBCBackend.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/backends/tbc/TBCClosure.hpp"
#include "nautilus/compiler/backends/tbc/TBCExecutable.hpp"
#include "nautilus/compiler/backends/tbc/TBCInterpreter.hpp"
#include "nautilus/compiler/backends/tbc/TBCLoweringProvider.hpp"
#include "nautilus/compiler/backends/tbc/TBCNirBackend.hpp"
#include "nautilus/compiler/backends/tbc/TBCProgramSetup.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/config.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <chrono>
#ifdef ENABLE_TBC_JIT
#include "nautilus/compiler/backends/tbc/jit/TBCJit.hpp"
#endif

namespace nautilus::compiler::tbc {

std::unique_ptr<Executable> TBCBackend::compile(const std::shared_ptr<ir::IRGraph>& ir, const DumpHandler& dumpHandler,
                                                const engine::Options& options,
                                                CompilationStatistics* statistics) const {
	// The graph of the NIR front end carries the module and no IR: it compiles natively (M4.1)
	if (ir->getNirModule() != nullptr && ir->getFunctionOperations().empty()) {
		auto module = std::static_pointer_cast<const nir::Module>(ir->getNirModule());
		// the traced module is process-local: its pointers are addresses, so the host resolves nothing
		const nirbackend::CompileContext context {dumpHandler, options, statistics, nullptr, {}};
		return TBCNirBackend().compile(std::move(module), context);
	}
	const auto backendStart = std::chrono::steady_clock::now();
	const auto& functionOperations = ir->getFunctionOperations();

	// All optimizations default to ON; the options exist for A/B comparisons.
	TBCLoweringOptions loweringOptions;
	loweringOptions.enableRegisterAllocator = options.getOptionOrDefault("tbc.registerAllocator", true);
	loweringOptions.enableRegisterCoalescing = options.getOptionOrDefault("tbc.coalescing", true);
	loweringOptions.enableSuperinstructions = options.getOptionOrDefault("tbc.superinstructions", true);
	loweringOptions.enableImmediates = options.getOptionOrDefault("tbc.immediates", true);

	auto program = std::make_shared<TBCProgram>();
	configureProgram(*program, options);

	// The function index and every function's signature must exist before any
	// lowering runs: calls and FunctionAddressOf between module functions
	// resolve to interpreter-native targets (or native closures) regardless of
	// lowering order.
	program->functions.resize(functionOperations.size());
	for (size_t i = 0; i < functionOperations.size(); i++) {
		const auto& funcOp = functionOperations[i];
		program->functionIndex[funcOp->getName()] = static_cast<uint32_t>(i);
		program->functionSlotById[ir->getFunctionTable().findByDefinition(funcOp)] = static_cast<uint32_t>(i);
		auto& function = program->functions[i];
		function.name = funcOp->getName();
		function.returnType = funcOp->getOutputArg();
		for (const auto* argument : funcOp->getFunctionBasicBlock().getArguments()) {
			function.argTypes.push_back(argument->getStamp());
		}
	}

	// Native entry points for every function. Built before lowering so
	// FunctionAddressOf resolves in any lowering order, and before stitching so
	// the closure bodies observe the final program.
	createClosures(*program);

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

	finishProgram(*program, options, statistics, totalInstructions, maxRegisters, backendStart);

	return std::make_unique<TBCExecutable>(std::move(program),
	                                       CapturedExceptionTransport::functionsNeedingCapture(*ir));
}

} // namespace nautilus::compiler::tbc
