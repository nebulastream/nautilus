
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/logging.hpp"
#include <fmt/format.h>
#include <utility>

namespace nautilus::compiler::ir {

IRGraph::IRGraph(const compiler::CompilationUnitID& id) : id(id) {
}

std::unique_ptr<FunctionOperation>&
IRGraph::addFunctionOperation(std::unique_ptr<FunctionOperation> functionOperation) {
	auto& slot = functionOperations.emplace_back(std::move(functionOperation));
	functionOperationsByName.emplace(std::string_view {slot->getName()}, slot.get());
	return slot;
}

const std::vector<std::unique_ptr<FunctionOperation>>& IRGraph::getFunctionOperations() const {
	return functionOperations;
}

const FunctionOperation* IRGraph::getFunctionOperation(const std::string& name) const {
	auto it = functionOperationsByName.find(std::string_view {name});
	return it != functionOperationsByName.end() ? it->second : nullptr;
}

const CompilationUnitID& IRGraph::getId() const {
	return id;
}

constexpr const char* binaryOpToString(Operation::OperationType type) {
	switch (type) {
	case Operation::OperationType::AddOp:
		return "+";
	case Operation::OperationType::AndOp:
		return "and";
	case Operation::OperationType::NotOp:
		return "!";
	case Operation::OperationType::DivOp:
		return "/";
	case Operation::OperationType::ModOp:
		return "%";
	case Operation::OperationType::MulOp:
		return "*";
	case Operation::OperationType::NegateOp:
		return "neg";
	case Operation::OperationType::OrOp:
		return "or";
	case Operation::OperationType::SubOp:
		return "-";
	default:
		return "";
	}
}

constexpr const char* shiftOpToString(ShiftOperation::ShiftType type) {
	switch (type) {
	case ShiftOperation::ShiftType::LS:
		return "<<";
	case ShiftOperation::ShiftType::RS:
		return ">>";
	default:
		return "";
	}
}

constexpr const char* shiftOpToString(BinaryCompOperation::Type type) {
	switch (type) {
	case BinaryCompOperation::BOR:
		return "|";
	case BinaryCompOperation::BAND:
		return "&";
	case BinaryCompOperation::XOR:
		return "^";
	default:
		return "";
	}
}

constexpr const char* compOpToString(CompareOperation::Comparator type) {
	switch (type) {
	case CompareOperation::Comparator::EQ:
		return "==";
	case CompareOperation::Comparator::LE:
		return "<=";
	case CompareOperation::Comparator::LT:
		return "<";
	case CompareOperation::Comparator::GE:
		return ">=";
	case CompareOperation::Comparator::GT:
		return ">";
	default:
		return "";
	}
}

} // namespace nautilus::compiler::ir

std::string nautilus::compiler::ir::IRGraph::toString() const {
	return fmt::to_string(*this);
}

namespace fmt {
using namespace nautilus::compiler::ir;
template <>
struct formatter<nautilus::compiler::ir::IRGraph> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::IRGraph& c, format_context& ctx) -> format_context::iterator;
};

template <>
struct formatter<nautilus::compiler::ir::Operation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::Operation& c, format_context& ctx) -> format_context::iterator;
};

template <>
struct formatter<nautilus::compiler::ir::OperationIdentifier> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::OperationIdentifier& op,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "${}", op.getId());
		return out;
	}
};

template <>
struct formatter<nautilus::compiler::ir::BlockIdentifier> : formatter<std::string_view> {
	static auto format(nautilus::compiler::ir::BlockIdentifier id,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "{}", id.getId());
		return out;
	}
};

template <>
struct formatter<nautilus::compiler::ir::BasicBlockInvocation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::BasicBlockInvocation& op,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "Block_{}(", op.getBlock()->getIdentifier());
		const auto& args = op.getArguments();
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) {
				fmt::format_to(out, ", ");
			}
			fmt::format_to(out, "{}", args.at(i)->getIdentifier());
		}
		fmt::format_to(out, ")");
		return out;
	}
};

