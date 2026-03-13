

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

/// dyncallback handler: reads arguments from DCArgs, invokes the BCInterpreter, writes result to DCValue.
static DCsigchar bcCallbackHandler(DCCallback* /*pcb*/, DCArgs* args, DCValue* result, void* userdata) {
	auto* data = static_cast<BCCallbackData*>(userdata);

	std::vector<std::any> anyArgs;
	anyArgs.reserve(data->argTypes.size());
	for (auto type : data->argTypes) {
		switch (type) {
		case Type::b:
			anyArgs.push_back(static_cast<bool>(dcbArgBool(args)));
			break;
		case Type::i8:
			anyArgs.push_back(static_cast<int8_t>(dcbArgChar(args)));
			break;
		case Type::i16:
			anyArgs.push_back(static_cast<int16_t>(dcbArgShort(args)));
			break;
		case Type::i32:
			anyArgs.push_back(static_cast<int32_t>(dcbArgInt(args)));
			break;
		case Type::i64:
			anyArgs.push_back(static_cast<int64_t>(dcbArgLong(args)));
			break;
		case Type::ui8:
			anyArgs.push_back(static_cast<uint8_t>(dcbArgUChar(args)));
			break;
		case Type::ui16:
			anyArgs.push_back(static_cast<uint16_t>(dcbArgUShort(args)));
			break;
		case Type::ui32:
			anyArgs.push_back(static_cast<uint32_t>(dcbArgUInt(args)));
			break;
		case Type::ui64:
			anyArgs.push_back(static_cast<uint64_t>(dcbArgULong(args)));
			break;
		case Type::f32:
			anyArgs.push_back(static_cast<float>(dcbArgFloat(args)));
			break;
		case Type::f64:
			anyArgs.push_back(static_cast<double>(dcbArgDouble(args)));
			break;
		case Type::ptr:
			anyArgs.push_back(static_cast<void*>(dcbArgPointer(args)));
			break;
		default:
			break;
		}
	}

	auto ret = data->interpreter->invokeGeneric(anyArgs);

	switch (data->returnType) {
	case Type::b:
		result->B = std::any_cast<bool>(ret);
		return DC_SIGCHAR_BOOL;
	case Type::i8:
		result->c = std::any_cast<int8_t>(ret);
		return DC_SIGCHAR_CHAR;
	case Type::i16:
		result->s = std::any_cast<int16_t>(ret);
		return DC_SIGCHAR_SHORT;
	case Type::i32:
		result->i = std::any_cast<int32_t>(ret);
		return DC_SIGCHAR_INT;
	case Type::i64:
		result->l = std::any_cast<int64_t>(ret);
		return DC_SIGCHAR_LONG;
	case Type::ui8:
		result->C = std::any_cast<uint8_t>(ret);
		return DC_SIGCHAR_UCHAR;
	case Type::ui16:
		result->S = std::any_cast<uint16_t>(ret);
		return DC_SIGCHAR_USHORT;
	case Type::ui32:
		result->I = std::any_cast<uint32_t>(ret);
		return DC_SIGCHAR_UINT;
	case Type::ui64:
		result->L = std::any_cast<uint64_t>(ret);
		return DC_SIGCHAR_ULONG;
	case Type::f32:
		result->f = std::any_cast<float>(ret);
		return DC_SIGCHAR_FLOAT;
	case Type::f64:
		result->d = std::any_cast<double>(ret);
		return DC_SIGCHAR_DOUBLE;
	case Type::ptr:
		result->p = std::any_cast<void*>(ret);
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
		auto [code, regFile] = BCLoweringProvider().lower(ir, funcOp->getName(), functionPtrs);

		if (funcOp->getName() == "execute") {
			dumpHandler.dump("after_bc_generation", "bc", [&code]() { return code.toString(); });
		}

		callbackDataStore[i]->interpreter = std::make_unique<BCInterpreter>(std::move(code), std::move(regFile));
	}

	return std::make_unique<BCExecutable>(std::move(functionPtrs), std::move(callbackDataStore),
	                                      std::move(callbackPtrs));
}

} // namespace nautilus::compiler::bc
