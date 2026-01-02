
#include "nautilus/tracing/ExecutionTrace.hpp"
#include <algorithm>
#include <fmt/format.h>
#include <nautilus/config.hpp>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/logging.hpp>

namespace nautilus::tracing {

uint16_t ExecutionTrace::createBlock() {
	auto blockIndex = blocks.size();
	blocks.emplace_back(blockIndex);
	return blockIndex;
}

Block& ExecutionTrace::getBlock(uint16_t blockIndex) {
	if (blockIndex >= blocks.size()) {
		throw RuntimeException("Block index out of bounds: " + std::to_string(blockIndex));
	}
	return blocks[blockIndex];
}

const Block& ExecutionTrace::getBlock(uint16_t blockIndex) const {
	return const_cast<ExecutionTrace*>(this)->getBlock(blockIndex);
}

std::vector<Block>& ExecutionTrace::getBlocks() {
	return blocks;
}

const std::vector<Block>& ExecutionTrace::getBlocks() const {
	return blocks;
}

TypedValueRef& ExecutionTrace::setArgument(Type type, size_t index) {
	ValueRef argRef = index + 1;
	auto& arguments = blocks[0].arguments;
	if (arguments.size() < argRef) {
		arguments.resize(argRef);
	}
	// arguments[index] = {argRef, type};
	arguments[index] = TypedValueRef(argRef, type);
	return arguments[index];
}

std::vector<operation_identifier>& ExecutionTrace::getReturns() {
	return returnRefs;
}

const std::vector<operation_identifier>& ExecutionTrace::getReturns() const {
	return returnRefs;
}

const std::vector<TypedValueRef>& ExecutionTrace::getArguments() {
	return blocks[0].arguments;
}

} // namespace nautilus::tracing

std::string nautilus::tracing::ExecutionTrace::toString() const {
	return fmt::to_string(*this);
}

namespace fmt {
template <>
struct formatter<nautilus::tracing::ExecutionTrace> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::ExecutionTrace& trace, format_context& ctx) -> format_context::iterator;
};

template <>
struct formatter<nautilus::tracing::Block> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::Block& trace, format_context& ctx) -> format_context::iterator;
};

template <>
struct formatter<nautilus::tracing::TraceOperation> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::TraceOperation& trace, format_context& ctx) -> format_context::iterator;
};

auto formatter<nautilus::tracing::ExecutionTrace>::format(const nautilus::tracing::ExecutionTrace& trace,
                                                          fmt::format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	for (size_t i = 0; i < trace.getBlocks().size(); i++) {
		fmt::format_to(out, "B{}{}", trace.getBlock(i).blockId, trace.getBlock(i));
	}
	return out;
}

auto formatter<nautilus::tracing::Block>::format(const nautilus::tracing::Block& block, format_context& ctx)
    -> format_context::iterator {
	auto out = ctx.out();
	fmt::format_to(out, "(");
	for (size_t i = 0; i < block.arguments.size(); i++) {
		if (i != 0) {
			fmt::format_to(out, ",");
		}
		fmt::format_to(out, "${}:{}", block.arguments[i].ref, toString(block.arguments[i].type));
	}
	fmt::format_to(out, ")");
	if (block.type == nautilus::tracing::Block::Type::ControlFlowMerge) {
		fmt::format_to(out, " ControlFlowMerge");
	}
	fmt::format_to(out, "\n");
	for (const auto& operation : block.operations) {
		fmt::format_to(out, "{}\n", operation);
	}
	return out;
}

template <>
struct formatter<nautilus::tracing::TypedValueRef> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::TypedValueRef& typeValRef, format_context& ctx)
	    -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "${}", typeValRef.ref);
		return out;
	}
};

template <>
struct formatter<nautilus::tracing::BlockRef> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::BlockRef& ref, format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "B{}(", ref.block);
		for (size_t i = 0; i < ref.arguments.size(); i++) {
			if (i != 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", ref.arguments[i]);
		}
		fmt::format_to(out, ")");
		return out;
	}
};

template <>
struct formatter<nautilus::tracing::FunctionCall> : formatter<std::string_view> {
	static auto format(const nautilus::tracing::FunctionCall& call, format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		if (nautilus::log::options::getLogAddresses()) {
			fmt::format_to(out, "{}(", call.functionName);
		} else {
			fmt::format_to(out, "func_*(");
		}

		for (size_t i = 0; i < call.arguments.size(); i++) {
			if (i != 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", call.arguments[i]);
		}
		fmt::format_to(out, ")");
		return out;
	}
};

template <>
struct formatter<nautilus::ConstantLiteral> : formatter<std::string_view> {
	auto format(nautilus::ConstantLiteral c, format_context& ctx) const -> format_context::iterator;
};

auto formatter<nautilus::tracing::TraceOperation>::format(const nautilus::tracing::TraceOperation& operation,
                                                          format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	fmt::format_to(out, "\t{}\t", toString(operation.op));
	fmt::format_to(out, "{}\t", operation.resultRef);
	for (const auto& opInput : operation.input) {
		if (auto inputRef = std::get_if<nautilus::tracing::TypedValueRef>(&opInput)) {
			fmt::format_to(out, "{}\t", *inputRef);
		} else if (auto blockRef = std::get_if<nautilus::tracing::BlockRef>(&opInput)) {
			fmt::format_to(out, "{}\t", *blockRef);
		} else if (auto fCall = std::get_if<nautilus::tracing::FunctionCall>(&opInput)) {
			fmt::format_to(out, "{}\t", *fCall);
		} else if (auto constant = std::get_if<nautilus::ConstantLiteral>(&opInput)) {
			fmt::format_to(out, "{}", *constant);
		}
	}
	fmt::format_to(out, ":{}", toString(operation.resultType));
	return out;
}

} // namespace fmt
