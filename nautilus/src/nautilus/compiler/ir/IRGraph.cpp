
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/NegateOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
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
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/operations/StoreOperation.hpp"
#include "nautilus/logging.hpp"
#include "nautilus/tracing/tag/SourceLocationResolver.hpp"
#include <cassert>
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

/// The graph currently being formatted.  A call operation stores only its
/// callee's FunctionId, and resolving that to a name and a linkage needs the
/// module's function table -- which the per-Operation fmt formatter has no
/// other way to reach.  Set for the duration of formatting the graph, so a
/// bare `fmt::format("{}", someOperation)` outside that scope still prints
/// exactly what it always did.
thread_local const IRGraph* currentPrintGraph = nullptr;

struct PrintGraphScope {
	explicit PrintGraphScope(const IRGraph* graph) : previous(currentPrintGraph) {
		currentPrintGraph = graph;
	}
	~PrintGraphScope() {
		currentPrintGraph = previous;
	}
	PrintGraphScope(const PrintGraphScope&) = delete;
	PrintGraphScope& operator=(const PrintGraphScope&) = delete;

	const IRGraph* previous;
};

/// The exception region of the function currently being formatted, or nullptr
/// before ExceptionRegionPreparationPass has run (or for a function with no
/// exceptional call sites). A call operation knows only whether it *might*
/// unwind (`requiresExceptionHandling()`); which landing pad it actually
/// targets is decided later and lives in the enclosing FunctionOperation's
/// side table, which the per-Operation fmt formatter has no other way to
/// reach. Set for the duration of formatting one function's blocks.
thread_local const FunctionExceptionRegion* currentPrintExceptionRegion = nullptr;

/// The region table of the function currently being formatted. An operation carries only
/// the index of the region() it was traced inside; the attributes that index names live on
/// the enclosing FunctionOperation, which the per-Operation fmt formatter has no other way
/// to reach. Null outside the scope of formatting a function, which is why a bare
/// `fmt::format("{}", someOperation)` prints exactly what it always did.
thread_local const std::vector<RegionSpec>* currentPrintRegions = nullptr;

struct PrintRegionScope {
	explicit PrintRegionScope(const std::vector<RegionSpec>* regions) : previous(currentPrintRegions) {
		currentPrintRegions = regions;
	}
	~PrintRegionScope() {
		currentPrintRegions = previous;
	}
	PrintRegionScope(const PrintRegionScope&) = delete;
	PrintRegionScope& operator=(const PrintRegionScope&) = delete;

	const std::vector<RegionSpec>* previous;
};

struct PrintExceptionRegionScope {
	explicit PrintExceptionRegionScope(const FunctionExceptionRegion* region) : previous(currentPrintExceptionRegion) {
		currentPrintExceptionRegion = region;
	}
	~PrintExceptionRegionScope() {
		currentPrintExceptionRegion = previous;
	}
	PrintExceptionRegionScope(const PrintExceptionRegionScope&) = delete;
	PrintExceptionRegionScope& operator=(const PrintExceptionRegionScope&) = delete;

	const FunctionExceptionRegion* previous;
};

/// The landing-pad suffix for @p call (` -> pad_N`), or empty when the region
/// hasn't been computed yet, @p call isn't one of its exceptional call sites,
/// or it is one with nothing to clean up on unwind (no pad to link to).
/// Linear scan: this only runs while formatting IR for humans, and a
/// function's call-site list is small.
std::string padLinkSuffix(const Operation* call) {
	if (currentPrintExceptionRegion == nullptr) {
		return {};
	}
	for (const auto& site : currentPrintExceptionRegion->callSites) {
		if (site.call == call) {
			return site.hasPad() ? fmt::format(" -> pad_{}", site.padIndex) : "";
		}
	}
	return {};
}

