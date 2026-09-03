

#include <chrono>
#include <cstdint>
#include <cstring>
#include <nautilus/CompilationStatistics.hpp>
#include <nautilus/compiler/backends/CapturedExceptionTransport.hpp>
#include <nautilus/compiler/backends/NativeClosure.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreterBackend.hpp>
#include <nautilus/compiler/backends/bc/BCLoweringProvider.hpp>
#include <nautilus/compiler/backends/bc/ByteCode.hpp>
#include <nautilus/compiler/ir/operations/FunctionOperation.hpp>
#include <nautilus/config.hpp>
#include <stdexcept>

namespace nautilus::compiler::bc {

/// Closure body for one bytecode function: reads the incoming arguments straight
/// into the interpreter's register file, executes, and returns the raw result
/// slot. The typed return writeback and the argument decoding both live in the
/// shared NativeClosure component.
static uint64_t bcClosureBody(ClosureArgs& args, void* userdata) {
	auto* data = static_cast<BCCallbackData*>(userdata);
	return static_cast<uint64_t>(data->interpreter->invoke(args, data->argTypes));
}

std::unique_ptr<Executable> BCInterpreterBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                          const DumpHandler& dumpHandler,
                                                          const engine::Options& options,
                                                          CompilationStatistics* statistics) const {
	const auto backendStart = std::chrono::steady_clock::now();
	const auto& functionOperations = ir->getFunctionOperations();

	// Name -> thunk, handed to BCExecutable so a caller can invoke a module
	// function by the name they gave it.
	std::unordered_map<std::string, void*> functionPtrs;
	// FunctionId -> thunk, handed to lowering. Separate from the map above
	// because lowering must resolve a callee by identity: a name lookup that
	// missed used to fall through and hand dyncall a NautilusFunctionDefinition
	// to call as if it were code.
	std::unordered_map<ir::FunctionId, void*> internalPtrsById;
	std::vector<std::unique_ptr<BCCallbackData>> callbackDataStore;
	std::vector<std::unique_ptr<NativeClosure>> closures;

	// Lowering-time option: the simple linear register allocator is
	// enabled by default but can be turned off via "bc.registerAllocator"
	// for A/B benchmarking or if a caller wants to reproduce the legacy
	// one-slot-per-value behaviour. "bc.registerCoalescing" opts into
	// sequencing block-invocation arguments as a parallel copy (fewer
	// REG_MOVs on loop back-edges) instead of always staging through a
	// temp; off by default so existing callers see no behavior change.
	LoweringOptions loweringOptions;
	loweringOptions.enableRegisterAllocator = options.getOptionOrDefault("bc.registerAllocator", true);
	loweringOptions.enableRegisterCoalescing = options.getOptionOrDefault("bc.registerCoalescing", false);

	// Execution-time options for the interpreter, mirroring the bc.registerAllocator
	// plumbing so the A/B benchmark harness can compare each in isolation.
	//   bc.dispatch          how each op is dispatched (call / switch / threaded)
	//   bc.regfileReuse      recycle the per-invocation register file from a pool
	//   bc.superinstructions fuse compare+branch in the threaded stream
	//   bc.immediates        fold constant operands in the threaded stream
	// All default off/legacy so existing callers see no behavior change; opt in
	// per option (or all four -- "threaded" with reuse/superinstructions/immediates
	// is the fastest validated combination, see BCDispatchModeTest's
	// {threaded,true,true,true} case, checked against the "call" baseline) for the
	// speedup.
	BCInterpreterOptions interpreterOptions;
	interpreterOptions.dispatch = parseDispatchMode(options.getOptionOrDefault<std::string>("bc.dispatch", "call"));
	interpreterOptions.reuseRegisterFile = options.getOptionOrDefault("bc.regfileReuse", false);
	interpreterOptions.superinstructions = options.getOptionOrDefault("bc.superinstructions", false);
	interpreterOptions.immediates = options.getOptionOrDefault("bc.immediates", false);

	// Phase 1: Allocate callback data and closure thunks for all functions.
	// The interpreter is not yet set — we need all function pointers resolved first.
	for (const auto& funcOp : functionOperations) {
		auto data = std::make_unique<BCCallbackData>();
		const auto& bbArgs = funcOp->getFunctionBasicBlock().getArguments();
		for (const auto& arg : bbArgs) {
			data->argTypes.push_back(arg->getStamp());
		}
		data->returnType = funcOp->getOutputArg();

		auto closure = std::make_unique<NativeClosure>(data->argTypes, data->returnType, bcClosureBody, data.get());
		functionPtrs[funcOp->getName()] = closure->code();
		internalPtrsById[ir->getFunctionTable().findByDefinition(funcOp)] = closure->code();
		closures.push_back(std::move(closure));
		callbackDataStore.push_back(std::move(data));
	}

	// Track total emitted opcodes across all functions for backend-level
	// code-size reporting. Each OpCode is fixed-width so total bytes is a
	// straightforward multiple of the instruction count. The peak register
	// file size across functions captures the linear-allocator's main win
	// (smaller per-invocation register file copy).
	int64_t totalInstructions = 0;
	int64_t totalRegisters = 0;
	int64_t maxRegisters = 0;

	// The dump must cover every module function, not just a fixed "execute"
	// entry point (module-API functions carry user-chosen names). Code is
	// moved into the interpreter below, so the text is aggregated per
	// function and dumped once after the loop.
	std::string bytecodeDump;
	const bool dumpBytecode = dumpHandler.shouldDump("after_bc_generation");

	// Phase 2: Lower all functions to bytecode and set the interpreter.
	// All function pointers are now available, so every function can call any other.
	for (size_t i = 0; i < functionOperations.size(); i++) {
		const auto& funcOp = functionOperations[i];
		auto result = BCLoweringProvider().lower(ir, funcOp->getName(), internalPtrsById, loweringOptions);
		auto& code = std::get<0>(result);
		auto& regFile = std::get<1>(result);

		if (dumpBytecode) {
			bytecodeDump += "// function " + funcOp->getName() + "\n" + code.toString() + "\n";
		}

		if (statistics != nullptr) {
			for (const auto& block : code.blocks) {
				totalInstructions += static_cast<int64_t>(block.code.size());
			}
			const auto regCount = static_cast<int64_t>(regFile.size());
			totalRegisters += regCount;
			maxRegisters = std::max(maxRegisters, regCount);
		}

		callbackDataStore[i]->interpreter =
		    std::make_unique<BCInterpreter>(std::move(code), std::move(regFile), interpreterOptions);
	}

	if (dumpBytecode) {
		dumpHandler.dump("after_bc_generation", "bc", [&bytecodeDump]() { return bytecodeDump; });
	}

	if (statistics != nullptr) {
		statistics->set("bc.instructions", totalInstructions);
		statistics->set("bc.codeSize.bytes", totalInstructions * static_cast<int64_t>(sizeof(OpCode)));
		statistics->set("bc.registers.total", totalRegisters);
		statistics->set("bc.registers.max", maxRegisters);
		statistics->set("bc.registerAllocator.enabled",
		                std::string(loweringOptions.enableRegisterAllocator ? "true" : "false"));
		const auto* dispatchName = interpreterOptions.dispatch == DispatchMode::Threaded ? "threaded"
		                           : interpreterOptions.dispatch == DispatchMode::Switch ? "switch"
		                                                                                 : "call";
		statistics->set("bc.dispatch", std::string(dispatchName));
		statistics->set("bc.regfileReuse", std::string(interpreterOptions.reuseRegisterFile ? "true" : "false"));
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}

	return std::make_unique<BCExecutable>(std::move(functionPtrs), std::move(callbackDataStore), std::move(closures),
	                                      CapturedExceptionTransport::functionsNeedingCapture(*ir));
}

} // namespace nautilus::compiler::bc