template <>
struct formatter<nautilus::compiler::ir::IfOperation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::IfOperation& op, format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "if {} ? {} : {}", op.getValue()->getIdentifier(), op.getTrueBlockInvocation(),
		               op.getFalseBlockInvocation());
		return out;
	}
};

template <>
struct formatter<nautilus::compiler::ir::ProxyCallOperation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::ProxyCallOperation& op,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();

		if (op.getStamp() != nautilus::Type::v) {
			fmt::format_to(out, "${} = ", op.getIdentifier().getId());
		}
		if (nautilus::log::options::getLogAddresses()) {
			fmt::format_to(out, "{}(", op.getFunctionName());
		} else {
			fmt::format_to(out, "func_*(");
		}
		const auto& args = op.getInputArguments();
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", args.at(i)->getIdentifier());
		}
		fmt::format_to(out, ")");
		return out;
	}
};

auto fmt::formatter<nautilus::compiler::ir::Operation>::format(const nautilus::compiler::ir::Operation& op,
                                                               format_context& ctx) -> format_context::iterator {
	using OpType = nautilus::compiler::ir::Operation::OperationType;
	auto out = ctx.out();
	switch (op.getOperationType()) {
	case OpType::ShiftOp: {
		const auto& shiftOp = *nautilus::compiler::ir::cast<ShiftOperation>(&op);
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), shiftOp.getLeftInput()->getIdentifier(),
		               shiftOpToString(shiftOp.getType()), shiftOp.getRightInput()->getIdentifier());
		break;
	}
	case OpType::CompareOp: {
		const auto& compOp = *nautilus::compiler::ir::cast<CompareOperation>(&op);
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), compOp.getLeftInput()->getIdentifier(),
		               compOpToString(compOp.getComparator()), compOp.getRightInput()->getIdentifier());
		break;
	}
	case OpType::BinaryComp: {
		const auto& bcompOp = *nautilus::compiler::ir::cast<BinaryCompOperation>(&op);
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), bcompOp.getLeftInput()->getIdentifier(),
		               shiftOpToString(bcompOp.getType()), bcompOp.getRightInput()->getIdentifier());
		break;
	}
	case OpType::AddOp:
	case OpType::SubOp:
	case OpType::MulOp:
	case OpType::DivOp:
	case OpType::ModOp:
	case OpType::AndOp:
	case OpType::OrOp: {
		const auto& bin = *nautilus::compiler::ir::cast<BinaryOperation>(&op);
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), bin.getLeftInput()->getIdentifier(),
		               binaryOpToString(op.getOperationType()), bin.getRightInput()->getIdentifier());
		break;
	}
	case OpType::IfOp:
		fmt::format_to(out, "{}", *nautilus::compiler::ir::cast<IfOperation>(&op));
		break;
	case OpType::BranchOp:
		fmt::format_to(out, "br {}", nautilus::compiler::ir::cast<BranchOperation>(&op)->getNextBlockInvocation());
		break;
	case OpType::ConstBooleanOp:
		fmt::format_to(out, "{} = {}", op.getIdentifier(),
		               nautilus::compiler::ir::cast<ConstBooleanOperation>(&op)->getValue());
		break;
	case OpType::ConstIntOp:
		fmt::format_to(out, "{} = {}", op.getIdentifier(),
		               nautilus::compiler::ir::cast<ConstIntOperation>(&op)->getValue());
		break;
	case OpType::ConstFloatOp:
		fmt::format_to(out, "{} = {}", op.getIdentifier(),
		               nautilus::compiler::ir::cast<ConstFloatOperation>(&op)->getValue());
		break;
	case OpType::ReturnOp: {
		const auto* returnOp = nautilus::compiler::ir::cast<ReturnOperation>(&op);
		fmt::format_to(out, "return");
		if (returnOp->hasReturnValue()) {
			fmt::format_to(out, " ({})", returnOp->getReturnValue()->getIdentifier());
		}
		break;
	}
	case OpType::ConstPtrOp:
		fmt::format_to(out, "{} = *", op.getIdentifier());
		break;
	case OpType::ProxyCallOp:
		fmt::format_to(out, "{}", *nautilus::compiler::ir::cast<ProxyCallOperation>(&op));
		break;
	case OpType::CastOp: {
		const auto* castOp = nautilus::compiler::ir::cast<CastOperation>(&op);
		fmt::format_to(out, "{} = {} cast_to {}", op.getIdentifier(), castOp->getInput()->getIdentifier(),
		               toString(castOp->getStamp()));
		break;
	}
	case OpType::LoadOp: {
		const auto* loadOp = nautilus::compiler::ir::cast<LoadOperation>(&op);
		fmt::format_to(out, "{} = load({})", op.getIdentifier(), loadOp->getAddress()->getIdentifier());
		break;
	}
	case OpType::StoreOp: {
		const auto* storeOp = nautilus::compiler::ir::cast<StoreOperation>(&op);
		fmt::format_to(out, "store({}, {})", storeOp->getValue()->getIdentifier(),
		               storeOp->getAddress()->getIdentifier());
		break;
	}
	case OpType::NotOp: {
		const auto* notOp = nautilus::compiler::ir::cast<NotOperation>(&op);
		fmt::format_to(out, "{} = !{}", op.getIdentifier(), notOp->getInput()->getIdentifier());
		break;
	}
	case OpType::NegateOp: {
		const auto* negateOp = nautilus::compiler::ir::cast<NegateOperation>(&op);
		fmt::format_to(out, "{} = ~{}", op.getIdentifier(), negateOp->getInput()->getIdentifier());
		break;
	}
	case OpType::AllocaOp: {
		const auto* alloca = nautilus::compiler::ir::cast<AllocaOperation>(&op);
		fmt::format_to(out, "{} = alloca {}b", op.getIdentifier(), alloca->getSize());
		break;
	}
	default:
		fmt::format_to(out, "{}", op.getIdentifier().toString());
		break;
	}
	fmt::format_to(out, " :{}", toString(op.getStamp()));
	return out;
}

