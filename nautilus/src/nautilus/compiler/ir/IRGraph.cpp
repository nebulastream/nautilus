
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
#include "nautilus/debug/DwarfVariableResolver.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <fmt/format.h>
#include <utility>

namespace nautilus::compiler::ir {

namespace {

/// Thread-local pointer consulted by the per-Operation fmt formatter while
/// `IRGraph::toString(options)` is running.  Using a scoped TLS keeps the
/// extension hidden from every other caller of `fmt::formatter<Operation>`
/// so the default `toString()` path produces byte-identical output to what
/// it did before source-location support existed.
thread_local const IRPrintOptions* currentPrintOptions = nullptr;

struct PrintOptionsScope {
	explicit PrintOptionsScope(const IRPrintOptions* opts) : previous(currentPrintOptions) {
		currentPrintOptions = opts;
	}
	~PrintOptionsScope() {
		currentPrintOptions = previous;
	}
	PrintOptionsScope(const PrintOptionsScope&) = delete;
	PrintOptionsScope& operator=(const PrintOptionsScope&) = delete;

	const IRPrintOptions* previous;
};

} // namespace

IRGraph::IRGraph(const compiler::CompilationUnitID& id) : arena_(common::ArenaPool::makeStandalone()), id(id) {
}

IRGraph::IRGraph(common::ArenaPool::Handle arena, const compiler::CompilationUnitID& id)
    : arena_(std::move(arena)), id(id) {
}

FunctionOperation* IRGraph::addFunctionOperation(FunctionOperation* functionOperation) {
	functionOperations.emplace_back(functionOperation);
	functionOperationsByName.emplace(std::string_view {functionOperation->getName()}, functionOperation);
	return functionOperation;
}

const std::vector<FunctionOperation*>& IRGraph::getFunctionOperations() const {
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

std::string nautilus::compiler::ir::IRGraph::toString(const nautilus::compiler::ir::IRPrintOptions& options) const {
	PrintOptionsScope scope(&options);
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
	static auto format(nautilus::compiler::ir::BlockIdentifier id, format_context& ctx) -> format_context::iterator {
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
		const auto args = op.getArguments();
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) {
				fmt::format_to(out, ", ");
			}
			fmt::format_to(out, "{}", args[i]->getIdentifier());
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
		const auto args = op.getInputArguments();
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", args[i]->getIdentifier());
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

	// Opt-in source-location trailer.  The TLS pointer is only non-null
	// inside the scope of `IRGraph::toString(options)`.
	//
	// LIFETIME WARNING: Operation::sourceTag points into the trie owned
	// by TagRecorder, which is currently stack-allocated inside
	// {Lazy,ExceptionBased}TraceContext::startTrace and destroyed when
	// tracing ends. Calling toString() with showSourceLocations=true
	// after the enclosing trace has finished therefore dereferences
	// dangling pointers. The compiler's own dump path runs while
	// tracing is still in scope only for the engine's after_ir_creation
	// dump; out-of-band callers (tests, post-trace inspection) need the
	// trie to be relocated into longer-lived storage first. Tracked as
	// a follow-up to PR #271.
	if (const auto* opts = nautilus::compiler::ir::currentPrintOptions;
	    opts != nullptr && opts->showSourceLocations && opts->resolver != nullptr) {
		if (const auto* tag = op.getSourceTag()) {
			const auto frames = opts->resolver->resolveStack(tag);
			if (!frames.empty()) {
				// Innermost frame on the same line; outer frames continue on
				// their own lines, outermost last.  Two tabs lines them up
				// under the op text the block formatter indents with one tab.
				const auto& innermost = frames.back();
				fmt::format_to(out, "  ; at {}:{} ({})", innermost.file, innermost.line, innermost.function);

				// Optional DWARF variable-name annotation. We query the
				// host binary's DWARF for a DW_TAG_variable declared at
				// the innermost frame's coordinates and append the
				// recovered name (e.g. `sum`, `factor`) when one exists.
				// Independent of the inlined-frames loop below — the
				// variable lives at the innermost user-visible site.
				if (opts->showVariableNames && opts->variableResolver != nullptr) {
					if (auto name = opts->variableResolver->resolveVariableName(innermost)) {
						fmt::format_to(out, " [var={}]", *name);
					}
				}

				for (auto it = frames.rbegin() + 1; it != frames.rend(); ++it) {
					fmt::format_to(out, "\n\t\t; inlined from {}:{} ({})", it->file, it->line, it->function);
				}
			}
		}
	}
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
		for (auto* operation : block.getOperations()) {
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
		// The trace-to-IR conversion leaves `inputArgs`/`inputArgNames` empty;
		// the parameters live on the entry block. Fall back to those.
		const auto& argTypes = func.getInputArgs();
		const auto& argNames = func.getInputArgNames();
		const auto* entry = func.getEntryBlock();
		if (entry != nullptr && argTypes.empty() && argNames.empty()) {
			const auto& blockArgs = entry->getArguments();
			for (size_t i = 0; i < blockArgs.size(); ++i) {
				if (i > 0) {
					fmt::format_to(out, ", ");
				}
				fmt::format_to(out, "{}:{}", blockArgs[i]->getIdentifier(), toString(blockArgs[i]->getStamp()));
			}
		} else {
			for (size_t i = 0; i < argTypes.size(); ++i) {
				if (i > 0) {
					fmt::format_to(out, ", ");
				}
				if (i < argNames.size()) {
					fmt::format_to(out, "{}:{}", argNames[i], toString(argTypes[i]));
				} else {
					fmt::format_to(out, "{}", toString(argTypes[i]));
				}
			}
		}
		fmt::format_to(out, ") :{} {{", toString(func.getOutputArg()));
		for (const auto* block : func.getBasicBlocks()) {
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
	fmt::format_to(out, "nautilus {{\n");

	// Print all function operations
	for (const auto* func : graph.getFunctionOperations()) {
		fmt::format_to(out, "{}", *func);
	}

	fmt::format_to(out, "}} //nautilus\n");
	return out;
}
