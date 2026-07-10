#include "nautilus/compiler/ir/util/IRSerializationUtil.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
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
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LoadOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/NotOperation.hpp"
#include "nautilus/compiler/ir/operations/ProxyCallOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/SelectOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <fmt/format.h>
#include <fmt/ranges.h>
#include <map>
#include <unordered_map>

namespace nautilus::compiler::ir {

namespace {

constexpr const char* binaryOpSymbol(Operation::OperationType type) {
	switch (type) {
	case Operation::OperationType::AddOp:
		return "+";
	case Operation::OperationType::SubOp:
		return "-";
	case Operation::OperationType::MulOp:
		return "*";
	case Operation::OperationType::DivOp:
		return "/";
	case Operation::OperationType::ModOp:
		return "%";
	case Operation::OperationType::AndOp:
		return "and";
	case Operation::OperationType::OrOp:
		return "or";
	default:
		return "";
	}
}

constexpr const char* comparatorSymbol(CompareOperation::Comparator comparator) {
	switch (comparator) {
	case CompareOperation::Comparator::EQ:
		return "==";
	case CompareOperation::Comparator::NE:
		return "!=";
	case CompareOperation::Comparator::LT:
		return "<";
	case CompareOperation::Comparator::LE:
		return "<=";
	case CompareOperation::Comparator::GT:
		return ">";
	case CompareOperation::Comparator::GE:
		return ">=";
	}
	return "";
}

/// Quotes @p text so the parser can read it back verbatim: backslashes and
/// double quotes are escaped. Mangled symbols and demangled function names
/// may contain almost arbitrary characters, so quoting is unconditional.
std::string quoted(const std::string& text) {
	std::string out;
	out.reserve(text.size() + 2);
	out.push_back('"');
	for (char c : text) {
		if (c == '"' || c == '\\') {
			out.push_back('\\');
		}
		out.push_back(c);
	}
	out.push_back('"');
	return out;
}

/// How every printed `$<id>` is chosen.
///
/// The in-memory IR distinguishes values by pointer identity and reuses the
/// same numeric identifier for different values in different blocks (the
/// tracer numbers per value slot, and passes such as block-argument pruning
/// create cross-block references between them). The portable rendering
/// therefore renumbers every value-producing operation and block argument
/// with an id that is unique within its function, so every reference in the
/// text is unambiguous. The display rendering keeps the stored identifiers
/// so dumps line up with in-memory state (verifier messages, backend
/// register frames, MLIR debug info).
class IdMap {
public:
	/// Display: stored identifiers.
	IdMap() = default;

	/// Portable: function-wide unique renumbering.
	explicit IdMap(const FunctionOperation& function) : renumbered(true) {
		uint32_t next = 1;
		for (const auto* block : function.getBasicBlocks()) {
			for (const auto* argument : block->getArguments()) {
				ids.emplace(argument, next++);
			}
			for (const auto* operation : block->getOperations()) {
				if (operation->getStamp() != Type::v) {
					ids.emplace(operation, next++);
				}
			}
		}
	}

	uint32_t ref(const Operation* operation) const {
		if (!renumbered) {
			return operation->getIdentifier().getId();
		}
		auto it = ids.find(operation);
		if (it == ids.end()) {
			throw RuntimeException(fmt::format(
			    "Cannot serialize IR: operation ${} is referenced but not defined in any block of its function",
			    operation->getIdentifier().getId()));
		}
		return it->second;
	}

private:
	bool renumbered = false;
	std::unordered_map<const Operation*, uint32_t> ids;
};

/// Rendering configuration shared by the whole writer. `portable` selects
/// the strict serialization form (full external symbols, errors on
/// process-specific state); otherwise the display form is produced
/// (placeholders, never throws, optional source locations).
struct WriterConfig {
	bool portable = false;
	const IRPrintOptions* displayOptions = nullptr;

