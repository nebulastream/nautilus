#include "ExecutionTrace.hpp"
#include "TracerTrace.hpp"
#include <cxxabi.h>
#include <dlfcn.h>

namespace nautilus::tracing {

std::string getMangledName(void* fnptr) {
	return fmt::format("{}", fmt::ptr(fnptr));
}

std::string getFunctionName(const std::string& mangledName) {
	// Try to demangle the function name for human-readable output
	int status;
	char* demangled = __cxxabiv1::__cxa_demangle(mangledName.c_str(), nullptr, nullptr, &status);
	if (status == 0 && demangled != nullptr) {
		// Demangling succeeded
		std::string result(demangled);
		std::free(demangled);
		return result;
	}

	// Demangling failed, return the mangled name
	return mangledName;
}

/// Convert a Trace::Operation to InputVariant vector for ExecutionTrace
std::vector<InputVariant> convertOperationInputs(Trace::Operation& op) {
	auto* header = op.view;
	auto* inputPtr = reinterpret_cast<char*>(header) + sizeof(Trace::OperationHeader);

	switch (header->op) {
	case ADD:
	case SUB:
	case MUL:
	case DIV:
	case MOD:
	case EQ:
	case NEQ:
	case LT:
	case LTE:
	case GT:
	case GTE:
	case AND:
	case OR:
	case LSH:
	case RSH:
	case BOR:
	case BXOR:
	case BAND: {
		auto* input = reinterpret_cast<BinaryInput*>(inputPtr);
		return {input->lhs, input->rhs};
	}

	case NOT:
	case NEGATE:
	case CAST: {
		auto* input = reinterpret_cast<UnaryInput*>(inputPtr);
		return {input->operand};
	}

	case CONST: {
		auto* input = reinterpret_cast<ConstantLiteral*>(inputPtr);
		return {*input};
	}

	case JMP: {
		auto* input = reinterpret_cast<JmpInput*>(inputPtr);
		return {BlockRef(input->targetBlock)};
	}

	case CMP: {
		auto* input = reinterpret_cast<CmpInput*>(inputPtr);
		return {input->condition, BlockRef(input->trueBlock), BlockRef(input->falseBlock), input->probability};
	}

	case RETURN: {
		auto* input = reinterpret_cast<TypedValueRef*>(inputPtr);
		if (input->type == Type::v) {
			return {};
		}
		return {*input};
	}

	case ASSIGN: {
		auto* input = reinterpret_cast<TypedValueRef*>(inputPtr);
		return {*input};
	}

	case LOAD: {
		auto* input = reinterpret_cast<MemoryInput*>(inputPtr);
		return {input->ptr};
	}

	case STORE: {
		auto* input = reinterpret_cast<MemoryInput*>(inputPtr);
		return {input->ptr, input->value};
	}

	case CALL: {
		auto* input = reinterpret_cast<CallInput*>(inputPtr);
		auto* args = reinterpret_cast<TypedValueRef*>(inputPtr + sizeof(CallInput));
		std::vector<TypedValueRef> callArgs;
		for (uint8_t i = 0; i < input->argCount; i++) {
			callArgs.push_back(args[i]);
		}
		auto mangled = getMangledName(input->funcPtr);
		FunctionCall fc {getFunctionName(mangled), mangled, input->funcPtr, std::move(callArgs), input->attrs};
		return {std::move(fc)};
	}

	case FREE: {
		auto* input = reinterpret_cast<TypedValueRef*>(inputPtr);
		return {*input};
	}

	default:
		return {};
	}
}

ExecutionTrace toExecutionTrace(Trace& trace) {
	ExecutionTrace execTrace;

	auto blocks = trace.getBlocks();

	// Create all blocks first
	for (size_t i = 0; i < blocks.size() - 1; i++) {
		execTrace.createBlock();
	}

	for (const auto& arg : trace.getArguments()) {
		execTrace.blocks.front().addArgument(arg);
	}

	// Convert operations for each block
	for (size_t blockIdx = 0; blockIdx < blocks.size(); blockIdx++) {
		auto& srcBlock = blocks[blockIdx];
		auto& dstBlock = execTrace.getBlock(blockIdx);
		if (srcBlock.view->controlFlowMerge) {
			dstBlock.type = Block::Type::ControlFlowMerge;
		}

		auto operations = srcBlock.getOperations();

		for (auto& op : operations) {
			auto* header = op.view;
			auto inputs = convertOperationInputs(op);

			if (header->op == RETURN) {
				execTrace.returnRefs.push_back(
				    {static_cast<uint16_t>(blockIdx), static_cast<uint16_t>(dstBlock.operations.size())});
			}

			if (header->op == CMP) {
				auto trueBlock = std::get<tracing::BlockRef>(inputs.at(1));
				auto falseBlock = std::get<tracing::BlockRef>(inputs.at(2));
				execTrace.blocks.at(static_cast<size_t>(trueBlock.block))
				    .predecessors.emplace_back(static_cast<uint16_t>(blockIdx));
				execTrace.blocks.at(static_cast<size_t>(falseBlock.block))
				    .predecessors.emplace_back(static_cast<uint16_t>(blockIdx));
			}

			if (header->op == JMP) {
				auto target = std::get<tracing::BlockRef>(inputs.at(0));
				execTrace.blocks.at(static_cast<size_t>(target.block))
				    .predecessors.emplace_back(static_cast<uint16_t>(blockIdx));
			}

			TraceOperation traceOp(header->tag, header->op, header->resultRef.type, header->resultRef,
			                       std::move(inputs));
			dstBlock.addOperation(std::move(traceOp));
		}
	}

	// Set value ref counter
	execTrace.lastValueRef = trace.view->valueRefCounter;

	return execTrace;
}

} // namespace nautilus::tracing
