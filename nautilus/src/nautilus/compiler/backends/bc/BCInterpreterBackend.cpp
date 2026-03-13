

#include <cstdint>
#include <dyncall_callback.h>
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
                                                          const engine::Options&) const {
	const auto& functionOperations = ir->getFunctionOperations();

	std::unordered_map<std::string, void*> functionPtrs;
	std::vector<std::unique_ptr<BCCallbackData>> callbackDataStore;
	std::vector<DCCallback*> callbackPtrs;

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

	// Phase 2: Lower all functions to bytecode and set the interpreter.
	// All function pointers are now available, so every function can call any other.
	for (size_t i = 0; i < functionOperations.size(); i++) {
		const auto& funcOp = functionOperations[i];
		auto result = BCLoweringProvider().lower(ir, funcOp->getName(), functionPtrs);
		auto& code = std::get<0>(result);
		auto& regFile = std::get<1>(result);

		if (funcOp->getName() == "execute") {
			dumpHandler.dump("after_bc_generation", "bc", [&code]() { return code.toString(); });
		}

		callbackDataStore[i]->interpreter = std::make_unique<BCInterpreter>(std::move(code), std::move(regFile));
	}

	return std::make_unique<BCExecutable>(std::move(functionPtrs), std::move(callbackDataStore),
	                                      std::move(callbackPtrs));
}

} // namespace nautilus::compiler::bc
