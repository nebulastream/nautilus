#include "TracerTrace.hpp"
#include <fmt/chrono.h>
#include <fmt/format.h>
#include <sstream>
#include <string>
#include <utility>

namespace nautilus::tracing {

// Helper to format a TypedValueRef as "$N:type"
std::string formatRef(const TypedValueRef& ref) {
	std::ostringstream ss;
	ss << "$" << ref.ref << ":" << toString(ref.type);
	return ss.str();
}

// Helper to format a ConstantLiteral value
std::string formatConstant(const ConstantLiteral& lit) {
	return std::visit(
	    [](auto&& value) -> std::string {
		    using T = std::decay_t<decltype(value)>;
		    std::ostringstream ss;
		    if constexpr (std::is_same_v<T, bool>) {
			    ss << (value ? "true" : "false");
		    } else if constexpr (std::is_same_v<T, void*>) {
			    ss << value;
		    } else {
			    ss << value;
		    }
		    return ss.str();
	    },
	    lit);
}

// Format a single operation
std::string formatOperation(Trace::Operation& op) {
	std::ostringstream ss;
	auto* header = op.view;
	ss << "\t" << toString(header->op);

	// Get pointer to input data after header
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
		ss << "\t" << formatRef(header->resultRef) << "\t" << formatRef(input->lhs) << "\t" << formatRef(input->rhs);
		break;
	}

	case NOT:
	case NEGATE: {
		auto* input = reinterpret_cast<UnaryInput*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << "\t" << formatRef(input->operand);
		break;
	}

	case CAST: {
		auto* input = reinterpret_cast<UnaryInput*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << "\t" << formatRef(input->operand);
		break;
	}

	case CONST: {
		auto* input = reinterpret_cast<ConstantLiteral*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << "\t" << formatConstant(*input);
		break;
	}

	case JMP: {
		auto* input = reinterpret_cast<JmpInput*>(inputPtr);
		ss << "\tB" << input->targetBlock;
		break;
	}

	case CMP: {
		auto* input = reinterpret_cast<CmpInput*>(inputPtr);
		ss << "\t" << formatRef(input->condition) << "\tB" << input->trueBlock << "\tB" << input->falseBlock;
		break;
	}

	case RETURN: {
		auto* input = reinterpret_cast<TypedValueRef*>(inputPtr);
		if (input->type != Type::v) {
			ss << "\t" << formatRef(*input);
		}
		break;
	}

	case ASSIGN: {
		auto* input = reinterpret_cast<TypedValueRef*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << "\t" << formatRef(*input);
		break;
	}

	case LOAD: {
		auto* input = reinterpret_cast<MemoryInput*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << "\t" << formatRef(input->ptr);
		break;
	}

	case STORE: {
		auto* input = reinterpret_cast<MemoryInput*>(inputPtr);
		ss << "\t" << formatRef(input->ptr) << "\t" << formatRef(input->value);
		break;
	}

	case CALL: {
		auto* input = reinterpret_cast<CallInput*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << "\tfunc_" << input->funcPtr << "(";
		auto* args = reinterpret_cast<TypedValueRef*>(inputPtr + sizeof(CallInput));
		for (uint8_t i = 0; i < input->argCount; i++) {
			if (i > 0)
				ss << ", ";
			ss << formatRef(args[i]);
		}
		ss << ")";
		break;
	}

	case FREE: {
		auto* input = reinterpret_cast<TypedValueRef*>(inputPtr);
		ss << "\t" << formatRef(*input);
		break;
	}

	case SELECT: {
		auto* input = reinterpret_cast<SelectInput*>(inputPtr);
		ss << "\t" << formatRef(header->resultRef) << " " << formatRef(input->condition) << " ? "
		   << formatRef(input->trueValue) << " : " << formatRef(input->falseValue);
		break;
	}
	}

	return ss.str();
}

std::string toString(Trace& trace) {
	std::ostringstream ss;
	auto blocks = trace.getBlocks();

	for (auto& block : blocks) {
		ss << "B" << block.view->blockId << "()\n";
		auto operations = block.getOperations();
		for (auto& op : operations) {
			ss << formatOperation(op) << "\n";
		}
	}

	return ss.str();
}