/// How a callee should be spelled at a call site.
///
/// An internal callee's name comes from the user (a NautilusFunction is
/// constructed with it), so it is deterministic and safe to print into a
/// checked-in reference dump. A native callee's name comes from dladdr and is
/// a stringified address wherever that misses -- different on every machine,
/// and on every run -- so it stays behind the address-logging flag, exactly as
/// the whole call did before.
std::string nativeSpelling(const std::string& storedName) {
	return log::options::getLogAddresses() ? storedName : "func_*";
}

std::string calleeSpelling(FunctionId calleeId, const std::string& storedName) {
	if (currentPrintGraph != nullptr && calleeId != INVALID_FUNCTION_ID &&
	    currentPrintGraph->getFunctionTable().contains(calleeId)) {
		const auto& target = currentPrintGraph->getFunctionTarget(calleeId);
		if (target.getLinkage() == Linkage::Internal) {
			return target.getName().get();
		}
		// A native callee's name is withheld for reproducibility, which leaves
		// two distinct externals indistinguishable at their call sites. The id
		// is the discriminator that survives: it ties the call to one line of
		// the declaration region, and it is assigned deterministically.
		return fmt::format("{}#{}", nativeSpelling(storedName), calleeId);
	}
	return nativeSpelling(storedName);
}

/// `[attr, attr, ...]` for whatever @p attrs asserts beyond the pessimistic
/// default (ModRef, willReturn=false, noUnwind=false) -- empty when nothing
/// does, so a callee no pass has looked at yet prints exactly as it always
/// did. Names follow LLVM's own vocabulary for the same guarantees, since
/// FunctionAttributes is explicitly modeled on it.
std::string attributesSuffix(const FunctionAttributes& attrs) {
	std::vector<std::string> tokens;
	switch (attrs.modRefInfo) {
	case ModRefInfo::NoModRef:
		tokens.emplace_back("readnone");
		break;
	case ModRefInfo::Ref:
		tokens.emplace_back("readonly");
		break;
	case ModRefInfo::Mod:
		tokens.emplace_back("writeonly");
		break;
	case ModRefInfo::ModRef:
		break;
	}
	if (attrs.willReturn) {
		tokens.emplace_back("willreturn");
	}
	if (attrs.noUnwind) {
		tokens.emplace_back("nounwind");
	}
	if (tokens.empty()) {
		return {};
	}
	std::string out = " [";
	for (size_t i = 0; i < tokens.size(); ++i) {
		if (i > 0) {
			out += ", ";
		}
		out += tokens[i];
	}
	out += "]";
	return out;
}

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

FunctionId IRGraph::internCallee(const CalleeDescriptor& descriptor) {
	return functionTable.intern(descriptor);
}

void IRGraph::defineFunction(FunctionId id, FunctionOperation* functionOperation) {
	functionTable.define(id, functionOperation);
}

