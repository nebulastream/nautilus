

#include <chrono>
#include <cstdint>
#include <cstring>
#include <nautilus/CompilationStatistics.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreter.hpp>
#include <nautilus/compiler/backends/bc/BCInterpreterBackend.hpp>
#include <nautilus/compiler/backends/bc/BCLoweringProvider.hpp>
#include <nautilus/compiler/backends/bc/ByteCode.hpp>
#include <nautilus/compiler/ir/operations/FunctionOperation.hpp>
#include <nautilus/config.hpp>
#include <stdexcept>

#ifdef NAUTILUS_BC_LIBFFI
#include <ffi.h>
#else
#include <dyncall_callback.h>
#endif

namespace nautilus::compiler::bc {

#ifdef NAUTILUS_BC_LIBFFI

/// Map a nautilus Type to the libffi type used in the closure signature.
static ffi_type* typeToFFIType(Type type) {
	switch (type) {
	case Type::b:
		// C++ bool is one byte; there is no ffi_type_bool.
		return &ffi_type_uint8;
	case Type::i8:
		return &ffi_type_sint8;
	case Type::i16:
		return &ffi_type_sint16;
	case Type::i32:
		return &ffi_type_sint32;
	case Type::i64:
		return &ffi_type_sint64;
	case Type::ui8:
		return &ffi_type_uint8;
	case Type::ui16:
		return &ffi_type_uint16;
	case Type::ui32:
		return &ffi_type_uint32;
	case Type::ui64:
		return &ffi_type_uint64;
	case Type::f32:
		return &ffi_type_float;
	case Type::f64:
		return &ffi_type_double;
	case Type::ptr:
		return &ffi_type_pointer;
	case Type::v:
		return &ffi_type_void;
	}
	return &ffi_type_void;
}

/// libffi closure handler: reads arguments from the libffi argument array into the
/// interpreter's register file, executes, and writes the typed result to `ret`.
static void bcFFIHandler(ffi_cif* /*cif*/, void* ret, void** args, void* userdata) {
	auto* data = static_cast<BCCallbackData*>(userdata);
	auto raw = data->interpreter->invoke(args, data->argTypes);

	switch (data->returnType) {
	case Type::v:
		return;
	case Type::b:
		// Integral/pointer returns must be written at ffi_arg width; cast through
		// the concrete type first so libffi's truncation yields the right value.
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<bool>(raw));
		return;
	case Type::i8:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int8_t>(raw));
		return;
	case Type::i16:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int16_t>(raw));
		return;
	case Type::i32:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int32_t>(raw));
		return;
	case Type::i64:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<int64_t>(raw));
		return;
	case Type::ui8:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint8_t>(raw));
		return;
	case Type::ui16:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint16_t>(raw));
		return;
	case Type::ui32:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint32_t>(raw));
		return;
	case Type::ui64:
		*static_cast<ffi_arg*>(ret) = static_cast<ffi_arg>(static_cast<uint64_t>(raw));
		return;
	case Type::f32: {
		// The result register holds the float's bit pattern; reinterpret, do not
		// numerically convert.
		float value;
		std::memcpy(&value, &raw, sizeof(float));
		*static_cast<float*>(ret) = value;
		return;
	}
	case Type::f64: {
		double value;
		std::memcpy(&value, &raw, sizeof(double));
		*static_cast<double*>(ret) = value;
		return;
	}
	case Type::ptr:
		*static_cast<void**>(ret) = reinterpret_cast<void*>(raw);
		return;
	}
}

#else

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

#endif // NAUTILUS_BC_LIBFFI

std::unique_ptr<Executable> BCInterpreterBackend::compile(const std::shared_ptr<ir::IRGraph>& ir,
                                                          const DumpHandler& dumpHandler,
                                                          const engine::Options& options,
                                                          CompilationStatistics* statistics) const {
	const auto backendStart = std::chrono::steady_clock::now();
	const auto& functionOperations = ir->getFunctionOperations();

	std::unordered_map<std::string, void*> functionPtrs;
	std::vector<std::unique_ptr<BCCallbackData>> callbackDataStore;
	std::vector<BCClosureHandle> callbackPtrs;

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

#ifdef NAUTILUS_BC_LIBFFI
		// Build a libffi closure with a static-trampoline thunk (no runtime RWX
		// memory). The cif and its argument-type array are stored in `data` so they
		// outlive the closure.
		data->argFFITypes.reserve(data->argTypes.size());
		for (auto argType : data->argTypes) {
			data->argFFITypes.push_back(typeToFFIType(argType));
		}
		void* code = nullptr;
		data->closure = static_cast<ffi_closure*>(ffi_closure_alloc(sizeof(ffi_closure), &code));
		if (data->closure == nullptr) {
			throw std::runtime_error("Failed to allocate libffi closure for bytecode function");
		}
		if (ffi_prep_cif(&data->cif, FFI_DEFAULT_ABI, static_cast<unsigned int>(data->argFFITypes.size()),
		                 typeToFFIType(data->returnType), data->argFFITypes.data()) != FFI_OK) {
			throw std::runtime_error("Failed to prepare libffi call interface for bytecode function");
		}
		if (ffi_prep_closure_loc(data->closure, &data->cif, bcFFIHandler, data.get(), code) != FFI_OK) {
			throw std::runtime_error("Failed to prepare libffi closure for bytecode function");
		}
		data->code = code;
		functionPtrs[funcOp->getName()] = code;
		callbackPtrs.push_back(data->closure);
#else
		auto sig = buildDCSignature(data->argTypes, data->returnType);
		auto* cb = dcbNewCallback(sig.c_str(), bcCallbackHandler, data.get());
		functionPtrs[funcOp->getName()] = reinterpret_cast<void*>(cb);
		callbackPtrs.push_back(cb);
#endif
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

		callbackDataStore[i]->interpreter =
		    std::make_unique<BCInterpreter>(std::move(code), std::move(regFile), interpreterOptions);
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

	return std::make_unique<BCExecutable>(std::move(functionPtrs), std::move(callbackDataStore),
	                                      std::move(callbackPtrs));
}

} // namespace nautilus::compiler::bc