std::pair<TypedValueRef, Trace::TracerOperationIdentifier>
Trace::addOperationWithResult(Snapshot snapshot, Type resultType, Op op, std::span<const TypedValueRef> operands) {
	if (isBinaryOp(op)) {
		assert(operands.size() == 2 && "Binary operations require exactly 2 operands");
		return appendOperation(snapshot, resultType, op, BinaryInput {operands[0], operands[1]});
	}
	if (isUnaryOp(op)) {
		assert(operands.size() == 1 && "Unary operations require exactly 1 operand");
		return appendOperation(snapshot, resultType, op, UnaryInput {operands[0]});
	}
	if (op == LOAD) {
		assert(operands.size() == 1 && "LOAD requires exactly 1 operand (pointer)");
		return appendOperation(snapshot, resultType, op, MemoryInput {operands[0], {}});
	}
	if (op == STORE) {
		assert(operands.size() == 2 && "STORE requires exactly 2 operands (pointer, value)");
		return appendOperation(snapshot, resultType, op, MemoryInput {operands[0], operands[1]});
	}
	if (op == SELECT) {
		assert(operands.size() == 3 && "SELECT requires exactly 3 operands (condition, trueValue, falseValue)");
		return appendOperation(snapshot, resultType, op, SelectInput {operands[0], operands[1], operands[2]});
	}
	assert(false && "Unsupported operation type for addOperationWithResult");
	__builtin_unreachable();
}

Trace::Trace() {
	auto* memory = mmap(nullptr, 1024 * 1024 * 1024, PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	view = new (memory) TraceHeader();
	capacity = 1024 * 1024 * 1024;
}
Trace::~Trace() {
	[[maybe_unused]] auto result = munmap(view, capacity);
	assert(result == 0 && "Failed to unmap memory");
}
size_t getOperationSize(Op op, uint8_t argCount) {
	size_t base = sizeof(Trace::OperationHeader);
	switch (op) {
	// Binary operations
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
	case BAND:
		return base + sizeof(BinaryInput);

	// Unary operations
	case NOT:
	case NEGATE:
	case CAST:
		return base + sizeof(UnaryInput);

	case CMP:
		return base + sizeof(CmpInput);

	case JMP:
		return base + sizeof(JmpInput);

	case CONST:
		return base + sizeof(ConstantLiteral);

	case LOAD:
	case STORE:
		return base + sizeof(MemoryInput);

	case CALL:
		return base + sizeof(CallInput) + argCount * sizeof(TypedValueRef);

	case RETURN:
	case ASSIGN:
		return base + sizeof(TypedValueRef);

	case FREE:
		return base + sizeof(TypedValueRef);

	case SELECT:
		return base + sizeof(SelectInput);
	}
	throw std::runtime_error("Unknown operation type");
}

bool isBinaryOp(Op op) {
	switch (op) {
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
	case BAND:
		return true;
	default:
		return false;
	}
}

bool isUnaryOp(Op op) {
	switch (op) {
	case NOT:
	case NEGATE:
	case CAST:
		return true;
	default:
		return false;
	}
}

size_t Trace::Operation::size() {
	if (view->op == CALL) {
		return getOperationSize(view->op, arguments<CallInput>().argCount);
	}
	return getOperationSize(view->op);
}

Trace::TraceToOperationOffset operator+(const Trace::TraceToBlockOffset& traceToBlock,
                                        const Trace::BlockToOperationOffset& blockToOperation) {
	return Trace::TraceToOperationOffset(traceToBlock.offset + blockToOperation.offset);
}
Trace::TraceToBlockOffset operator+(const Trace::TraceToBlockOffset& traceToBlock,
                                    const Trace::BlockToBlockOffset& blockToBlockOffset) {
	return Trace::TraceToBlockOffset(traceToBlock.offset + blockToBlockOffset.offset);
}

} // namespace nautilus::tracing
