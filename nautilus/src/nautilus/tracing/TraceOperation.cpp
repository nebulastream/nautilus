#include "nautilus/tracing/TraceOperation.hpp"
#include <spdlog/spdlog.h>

namespace nautilus::tracing {

std::ostream& operator<<(std::ostream& os, const FunctionCall& call) {
	os << call.functionName << "(";
	for (size_t i = 0; i < call.arguments.size(); i++) {
		if (i != 0) {
			os << ",";
		}
		os << "$" << call.arguments[i].ref;
	}
	os << ")";
	return os;
}

BlockRef::BlockRef(uint16_t block) : block(block) {
}

std::ostream& operator<<(std::ostream& os, const BlockRef& ref) {
	os << "B" << ref.block << "(";
	for (size_t i = 0; i < ref.arguments.size(); i++) {
		if (i != 0) {
			os << ",";
		}
		os << ref.arguments[i].toString();
	}
	os << ")";
	return os;
}

TraceOperation::TraceOperation(Snapshot& tag, Op op, Type resultType, value_ref ref, std::vector<InputVariant>&& input) : tag(tag), op(op), resultType(resultType), resultRef(ref), input(input) {
}

TraceOperation::TraceOperation(Op op, std::vector<InputVariant>&& input) : tag(), op(op), resultType(Type::v), resultRef(), input(input) {
}

std::ostream& operator<<(std::ostream& os, const TraceOperation& operation) {
	if (operation.op == FREE) {
		return os;
	}
	os << fmt::format("{}\t", toString(operation.op));
	os << operation.resultRef.toString() << "\t";
	for (const InputVariant& opInput : operation.input) {
		if (auto inputRef = std::get_if<value_ref>(&opInput)) {
			os << fmt::format("${}\t", inputRef->ref);
		} else if (auto blockRef = std::get_if<BlockRef>(&opInput)) {
			os << *blockRef << "\t";
		} else if (auto fCall = std::get_if<FunctionCall>(&opInput)) {
			os << *fCall << "\t";
		} else if (auto constant = std::get_if<ConstantLiteral>(&opInput)) {
			std::visit(
			    [&](auto&& value) -> void {
				    using T = std::decay_t<decltype(value)>;
				    if constexpr (!std::is_pointer_v<T>) {
					    os << value << "\t";
				    } else {
					    os << "*\t";
				    }
			    },
			    *constant);
		}
	}
	os << fmt::format(":{}\t", toString(operation.resultType));
	return os;
}
} // namespace nautilus::tracing
