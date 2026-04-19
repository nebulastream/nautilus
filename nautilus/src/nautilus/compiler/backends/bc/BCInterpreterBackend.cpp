

#include <chrono>
#include <cstdint>
#include <dyncall_callback.h>
#include <nautilus/CompilationStatistics.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreterBackend.hpp>
#include <nautilus/compiler/backends/bc/BCLoweringProvider.hpp>
#include <nautilus/compiler/backends/bc/ByteCode.hpp>
#include <nautilus/compiler/ir/operations/FunctionOperation.hpp>

namespace nautilus::compiler::bc {

static char typeToDCSigChar(Type type) {
	switch (type) {
	case Type::b:
		return DC_SIGCHAR_BOOL;
	case Type::i8:
		return DC_SIGCHAR_CHAR;
	case Type::i16:
		return DC_SIGCHAR_SHORT;
	case Type::i32:
		return DC_SIGCHAR_INT;
	case Type::i64:
		return DC_SIGCHAR_LONG;
	case Type::ui8:
		return DC_SIGCHAR_UCHAR;
	case Type::ui16:
		return DC_SIGCHAR_USHORT;
	case Type::ui32:
		return DC_SIGCHAR_UINT;
	case Type::ui64:
		return DC_SIGCHAR_ULONG;
	case Type::f32:
		return DC_SIGCHAR_FLOAT;
	case Type::f64:
		return DC_SIGCHAR_DOUBLE;
	case Type::ptr:
		return DC_SIGCHAR_POINTER;
	case Type::v:
		return DC_SIGCHAR_VOID;
	}
	return DC_SIGCHAR_VOID;
}

static std::string buildDCSignature(const std::vector<Type>& argTypes, Type returnType) {
	std::string sig;
	for (auto t : argTypes) {
		sig += typeToDCSigChar(t);
	}
	sig += DC_SIGCHAR_ENDARG;
	sig += typeToDCSigChar(returnType);
	return sig;
}

/// dyncallback handler: reads arguments directly into the interpreter's register file, executes, and writes result.
static DCsigchar bcCallbackHandler(DCCallback* /*pcb*/, DCArgs* args, DCValue* result, void* userdata) {
	auto* data = static_cast<BCCallbackData*>(userdata);
	auto raw = data->interpreter->invoke(args, data->argTypes);

	// Zero the result union so the full 64-bit register value is clean.
	// The dyncall ARM64 callback thunk loads all 8 bytes into x0 via "ldr x0, [sp, #offset]",
	// so sub-32-bit fields (c, s, C, S) must not leave garbage in the upper bytes.
	result->l = 0;

	switch (data->returnType) {
	case Type::b:
		result->l = static_cast<bool>(raw);
		return DC_SIGCHAR_BOOL;
	case Type::i8:
		result->l = static_cast<int8_t>(raw);
		return DC_SIGCHAR_CHAR;
	case Type::i16:
		result->l = static_cast<int16_t>(raw);
		return DC_SIGCHAR_SHORT;
	case Type::i32:
		result->l = static_cast<int32_t>(raw);
		return DC_SIGCHAR_INT;
	case Type::i64:
		result->l = raw;
		return DC_SIGCHAR_LONG;
	case Type::ui8:
		result->l = static_cast<uint8_t>(raw);
		return DC_SIGCHAR_UCHAR;
	case Type::ui16:
		result->l = static_cast<uint16_t>(raw);
		return DC_SIGCHAR_USHORT;
	case Type::ui32:
		result->l = static_cast<uint32_t>(raw);
		return DC_SIGCHAR_UINT;
	case Type::ui64:
		result->l = static_cast<uint64_t>(raw);
		return DC_SIGCHAR_ULONG;
	case Type::f32:
		result->f = *reinterpret_cast<float*>(&raw);
		return DC_SIGCHAR_FLOAT;
	case Type::f64:
		result->d = *reinterpret_cast<double*>(&raw);
		return DC_SIGCHAR_DOUBLE;
	case Type::ptr:
		result->p = reinterpret_cast<void*>(raw);
		return DC_SIGCHAR_POINTER;
	case Type::v:
		return DC_SIGCHAR_VOID;
	}
	return DC_SIGCHAR_VOID;
}

std::unique_ptr<Executable> BCInterpreterBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                          const DumpHandler& dumpHandler,
                                                          const engine::Options& options,
                                                          CompilationStatistics* statistics) const {
	const auto backendStart = std::chrono::steady_clock::now();
	const auto& functionOperations = ir->getFunctionOperations();

	std::unordered_map<std::string, void*> functionPtrs;
	std::vector<std::unique_ptr<BCCallbackData>> callbackDataStore;
	std::vector<DCCallback*> callbackPtrs;

	// Lowering-time option: the simple linear register allocator is
	// enabled by default but can be turned off via "bc.registerAllocator"
	// for A/B benchmarking or if a caller wants to reproduce the legacy
	// one-slot-per-value behaviour.
	LoweringOptions loweringOptions;
	loweringOptions.enableRegisterAllocator = options.getOptionOrDefault("bc.registerAllocator", true);

	// Phase 1: Allocate callback data and dyncallback thunks for all functions.
	// The interpreter is not yet set — we need all function pointers resolved first.
	for (const auto& funcOp : functionOperations) {
		auto data = std::make_unique<BCCallbackData>();
		const auto& bbArgs = funcOp->getFunctionBasicBlock().getArguments();
		for (const auto& arg : bbArgs) {
			data->argTypes.push_back(arg->getStamp());
		}
		data->returnType = funcOp->getOutputArg();

		auto sig = buildDCSignature(data->argTypes, data->returnType);
		auto* cb = dcbNewCallback(sig.c_str(), bcCallbackHandler, data.get());
		functionPtrs[funcOp->getName()] = reinterpret_cast<void*>(cb);
		callbackPtrs.push_back(cb);
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

	// Phase 2: Lower all functions to bytecode and set the interpreter.
	// All function pointers are now available, so every function can call any other.
	for (size_t i = 0; i < functionOperations.size(); i++) {
		const auto& funcOp = functionOperations[i];
		auto result = BCLoweringProvider().lower(ir, funcOp->getName(), functionPtrs, loweringOptions);
		auto& code = std::get<0>(result);
		auto& regFile = std::get<1>(result);

		if (funcOp->getName() == "execute") {
			dumpHandler.dump("after_bc_generation", "bc", [&code]() { return code.toString(); });
		}

		if (statistics != nullptr) {
			for (const auto& block : code.blocks) {
				totalInstructions += static_cast<int64_t>(block.code.size());
			}
			const auto regCount = static_cast<int64_t>(regFile.size());
			totalRegisters += regCount;
			maxRegisters = std::max(maxRegisters, regCount);
		}

		callbackDataStore[i]->interpreter = std::make_unique<BCInterpreter>(std::move(code), std::move(regFile));
	}

	if (statistics != nullptr) {
		statistics->set("bc.instructions", totalInstructions);
		statistics->set("bc.codeSize.bytes", totalInstructions * static_cast<int64_t>(sizeof(OpCode)));
		statistics->set("bc.registers.total", totalRegisters);
		statistics->set("bc.registers.max", maxRegisters);
		statistics->set("bc.registerAllocator.enabled",
		                std::string(loweringOptions.enableRegisterAllocator ? "true" : "false"));
		statistics->recordTimingMs("backend.totalMs", backendStart);
	}

	return std::make_unique<BCExecutable>(std::move(functionPtrs), std::move(callbackDataStore),
	                                      std::move(callbackPtrs));
}

} // namespace nautilus::compiler::bc