template <>
struct formatter<nautilus::compiler::ir::BasicBlock> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::BasicBlock& block,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "\nBlock_{}(", block.getIdentifier());
		const auto& args = block.getArguments();
		if (!args.empty()) {
			fmt::format_to(out, "{}:{}", args.at(0)->getIdentifier().toString(), toString(args.at(0)->getStamp()));
			for (size_t i = 1; i < args.size(); ++i) {
				fmt::format_to(out, ", {}:{}", args.at(i)->getIdentifier().toString(),
				               toString(args.at(i)->getStamp()));
			}
		}
		fmt::format_to(out, "):\n");
		for (auto& operation : block.getOperations()) {
			fmt::format_to(out, "\t{}\n", *operation);
		}
		return out;
	}
};

template <>
struct formatter<nautilus::compiler::ir::FunctionOperation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::FunctionOperation& func,
	                   format_context& ctx) -> format_context::iterator {
		auto out = ctx.out();
		fmt::format_to(out, "{}(", func.getName());
		for (const auto& arg : func.getInputArgNames()) {
			fmt::format_to(out, "{} ", arg);
		}
		fmt::format_to(out, ") {{");
		for (const auto& block : func.getBasicBlocks()) {
			fmt::format_to(out, "{}", *block);
		}
		fmt::format_to(out, "}}\n");
		return out;
	}
};
} // namespace fmt

auto fmt::formatter<nautilus::compiler::ir::IRGraph>::format(const nautilus::compiler::ir::IRGraph& graph,
                                                             format_context& ctx) -> format_context::iterator {
	auto out = ctx.out();
	fmt::format_to(out, "NautilusIr {{\n");

	// Print all function operations
	for (const auto& func : graph.getFunctionOperations()) {
		fmt::format_to(out, "{}", *func);
	}

	fmt::format_to(out, "}} //NESIR\n");
	return out;
}