	/// Whether external function symbols/names may be printed. The display
	/// form hides them behind `func_*` by default because symbols recorded
	/// for non-exported functions are raw addresses, which would make dumps
	/// nondeterministic.
	bool showExternalSymbols() const {
		return portable || log::options::getLogAddresses();
	}
};

void writeFunctionAttributes(fmt::memory_buffer& out, const FunctionAttributes& attrs) {
	std::vector<std::string> parts;
	if (attrs.modRefInfo != ModRefInfo::ModRef) {
		parts.push_back(fmt::format("modref={}", static_cast<uint8_t>(attrs.modRefInfo)));
	}
	if (attrs.willReturn) {
		parts.emplace_back("willreturn");
	}
	if (attrs.noUnwind) {
		parts.emplace_back("nounwind");
	}
	if (!parts.empty()) {
		fmt::format_to(std::back_inserter(out), " attrs[{}]", fmt::join(parts, ","));
	}
}

void writeArgumentList(fmt::memory_buffer& out, std::span<Operation* const> args, const IdMap& values) {
	out.push_back('(');
	for (size_t i = 0; i < args.size(); ++i) {
		if (i > 0) {
			fmt::format_to(std::back_inserter(out), ", ");
		}
		fmt::format_to(std::back_inserter(out), "${}", values.ref(args[i]));
	}
	out.push_back(')');
}

void writeInvocation(fmt::memory_buffer& out, const BasicBlockInvocation& invocation, const IdMap& values) {
	fmt::format_to(std::back_inserter(out), "Block_{}", invocation.getBlock()->getIdentifier().getId());
	writeArgumentList(out, invocation.getArguments(), values);
}

/// Appends the external-function reference of a proxy call or address-of
/// operation: the full `@"symbol" "name"` form when symbols may be shown,
/// the deterministic `func_*` placeholder otherwise.
void writeExternalFunction(fmt::memory_buffer& out, const std::string& symbol, const std::string& name,
                           const WriterConfig& config) {
	if (config.showExternalSymbols()) {
		fmt::format_to(std::back_inserter(out), "@{} {}", quoted(symbol), quoted(name));
	} else {
		fmt::format_to(std::back_inserter(out), "func_*");
	}
}

void writeOperation(fmt::memory_buffer& out, const Operation& op, const IdMap& values, const WriterConfig& config) {
	using OpType = Operation::OperationType;
	auto it = std::back_inserter(out);
	// Only value-producing operations carry a printable id; void operations
	// (store, br, if, return, void calls) are never referenced.
	const auto id = op.getStamp() != Type::v ? values.ref(&op) : 0;
	switch (op.getOperationType()) {
	case OpType::AddOp:
	case OpType::SubOp:
	case OpType::MulOp:
	case OpType::DivOp:
	case OpType::ModOp:
	case OpType::AndOp:
	case OpType::OrOp: {
		const auto* bin = cast<BinaryOperation>(&op);
		fmt::format_to(it, "${} = ${} {} ${}", id, values.ref(bin->getLeftInput()),
		               binaryOpSymbol(op.getOperationType()), values.ref(bin->getRightInput()));
		break;
	}
	case OpType::CompareOp: {
		const auto* compareOp = cast<CompareOperation>(&op);
		fmt::format_to(it, "${} = ${} {} ${}", id, values.ref(compareOp->getLeftInput()),
		               comparatorSymbol(compareOp->getComparator()), values.ref(compareOp->getRightInput()));
		break;
	}
	case OpType::ShiftOp: {
		const auto* shiftOp = cast<ShiftOperation>(&op);
		fmt::format_to(it, "${} = ${} {} ${}", id, values.ref(shiftOp->getLeftInput()),
		               shiftOp->getType() == ShiftOperation::ShiftType::LS ? "<<" : ">>",
		               values.ref(shiftOp->getRightInput()));
		break;
	}
	case OpType::BinaryComp: {
		const auto* binaryCompOp = cast<BinaryCompOperation>(&op);
		const char* symbol = binaryCompOp->getType() == BinaryCompOperation::BOR    ? "|"
		                     : binaryCompOp->getType() == BinaryCompOperation::BAND ? "&"
		                                                                            : "^";
		fmt::format_to(it, "${} = ${} {} ${}", id, values.ref(binaryCompOp->getLeftInput()), symbol,
		               values.ref(binaryCompOp->getRightInput()));
		break;
	}
	case OpType::NotOp:
		fmt::format_to(it, "${} = !${}", id, values.ref(cast<NotOperation>(&op)->getInput()));
		break;
	case OpType::NegateOp: {
		const auto* negateOp = cast<NegateOperation>(&op);
		const char* symbol = (negateOp->getStamp() == Type::f32 || negateOp->getStamp() == Type::f64) ? "-" : "~";
		fmt::format_to(it, "${} = {}${}", id, symbol, values.ref(negateOp->getInput()));
		break;
	}
	case OpType::ConstIntOp:
		fmt::format_to(it, "${} = {}", id, cast<ConstIntOperation>(&op)->getValue());
		break;
	case OpType::ConstFloatOp:
		fmt::format_to(it, "${} = {}", id, cast<ConstFloatOperation>(&op)->getValue());
		break;
	case OpType::ConstBooleanOp:
		fmt::format_to(it, "${} = {}", id, cast<ConstBooleanOperation>(&op)->getValue());
		break;
	case OpType::ConstPtrOp: {
		// A pointer constant is a raw address in the producing process; it
		// cannot mean anything in the process that loads the IR. Only null
		// is portable; the display form keeps the historic `*` placeholder.
		if (cast<ConstPtrOperation>(&op)->getValue() == nullptr) {
			fmt::format_to(it, "${} = null", id);
		} else if (config.portable) {
			throw RuntimeException(fmt::format("Cannot serialize IR: ${} is a non-null pointer constant, which is "
			                                   "process-specific. Pass the pointer as a function parameter instead.",
			                                   id));
		} else {
			fmt::format_to(it, "${} = *", id);
		}
		break;
	}
	case OpType::CastOp:
		fmt::format_to(it, "${} = ${} cast_to {}", id, values.ref(cast<CastOperation>(&op)->getInput()),
		               toString(op.getStamp()));
		break;
	case OpType::LoadOp:
		fmt::format_to(it, "${} = load(${})", id, values.ref(cast<LoadOperation>(&op)->getAddress()));
		break;
	case OpType::StoreOp: {
		const auto* storeOp = cast<StoreOperation>(&op);
		fmt::format_to(it, "store(${}, ${})", values.ref(storeOp->getValue()), values.ref(storeOp->getAddress()));
		break;
	}
	case OpType::AllocaOp:
		fmt::format_to(it, "${} = alloca[{}]", id, cast<AllocaOperation>(&op)->getIndex());
		break;
	case OpType::SelectOp: {
		const auto* selectOp = cast<SelectOperation>(&op);
		fmt::format_to(it, "${} = select ${} ? ${} : ${}", id, values.ref(selectOp->getCondition()),
		               values.ref(selectOp->getTrueValue()), values.ref(selectOp->getFalseValue()));
		break;
	}
	case OpType::ProxyCallOp: {
		const auto* callOp = cast<ProxyCallOperation>(&op);
		if (op.getStamp() != Type::v) {
			fmt::format_to(it, "${} = ", id);
		}
		fmt::format_to(it, "call ");
		writeExternalFunction(out, callOp->getFunctionSymbol(), callOp->getFunctionName(), config);
		writeArgumentList(out, callOp->getInputArguments(), values);
		writeFunctionAttributes(out, callOp->getFunctionAttributes());
		break;
	}
	case OpType::IndirectCallOp: {
		const auto* callOp = cast<IndirectCallOperation>(&op);
		if (op.getStamp() != Type::v) {
			fmt::format_to(it, "${} = ", id);
		}
		fmt::format_to(it, "call_indirect ${}", values.ref(callOp->getFunctionPtrOperand()));
		writeArgumentList(out, callOp->getInputArguments(), values);
		writeFunctionAttributes(out, callOp->getFunctionAttributes());
		break;
	}
	case OpType::FunctionAddressOfOp: {
		const auto* addressOfOp = cast<FunctionAddressOfOperation>(&op);
		fmt::format_to(it, "${} = addressof ", id);
		writeExternalFunction(out, addressOfOp->getFunctionSymbol(), addressOfOp->getFunctionName(), config);
		break;
	}
	case OpType::BranchOp:
		fmt::format_to(it, "br ");
		writeInvocation(out, cast<BranchOperation>(&op)->getNextBlockInvocation(), values);
		break;
	case OpType::IfOp: {
		const auto* ifOp = cast<IfOperation>(&op);
		fmt::format_to(it, "if ${} ? ", values.ref(ifOp->getValue()));
		writeInvocation(out, ifOp->getTrueBlockInvocation(), values);
		fmt::format_to(it, " : ");
		writeInvocation(out, ifOp->getFalseBlockInvocation(), values);
		fmt::format_to(it, " prob({})", ifOp->getProbability());
		break;
	}
	case OpType::ReturnOp: {
		const auto* returnOp = cast<ReturnOperation>(&op);
		fmt::format_to(it, "return");
		if (returnOp->hasReturnValue()) {
			fmt::format_to(it, " (${})", values.ref(returnOp->getReturnValue()));
		}
		break;
	}
	default:
		if (config.portable) {
			throw RuntimeException(fmt::format("Cannot serialize IR: unsupported operation type {} (op ${})",
			                                   static_cast<uint32_t>(op.getOperationType()),
			                                   op.getIdentifier().getId()));
		}
		fmt::format_to(it, "${}", id);
		break;
	}
	fmt::format_to(it, " :{}", toString(op.getStamp()));

	// Opt-in source-location trailer of the display form: `; `-prefixed
	// comments that the parser skips.
	if (const auto* opts = config.displayOptions;
	    opts != nullptr && opts->showSourceLocations && opts->resolver != nullptr) {
		if (const auto* tag = op.getSourceTag()) {
			const auto frames = opts->resolver->resolveStack(tag);
			if (!frames.empty()) {
				// Innermost frame on the same line; outer frames continue on
				// their own lines, outermost last. Two tabs line them up
				// under the op text that the block writer indents with one tab.
				const auto& innermost = frames.back();
				fmt::format_to(it, "  ; at {}:{} ({})", innermost.file, innermost.line, innermost.function);
				for (auto frame = frames.rbegin() + 1; frame != frames.rend(); ++frame) {
					fmt::format_to(it, "\n\t\t; inlined from {}:{} ({})", frame->file, frame->line, frame->function);
				}
			}
		}
	}
}

void writeBlock(fmt::memory_buffer& out, const BasicBlock& block, const IdMap& values, const WriterConfig& config) {
	auto it = std::back_inserter(out);
	fmt::format_to(it, "\nBlock_{}(", block.getIdentifier().getId());
	const auto& args = block.getArguments();
	for (size_t i = 0; i < args.size(); ++i) {
		if (i > 0) {
			fmt::format_to(it, ", ");
		}
		fmt::format_to(it, "${}:{}", values.ref(args[i]), toString(args[i]->getStamp()));
	}
	fmt::format_to(it, "):\n");
	for (const auto* operation : block.getOperations()) {
		out.push_back('\t');
		writeOperation(out, *operation, values, config);
		out.push_back('\n');
	}
}

void writeFunction(fmt::memory_buffer& out, const FunctionOperation& function, const WriterConfig& config) {
	auto it = std::back_inserter(out);
	fmt::format_to(it, "{}(", function.getName());
	const auto* entry = function.getEntryBlock();
	if (config.portable && entry == nullptr) {
		throw RuntimeException(
		    fmt::format("Cannot serialize IR: function '{}' has no entry block", function.getName()));
	}
	const IdMap values = config.portable ? IdMap(function) : IdMap();
	// The function's parameters are the entry block's arguments; the
	// trace-to-IR conversion leaves FunctionOperation::inputArgs empty.
	// Hand-built graphs (test fixtures) may instead populate inputArgs.
	const auto& argTypes = function.getInputArgs();
	const auto& argNames = function.getInputArgNames();
	if (entry != nullptr && argTypes.empty() && argNames.empty()) {
		const auto& params = entry->getArguments();
		for (size_t i = 0; i < params.size(); ++i) {
			if (i > 0) {
				fmt::format_to(it, ", ");
			}
			fmt::format_to(it, "${}:{}", values.ref(params[i]), toString(params[i]->getStamp()));
		}
	} else {
		for (size_t i = 0; i < argTypes.size(); ++i) {
			if (i > 0) {
				fmt::format_to(it, ", ");
			}
			if (i < argNames.size()) {
				fmt::format_to(it, "{}:{}", argNames[i], toString(argTypes[i]));
			} else {
				fmt::format_to(it, "{}", toString(argTypes[i]));
			}
		}
	}
	fmt::format_to(it, ") :{}", toString(function.getOutputArg()));
	const auto& allocaSpecs = function.getAllocaSpecs();
	if (!allocaSpecs.empty()) {
		fmt::format_to(it, " allocas[");
		for (size_t i = 0; i < allocaSpecs.size(); ++i) {
			fmt::format_to(it, "{}({},{})", i > 0 ? "," : "", allocaSpecs[i].size, allocaSpecs[i].align);
		}
		fmt::format_to(it, "]");
	}
	// Sort the attribute map so the output is deterministic.
	const std::map<std::string, std::string> attributes(function.getAttributes().begin(),
	                                                    function.getAttributes().end());
	if (!attributes.empty()) {
		fmt::format_to(it, " attrs[");
		bool first = true;
		for (const auto& [key, value] : attributes) {
			fmt::format_to(it, "{}{}={}", first ? "" : ",", quoted(key), quoted(value));
			first = false;
		}
		fmt::format_to(it, "]");
	}
	fmt::format_to(it, " {{");
	for (const auto* block : function.getBasicBlocks()) {
		writeBlock(out, *block, values, config);
	}
	fmt::format_to(it, "}}\n");
}

std::string writeGraph(const IRGraph& graph, const WriterConfig& config) {
	fmt::memory_buffer out;
	fmt::format_to(std::back_inserter(out), "nautilus {{\n");
	for (const auto* function : graph.getFunctionOperations()) {
		writeFunction(out, *function, config);
	}
	fmt::format_to(std::back_inserter(out), "}} //nautilus\n");
	return fmt::to_string(out);
}

} // namespace

std::string serializeIR(const IRGraph& graph) {
	WriterConfig config;
	config.portable = true;
	return writeGraph(graph, config);
}

std::string printIR(const IRGraph& graph, const IRPrintOptions& options) {
	WriterConfig config;
	config.displayOptions = &options;
	return writeGraph(graph, config);
}

std::string printOperation(const Operation& operation) {
	fmt::memory_buffer out;
	WriterConfig config;
	writeOperation(out, operation, IdMap(), config);
	return fmt::to_string(out);
}

} // namespace nautilus::compiler::ir
