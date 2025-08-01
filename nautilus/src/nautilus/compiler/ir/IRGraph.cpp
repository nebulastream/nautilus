
#include "nautilus/compiler/ir/IRGraph.hpp"
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

std::unique_ptr<FunctionOperation>& IRGraph::addRootOperation(std::unique_ptr<FunctionOperation> root) {
	this->rootOperation = std::move(root);
	return this->rootOperation;
}

const CompilationUnitID& IRGraph::getId() const {
	return id;
}

const FunctionOperation& IRGraph::getRootOperation() const {
	return *rootOperation;
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
	auto out = ctx.out();
	if (auto shiftOp = op.dynCast<ShiftOperation>()) {
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), shiftOp->getLeftInput()->getIdentifier(),
		               shiftOpToString(shiftOp->getType()), shiftOp->getRightInput()->getIdentifier());
	} else if (auto compOp = op.dynCast<CompareOperation>()) {
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), compOp->getLeftInput()->getIdentifier(),
		               compOpToString(compOp->getComparator()), compOp->getRightInput()->getIdentifier());
	} else if (auto bcompOp = op.dynCast<BinaryCompOperation>()) {
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), bcompOp->getLeftInput()->getIdentifier(),
		               shiftOpToString(bcompOp->getType()), bcompOp->getRightInput()->getIdentifier());
	} else if (auto res = op.dynCast<BinaryOperation>()) {
		fmt::format_to(out, "{} = {} {} {}", op.getIdentifier(), res->getLeftInput()->getIdentifier(),
		               binaryOpToString(op.getOperationType()), res->getRightInput()->getIdentifier());
	} else if (auto ifOp = op.dynCast<IfOperation>()) {
		fmt::format_to(out, "{}", *ifOp);
	} else if (auto brOp = op.dynCast<BranchOperation>()) {
		fmt::format_to(out, "br {}", brOp->getNextBlockInvocation());
	} else if (auto constBool = op.dynCast<ConstBooleanOperation>()) {
		fmt::format_to(out, "{} = {}", op.getIdentifier(), constBool->getValue());
	} else if (auto constInt = op.dynCast<ConstIntOperation>()) {
		fmt::format_to(out, "{} = {}", op.getIdentifier(), constInt->getValue());
	} else if (auto constFloat = op.dynCast<ConstFloatOperation>()) {
		fmt::format_to(out, "{} = {}", op.getIdentifier(), constFloat->getValue());
	} else if (auto returnOp = op.dynCast<ReturnOperation>()) {
		fmt::format_to(out, "return");
		if (returnOp->hasReturnValue()) {
			fmt::format_to(out, " ({})", returnOp->getReturnValue()->getIdentifier());
		}
	} else if (auto constPtr = op.dynCast<ConstPtrOperation>()) {
		fmt::format_to(out, "{} = *", constPtr->getIdentifier());
	} else if (auto callOp = op.dynCast<ProxyCallOperation>()) {
		fmt::format_to(out, "{}", *callOp);
	} else if (auto castOp = op.dynCast<CastOperation>()) {
		fmt::format_to(out, "{} = {} cast_to {}", castOp->getIdentifier(), castOp->getInput()->getIdentifier(),
		               toString(castOp->getStamp()));
	} else if (auto loadOp = op.dynCast<LoadOperation>()) {
		fmt::format_to(out, "{} = load({})", loadOp->getIdentifier(), loadOp->getAddress()->getIdentifier());
	} else if (auto storeOp = op.dynCast<StoreOperation>()) {
		fmt::format_to(out, "store({}, {})", storeOp->getValue()->getIdentifier(),
		               storeOp->getAddress()->getIdentifier());
	} else if (auto notOp = op.dynCast<NotOperation>()) {
		fmt::format_to(out, "{} = !{}", notOp->getIdentifier(), notOp->getInput()->getIdentifier());
	} else if (auto negateOp = op.dynCast<NegateOperation>()) {
		fmt::format_to(out, "{} = ~{}", negateOp->getIdentifier(), negateOp->getInput()->getIdentifier());
	} else {
		fmt::format_to(out, "{}", op.getIdentifier().toString());
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
	auto& rootOp = graph.getRootOperation();
	fmt::format_to(out, "{}", rootOp);
	fmt::format_to(out, "}} //NESIR\n");
	return out;
}