const std::string& IRGraph::getEmissionName(const FunctionOperation* functionOperation) const {
	const auto id = functionTable.findByDefinition(functionOperation);
	assert(id != INVALID_FUNCTION_ID && "every FunctionOperation is bound to a table entry by defineFunction()");
	return functionTable.get(id).getName().forEmission();
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
	static auto format(const nautilus::compiler::ir::OperationIdentifier& op, format_context& ctx)
	    -> format_context::iterator {
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
	static auto format(const nautilus::compiler::ir::BasicBlockInvocation& op, format_context& ctx)
	    -> format_context::iterator {
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
struct formatter<nautilus::compiler::ir::CallOperation> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::CallOperation& op, format_context& ctx)
	    -> format_context::iterator {
		auto out = ctx.out();

		if (op.getStamp() != nautilus::Type::v) {
			fmt::format_to(out, "${} = ", op.getIdentifier().getId());
		}
		fmt::format_to(out, "call {}(", nautilus::compiler::ir::calleeSpelling(op.getCalleeId(), op.getFunctionName()));
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
	case OpType::CallOp:
		fmt::format_to(out, "{}", *nautilus::compiler::ir::cast<CallOperation>(&op));
		break;
	case OpType::IndirectCallOp: {
		// The callee is an SSA value, so there is no name to print -- the
		// operand that holds the pointer is the whole story.
		const auto* call = nautilus::compiler::ir::cast<IndirectCallOperation>(&op);
		if (op.getStamp() != nautilus::Type::v) {
			fmt::format_to(out, "${} = ", op.getIdentifier().getId());
		}
		fmt::format_to(out, "call {}(", call->getFunctionPtrOperand()->getIdentifier());
		const auto args = call->getInputArguments();
		for (size_t i = 0; i < args.size(); ++i) {
			if (i > 0) {
				fmt::format_to(out, ",");
			}
			fmt::format_to(out, "{}", args[i]->getIdentifier());
		}
		fmt::format_to(out, ")");
		break;
	}
	case OpType::FunctionAddressOfOp: {
		const auto* addrOf = nautilus::compiler::ir::cast<FunctionAddressOfOperation>(&op);
		fmt::format_to(out, "{} = func_addr {}", op.getIdentifier(),
		               nautilus::compiler::ir::calleeSpelling(addrOf->getCalleeId(), addrOf->getFunctionName()));
		break;
	}
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
		const char* symbol =
		    (negateOp->getStamp() == nautilus::Type::f32 || negateOp->getStamp() == nautilus::Type::f64) ? "-" : "~";
		fmt::format_to(out, "{} = {}{}", op.getIdentifier(), symbol, negateOp->getInput()->getIdentifier());
		break;
	}
	case OpType::AllocaOp: {
		const auto* alloca = nautilus::compiler::ir::cast<AllocaOperation>(&op);
		fmt::format_to(out, "{} = alloca[{}]", op.getIdentifier(), alloca->getIndex());
		break;
	}
	default:
		fmt::format_to(out, "{}", op.getIdentifier().toString());
		break;
	}
	fmt::format_to(out, " :{}", toString(op.getStamp()));
	fmt::format_to(out, "{}", nautilus::compiler::ir::padLinkSuffix(&op));

	// Opt-in source-location trailer.  The TLS pointer is only non-null
	// inside the scope of `IRGraph::toString(options)`.
	bool trailerStarted = false;
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
				for (auto it = frames.rbegin() + 1; it != frames.rend(); ++it) {
					fmt::format_to(out, "\n\t\t; inlined from {}:{} ({})", it->file, it->line, it->function);
				}
				trailerStarted = true;
			}
		}
	}

	// Region trailer (docs/region.md): the region() call site this operation was traced
	// inside, then the ones enclosing it, innermost first.  Printed unconditionally --
	// unlike the source locations above it needs no resolver, and an operation outside
	// every region prints nothing, so IR that uses no region is unaffected.
	const auto* regions = nautilus::compiler::ir::currentPrintRegions;
	auto regionIndex = op.getRegionIndex();
	if (regions != nullptr && regionIndex < regions->size()) {
		const char* separator = trailerStarted ? "\n\t\t; region " : "  ; region ";
		while (regionIndex < regions->size()) {
			const auto& region = (*regions)[regionIndex];
			fmt::format_to(out, "{}{}", separator, region.attributes.toString());
			separator = "\n\t\t; nested in region ";
			regionIndex = region.parent;
		}
	}
	return out;
}

template <>
struct formatter<nautilus::compiler::ir::BasicBlock> : formatter<std::string_view> {
	static auto format(const nautilus::compiler::ir::BasicBlock& block, format_context& ctx)
	    -> format_context::iterator {
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
	static auto format(const nautilus::compiler::ir::FunctionOperation& func, format_context& ctx)
	    -> format_context::iterator {
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
		fmt::format_to(out, ") :{}", toString(func.getOutputArg()));
		// Derived by FunctionAttributeInferencePass (or absent before it runs);
		// the internal FunctionOperation is its own declaration, so its
		// attributes belong on this signature line rather than a separate one.
		if (nautilus::compiler::ir::currentPrintGraph != nullptr) {
			const auto id = nautilus::compiler::ir::currentPrintGraph->getFunctionTable().findByDefinition(&func);
			if (id != nautilus::compiler::ir::INVALID_FUNCTION_ID &&
			    nautilus::compiler::ir::currentPrintGraph->getFunctionTable().contains(id)) {
				fmt::format_to(out, "{}",
				               nautilus::compiler::ir::attributesSuffix(
				                   nautilus::compiler::ir::currentPrintGraph->getFunctionTarget(id).getAttributes()));
			}
		}
		fmt::format_to(out, " {{");
		{
			nautilus::compiler::ir::PrintExceptionRegionScope exceptionScope(
			    func.exceptionRegion.has_value() ? &*func.exceptionRegion : nullptr);
			nautilus::compiler::ir::PrintRegionScope regionScope(&func.getRegionSpecs());
			for (const auto* block : func.getBasicBlocks()) {
				fmt::format_to(out, "{}", *block);
			}
		}
		// Which pad each call targets is already inline on the call itself
		// (padLinkSuffix, in the shared Operation trailer); this section is
		// only the pads' own definitions -- the destructor calls a landing
		// pad actually contains, which appear nowhere else in the dump. A
		// function with only no-pad exceptional call sites has nothing left
		// to say here: `-> (no_pad)` on the call already said it.
		if (func.exceptionRegion.has_value()) {
			const auto& region = *func.exceptionRegion;
			if (!region.pads.empty()) {
				fmt::format_to(out, "exception_region:\n");
				for (size_t i = 0; i < region.pads.size(); ++i) {
					fmt::format_to(out, "\tpad_{}:\n", i);
					for (const auto* op : region.pads[i].block->getOperations()) {
						fmt::format_to(out, "\t\t{}\n", *op);
					}
				}
			}
		}
		fmt::format_to(out, "}}\n");
		return out;
	}
};
} // namespace fmt

auto fmt::formatter<nautilus::compiler::ir::IRGraph>::format(const nautilus::compiler::ir::IRGraph& graph,
                                                             format_context& ctx) -> format_context::iterator {
	// Make the module's function table reachable from the per-Operation
	// formatter, so a call site can spell its callee rather than swallowing it.
	nautilus::compiler::ir::PrintGraphScope graphScope(&graph);

	auto out = ctx.out();
	fmt::format_to(out, "nautilus {{\n");

	// The declaration region: every callee this module reaches that is *not*
	// defined here. An internal target is deliberately absent -- its
	// FunctionOperation below is its declaration, and repeating it would let
	// the two drift apart, which is the whole failure mode the function table
	// exists to remove.
	for (const auto& target : graph.getFunctionTable().getTargets()) {
		if (target.getLinkage() == nautilus::compiler::ir::Linkage::Internal) {
			continue;
		}
		// The id, not the name, identifies the entry: a native callee's name
		// comes from dladdr and is a stringified address wherever that misses,
		// so it is printed under the same rule as a call site.
		fmt::format_to(out, "declare {} #{} {}(",
		               target.getLinkage() == nautilus::compiler::ir::Linkage::Intrinsic ? "intrinsic" : "external",
		               target.getId(), nautilus::compiler::ir::nativeSpelling(target.getName().get()));
		const auto params = target.getParamTypes();
		for (size_t i = 0; i < params.size(); ++i) {
			if (i > 0) {
				fmt::format_to(out, ", ");
			}
			fmt::format_to(out, "{}", toString(params[i]));
		}
		fmt::format_to(out, ") :{}{}\n", toString(target.getResultType()),
		               nautilus::compiler::ir::attributesSuffix(target.getAttributes()));
	}

	// Print all function operations
	for (const auto* func : graph.getFunctionOperations()) {
		fmt::format_to(out, "{}", *func);
	}

	fmt::format_to(out, "}} //nautilus\n");
	return out;
}

std::string nautilus::compiler::ir::IRGraph::toString() const {
	return fmt::to_string(*this);
}

std::string nautilus::compiler::ir::IRGraph::toString(const nautilus::compiler::ir::IRPrintOptions& options) const {
	PrintOptionsScope scope(&options);
	return fmt::to_string(*this);
}
