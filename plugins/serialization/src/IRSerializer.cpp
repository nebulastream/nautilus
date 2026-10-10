#include "Crc32c.hpp"
#include "nautilus/compiler/ir/IRGraph.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/BinaryCompOperation.hpp"
#include "nautilus/compiler/ir/operations/BinaryOperations/ShiftOperation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/CallOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstBooleanOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstFloatOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstPtrOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionAddressOfOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/compiler/ir/operations/LogicalOperations/CompareOperation.hpp"
#include "nautilus/serialization/IRBinaryFormat.hpp"
#include "nautilus/serialization/IRSerialization.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
#include <fmt/format.h>
#include <string>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

namespace nautilus::serialization {

using namespace compiler::ir;

namespace {

uint32_t checkedU32(size_t value, const char* what) {
	if (value >= NONE) {
		throw SerializationException(fmt::format("{} ({}) exceeds the format's 32-bit limit", what, value));
	}
	return static_cast<uint32_t>(value);
}

uint8_t attributeFlags(const FunctionAttributes& attrs) {
	return static_cast<uint8_t>((attrs.willReturn ? ATTR_WILL_RETURN : 0) | (attrs.noUnwind ? ATTR_NO_UNWIND : 0));
}

WireOpcode wireOpcode(const Operation& op) {
	using OT = Operation::OperationType;
	switch (op.getOperationType()) {
	case OT::BasicBlockArgument:
		return WireOpcode::BlockArgument;
	case OT::ConstIntOp:
		return WireOpcode::ConstInt;
	case OT::ConstFloatOp:
		return WireOpcode::ConstFloat;
	case OT::ConstBooleanOp:
		return WireOpcode::ConstBoolean;
	case OT::ConstPtrOp:
		return WireOpcode::ConstPtr;
	case OT::AddOp:
		return WireOpcode::Add;
	case OT::SubOp:
		return WireOpcode::Sub;
	case OT::MulOp:
		return WireOpcode::Mul;
	case OT::DivOp:
		return WireOpcode::Div;
	case OT::ModOp:
		return WireOpcode::Mod;
	case OT::AndOp:
		return WireOpcode::And;
	case OT::OrOp:
		return WireOpcode::Or;
	case OT::NotOp:
		return WireOpcode::Not;
	case OT::NegateOp:
		return WireOpcode::Negate;
	case OT::CompareOp:
		return WireOpcode::Compare;
	case OT::ShiftOp:
		return WireOpcode::Shift;
	case OT::BinaryComp:
		return WireOpcode::BinaryComp;
	case OT::CastOp:
		return WireOpcode::Cast;
	case OT::LoadOp:
		return WireOpcode::Load;
	case OT::StoreOp:
		return WireOpcode::Store;
	case OT::SelectOp:
		return WireOpcode::Select;
	case OT::AllocaOp:
		return WireOpcode::Alloca;
	case OT::CallOp:
		return WireOpcode::Call;
	case OT::IndirectCallOp:
		return WireOpcode::IndirectCall;
	case OT::FunctionAddressOfOp:
		return WireOpcode::FunctionAddressOf;
	case OT::BranchOp:
		return WireOpcode::Branch;
	case OT::IfOp:
		return WireOpcode::If;
	case OT::ReturnOp:
		return WireOpcode::Return;
	case OT::BlockInvocation:
	case OT::FunctionOp:
	case OT::MLIR_YIELD:
		break;
	}
	throw SerializationException(
	    fmt::format("operation type {} cannot appear in a block", static_cast<int>(op.getOperationType())));
}

WireComparator wireComparator(CompareOperation::Comparator comparator) {
	switch (comparator) {
	case CompareOperation::EQ:
		return WireComparator::EQ;
	case CompareOperation::NE:
		return WireComparator::NE;
	case CompareOperation::LT:
		return WireComparator::LT;
	case CompareOperation::LE:
		return WireComparator::LE;
	case CompareOperation::GT:
		return WireComparator::GT;
	case CompareOperation::GE:
		return WireComparator::GE;
	}
	throw SerializationException("unknown comparator");
}

WireShift wireShift(ShiftOperation::ShiftType type) {
	switch (type) {
	case ShiftOperation::LS:
		return WireShift::Left;
	case ShiftOperation::RS:
		return WireShift::Right;
	}
	throw SerializationException("unknown shift kind");
}

WireBitwise wireBitwise(BinaryCompOperation::Type type) {
	switch (type) {
	case BinaryCompOperation::BAND:
		return WireBitwise::And;
	case BinaryCompOperation::BOR:
		return WireBitwise::Or;
	case BinaryCompOperation::XOR:
		return WireBitwise::Xor;
	}
	throw SerializationException("unknown bitwise kind");
}

/// Collects every section's records while the graph is walked, then lays
/// them out into one buffer.
class Writer {
public:
	Writer(const IRGraph& graph, const SerializeOptions& options) : graph(graph), options(options) {
		if (options.pointerMode == PointerMode::Portable && options.namer == nullptr) {
			throw SerializationException("portable serialization needs a SymbolNamer");
		}
	}

	std::vector<std::byte> write() {
		module.compilationUnitId = str(graph.getId());

		const auto& functionOps = graph.getFunctionOperations();
		for (size_t i = 0; i < functionOps.size(); ++i) {
			functionIndex.emplace(functionOps[i], checkedU32(i, "function count"));
		}
		writeTargets();
		for (const auto* function : functionOps) {
			writeFunction(*function);
		}
		return assemble();
	}

private:
	// ── pools ───────────────────────────────────────────────────────────────

	StringRef str(std::string_view text) {
		auto [it, inserted] = stringOffsets.try_emplace(std::string(text), 0);
		if (inserted) {
			it->second = checkedU32(strings.size(), "string pool size");
			strings.append(text);
			checkedU32(strings.size(), "string pool size");
		}
		return StringRef {it->second, checkedU32(text.size(), "string length")};
	}

	StringRef cstr(const char* text) {
		return text == nullptr ? StringRef {NONE, 0} : str(text);
	}

	SourceLocationRecord location(const SourceLocation& loc) {
		return SourceLocationRecord {cstr(loc.file), cstr(loc.function), loc.line, loc.column};
	}

	uint32_t constant(uint64_t bits) {
		constants.push_back(bits);
		return checkedU32(constants.size() - 1, "constant pool size");
	}

	uint32_t type(Type t) {
		types.push_back(static_cast<uint8_t>(t));
		return checkedU32(types.size() - 1, "type pool size");
	}

	/// @p internalIdentity marks a pointer that is an in-module callee's
	/// identity key rather than code; it cannot be named and need not be,
	/// because backends reach such callees through the function table.
	uint32_t pointer(const void* address, std::string_view context, bool internalIdentity = false) {
		const bool portable = options.pointerMode == PointerMode::Portable;
		if (portable && internalIdentity) {
			address = nullptr;
		}
		if (const auto it = pointerIndex.find(address); it != pointerIndex.end()) {
			return it->second;
		}
		PointerRecord record {};
		record.symbol = StringRef {NONE, 0};
		if (address == nullptr) {
			record.kind = static_cast<uint8_t>(PointerKind::Null);
		} else {
			auto name = options.namer != nullptr ? options.namer->nameOf(address) : std::nullopt;
			if (portable) {
				if (!name) {
					throw SerializationException(fmt::format(
					    "cannot serialize {} portably: the SymbolNamer has no name for address {}", context, address));
				}
				record.kind = static_cast<uint8_t>(PointerKind::Symbol);
			} else {
				record.kind = static_cast<uint8_t>(PointerKind::Raw);
				record.address = reinterpret_cast<uintptr_t>(address);
			}
			if (name) {
				record.symbol = str(*name);
			}
		}
		pointers.push_back(record);
		const auto index = checkedU32(pointers.size() - 1, "pointer table size");
		pointerIndex.emplace(address, index);
		return index;
	}

	/// A name for diagnostics: the stored one, else the function table's,
	/// which is never empty.
	std::string calleeName(FunctionId id, const std::string& storedName) const {
		if (!storedName.empty()) {
			return storedName;
		}
		if (id == INVALID_FUNCTION_ID || !graph.getFunctionTable().contains(id)) {
			return "<indirect>";
		}
		return graph.getFunctionTarget(id).getName().get();
	}

	bool isInternalCallee(FunctionId id) const {
		return id != INVALID_FUNCTION_ID && graph.getFunctionTable().contains(id) &&
		       graph.getFunctionTarget(id).getLinkage() == Linkage::Internal;
	}

	// ── module-level tables ─────────────────────────────────────────────────

	void writeTargets() {
		for (const auto& target : graph.getFunctionTable().getTargets()) {
			TargetRecord record {};
			const auto& name = target.getName();
			record.mangled = str(name.getMangled());
			record.demangled = str(name.getDemangled());
			record.custom = str(name.getCustom());
			record.minted = str(name.getMinted());
			record.emission = str(name.forEmission());
			const auto attrs = target.getAttributes();
			record.modRef = static_cast<uint8_t>(attrs.modRefInfo);
			record.attrFlags = attributeFlags(attrs);
			record.resultType = static_cast<uint8_t>(target.getResultType());
			record.paramTypesBegin = checkedU32(types.size(), "type pool size");
			if (const auto* native = target.getNative()) {
				record.linkage = static_cast<uint8_t>(WireLinkage::Native);
				record.payload = pointer(native->address, fmt::format("callee '{}'", name.get()));
				for (const auto paramType : native->paramTypes) {
					type(paramType);
				}
				record.paramCount = checkedU32(native->paramTypes.size(), "parameter count");
			} else {
				record.linkage = static_cast<uint8_t>(WireLinkage::Internal);
				record.payload = NONE;
				if (auto* definition = target.getDefinition()) {
					const auto it = functionIndex.find(definition);
					if (it == functionIndex.end()) {
						throw SerializationException(fmt::format(
						    "function table entry '{}' is defined by a function the graph does not hold", name.get()));
					}
					record.payload = it->second;
				}
			}
			targets.push_back(record);
		}
	}

	// ── functions ───────────────────────────────────────────────────────────

	struct FunctionScope {
		std::vector<const BasicBlock*> blocks;
		std::unordered_map<const BasicBlock*, uint32_t> blockIndex;
		std::unordered_map<const Operation*, uint32_t> valueIndex;
	};

	static void addBlock(FunctionScope& scope, const BasicBlock* block) {
		if (block != nullptr &&
		    scope.blockIndex.try_emplace(block, static_cast<uint32_t>(scope.blocks.size())).second) {
			scope.blocks.push_back(block);
		}
	}

	/// The function body in order, then every block only a side table or a
	/// terminator reaches (landing pads, and in principle a merge block that
	/// is not in the body), in discovery order.
	static FunctionScope collectBlocks(const FunctionOperation& function) {
		FunctionScope scope;
		for (const auto* block : function.getBasicBlocks()) {
			if (scope.blockIndex.contains(block)) {
				throw SerializationException(fmt::format("function '{}' lists block {} twice", function.getName(),
				                                         block->getIdentifier().getId()));
			}
			addBlock(scope, block);
		}
		if (function.exceptionRegion) {
			for (const auto& pad : function.exceptionRegion->pads) {
				addBlock(scope, pad.block);
			}
		}
		for (size_t i = 0; i < scope.blocks.size(); ++i) {
			const auto& operations = scope.blocks[i]->getOperations();
			if (operations.empty()) {
				continue;
			}
			auto* terminator = const_cast<Operation*>(operations.back());
			if (auto* branch = dyn_cast<BranchOperation>(terminator)) {
				addBlock(scope, branch->getNextBlockInvocation().getBlock());
			} else if (auto* ifOp = dyn_cast<IfOperation>(terminator)) {
				addBlock(scope, ifOp->getTrueBlockInvocation().getBlock());
				addBlock(scope, ifOp->getFalseBlockInvocation().getBlock());
				addBlock(scope, ifOp->getMergeBlock());
			}
		}
		return scope;
	}

	uint32_t value(const FunctionScope& scope, const Operation* op, const FunctionOperation& function) {
		if (op == nullptr) {
			throw SerializationException(fmt::format("function '{}' has a null operand", function.getName()));
		}
		const auto it = scope.valueIndex.find(op);
		if (it == scope.valueIndex.end()) {
			throw SerializationException(fmt::format("function '{}' uses value {} that no block of it defines",
			                                         function.getName(), op->getIdentifier().getId()));
		}
		return it->second;
	}

	uint32_t blockRef(const FunctionScope& scope, const BasicBlock* block) {
		return block == nullptr ? NONE : scope.blockIndex.at(block);
	}

	uint32_t edge(const FunctionScope& scope, const BasicBlockInvocation& invocation,
	              const FunctionOperation& function) {
		if (invocation.getBlock() == nullptr) {
			throw SerializationException(
			    fmt::format("function '{}' has a terminator without a target block", function.getName()));
		}
		EdgeRecord record {};
		record.targetBlock = blockRef(scope, invocation.getBlock());
		record.argumentsBegin = checkedU32(operands.size(), "operand pool size");
		for (const auto* argument : invocation.getArguments()) {
			operands.push_back(value(scope, argument, function));
		}
		record.argumentCount = checkedU32(invocation.getArguments().size(), "edge argument count");
		edges.push_back(record);
		return checkedU32(edges.size() - 1, "edge count");
	}

	template <typename Destructor>
	uint32_t call(const FunctionScope& scope, const FunctionOperation& function, const std::string& symbol,
	              const std::string& name, FunctionId calleeId, const void* functionPtr, const void* captureFunc,
	              const FunctionAttributes& attrs, uint8_t callFlags, const std::vector<Destructor>& callDestructors) {
		CallRecord record {};
		record.symbol = str(symbol);
		record.name = str(name);
		const auto displayName = calleeName(calleeId, name);
		record.functionPtr =
		    pointer(functionPtr, fmt::format("call target '{}'", displayName), isInternalCallee(calleeId));
		record.captureFunc = pointer(
		    captureFunc, fmt::format("exception capture thunk of call '{}' in '{}'", displayName, function.getName()));
		record.modRef = static_cast<uint8_t>(attrs.modRefInfo);
		record.attrFlags = attributeFlags(attrs);
		record.callFlags = callFlags;
		record.destructorsBegin = checkedU32(destructors.size(), "destructor count");
		record.destructorCount = checkedU32(callDestructors.size(), "destructor count");
		for (const auto& destructor : callDestructors) {
			DestructorRecord d {};
			d.address = value(scope, destructor.address, function);
			d.functionPtr = pointer(destructor.functionPtr, fmt::format("destructor '{}'", destructor.functionName));
			d.symbol = str(destructor.functionSymbol);
			d.name = str(destructor.functionName);
			destructors.push_back(d);
		}
		calls.push_back(record);
		return checkedU32(calls.size() - 1, "call count");
	}

	OpRecord operation(const FunctionScope& scope, const FunctionOperation& function, const Operation& op) {
		OpRecord record {};
		const auto opcode = wireOpcode(op);
		record.opcode = static_cast<uint8_t>(opcode);
		record.type = static_cast<uint8_t>(op.getStamp());
		record.region = op.getRegionIndex();
		record.identifier = op.getIdentifier().getId();
		record.payload0 = record.payload1 = record.payload2 = NONE;
		record.operandsBegin = checkedU32(operands.size(), "operand pool size");
		for (const auto* input : op.getInputs()) {
			operands.push_back(value(scope, input, function));
		}
		record.operandCount = checkedU32(op.getInputs().size(), "operand count");

		auto& mutableOp = const_cast<Operation&>(op);
		switch (opcode) {
		case WireOpcode::ConstInt:
			record.payload0 = constant(static_cast<uint64_t>(cast<ConstIntOperation>(&op)->getValue()));
			break;
		case WireOpcode::ConstFloat:
			record.payload0 = constant(std::bit_cast<uint64_t>(cast<ConstFloatOperation>(&op)->getValue()));
			break;
		case WireOpcode::ConstBoolean:
			record.aux = cast<ConstBooleanOperation>(&op)->getValue() ? 1 : 0;
			break;
		case WireOpcode::ConstPtr:
			record.payload0 =
			    pointer(cast<ConstPtrOperation>(&op)->getValue(),
			            fmt::format("constant pointer ${} in '{}'", record.identifier, function.getName()));
			break;
		case WireOpcode::Compare:
			record.aux = static_cast<uint32_t>(wireComparator(cast<CompareOperation>(&op)->getComparator()));
			break;
		case WireOpcode::Shift:
			record.aux = static_cast<uint32_t>(wireShift(cast<ShiftOperation>(&op)->getType()));
			break;
		case WireOpcode::BinaryComp:
			record.aux = static_cast<uint32_t>(wireBitwise(cast<BinaryCompOperation>(&op)->getType()));
			break;
		case WireOpcode::Alloca:
			record.payload0 = cast<AllocaOperation>(&op)->getIndex();
			break;
		case WireOpcode::Call: {
			auto* callOp = cast<CallOperation>(&mutableOp);
			const uint8_t flags =
			    static_cast<uint8_t>((callOp->requiresExceptionHandling() ? CALL_EXCEPTION_HANDLING : 0) |
			                         (callOp->isNautilusFunctionCall() ? CALL_IS_NAUTILUS_CALL : 0));
			record.payload0 = callOp->getCalleeId();
			record.payload1 = call(scope, function, callOp->getFunctionSymbol(), callOp->getFunctionName(),
			                       callOp->getCalleeId(), callOp->getFunctionPtr(), callOp->getCaptureFunc(),
			                       callOp->getFunctionAttributes(), flags, callOp->getDestructors());
			break;
		}
		case WireOpcode::IndirectCall: {
			const auto* callOp = cast<IndirectCallOperation>(&op);
			const uint8_t flags = callOp->requiresExceptionHandling() ? CALL_EXCEPTION_HANDLING : 0;
			record.payload1 =
			    call(scope, function, std::string {}, std::string {}, INVALID_FUNCTION_ID, nullptr,
			         callOp->getCaptureFunc(), callOp->getFunctionAttributes(), flags, callOp->getDestructors());
			break;
		}
		case WireOpcode::FunctionAddressOf: {
			auto* addrOp = cast<FunctionAddressOfOperation>(&mutableOp);
			record.payload0 = addrOp->getCalleeId();
			record.payload1 = call(scope, function, addrOp->getFunctionSymbol(), addrOp->getFunctionName(),
			                       addrOp->getCalleeId(), addrOp->getFunctionPtr(), nullptr, FunctionAttributes {}, 0,
			                       std::vector<CallOperation::Destructor> {});
			break;
		}
		case WireOpcode::Branch:
			record.payload0 = edge(scope, cast<BranchOperation>(&op)->getNextBlockInvocation(), function);
			break;
		case WireOpcode::If: {
			auto* ifOp = cast<IfOperation>(&mutableOp);
			record.payload0 = edge(scope, ifOp->getTrueBlockInvocation(), function);
			edge(scope, ifOp->getFalseBlockInvocation(), function);
			record.payload1 = blockRef(scope, ifOp->getMergeBlock());
			record.payload2 = constant(std::bit_cast<uint64_t>(ifOp->getProbability()));
			break;
		}
		default:
			break;
		}
		return record;
	}

	void writeFunction(const FunctionOperation& function) {
		FunctionRecord record {};
		record.name = str(function.getName());
		record.outputType = static_cast<uint8_t>(function.getOutputArg());
		record.location = location(function.getLocation());

		auto scope = collectBlocks(function);
		record.blocksBegin = checkedU32(blocks.size(), "block count");
		record.bodyBlockCount = checkedU32(function.getBasicBlocks().size(), "block count");
		record.extraBlockCount = checkedU32(scope.blocks.size() - function.getBasicBlocks().size(), "block count");

		// Number every value before emitting any operation: an operand may name
		// a value of a block that comes later in list order.
		uint32_t nextValue = 0;
		for (const auto* block : scope.blocks) {
			BlockRecord blockRecord {};
			blockRecord.identifier = block->getIdentifier().getId();
			blockRecord.region = block->getRegionIndex();
			blockRecord.firstValue = nextValue;
			blockRecord.argumentCount = checkedU32(block->getArguments().size(), "block argument count");
			blockRecord.operationCount = checkedU32(block->getOperations().size(), "block operation count");
			blocks.push_back(blockRecord);
			for (const auto* argument : block->getArguments()) {
				scope.valueIndex.emplace(argument, nextValue++);
			}
			for (const auto* op : block->getOperations()) {
				if (!scope.valueIndex.emplace(op, nextValue++).second) {
					throw SerializationException(fmt::format("function '{}' holds operation ${} twice",
					                                         function.getName(), op->getIdentifier().getId()));
				}
			}
			checkedU32(nextValue, "value count");
		}

		record.opsBegin = checkedU32(ops.size(), "operation count");
		record.opCount = nextValue;
		for (const auto* block : scope.blocks) {
			for (const auto* argument : block->getArguments()) {
				ops.push_back(operation(scope, function, *argument));
			}
			for (const auto* op : block->getOperations()) {
				ops.push_back(operation(scope, function, *op));
			}
		}

		record.inputTypesBegin = checkedU32(types.size(), "type pool size");
		for (const auto inputType : function.getInputArgs()) {
			type(inputType);
		}
		record.inputTypeCount = checkedU32(function.getInputArgs().size(), "argument count");

		record.argNamesBegin = checkedU32(stringRefs.size(), "string reference count");
		for (const auto& argName : function.getInputArgNames()) {
			stringRefs.push_back(str(argName));
		}
		record.argNameCount = checkedU32(function.getInputArgNames().size(), "argument count");

		record.allocasBegin = checkedU32(allocas.size(), "alloca count");
		for (const auto& spec : function.getAllocaSpecs()) {
			allocas.push_back(AllocaRecord {spec.size, spec.align});
		}
		record.allocaCount = checkedU32(function.getAllocaSpecs().size(), "alloca count");

		// Sorted, so an unordered_map does not make the output nondeterministic.
		std::vector<std::pair<std::string, std::string>> sortedAttributes(function.getAttributes().begin(),
		                                                                  function.getAttributes().end());
		std::sort(sortedAttributes.begin(), sortedAttributes.end());
		record.attributesBegin = checkedU32(attributes.size(), "attribute count");
		for (const auto& [key, attributeValue] : sortedAttributes) {
			attributes.push_back(AttributeRecord {str(key), str(attributeValue)});
		}
		record.attributeCount = checkedU32(sortedAttributes.size(), "attribute count");

		record.regionsBegin = checkedU32(regions.size(), "region count");
		for (const auto& spec : function.getRegionSpecs()) {
			RegionRecord region {};
			region.name = cstr(spec.attributes.name);
			region.location = location(spec.attributes.location);
			region.parent = spec.parent;
			region.id = spec.id;
			regions.push_back(region);
		}
		record.regionCount = checkedU32(function.getRegionSpecs().size(), "region count");

		record.padsBegin = checkedU32(indices.size(), "index pool size");
		record.callSitesBegin = checkedU32(callSites.size(), "call site count");
		if (function.exceptionRegion) {
			record.hasExceptionRegion = 1;
			for (const auto& pad : function.exceptionRegion->pads) {
				indices.push_back(blockRef(scope, pad.block));
			}
			for (const auto& site : function.exceptionRegion->callSites) {
				callSites.push_back(CallSiteRecord {
				    value(scope, site.call, function),
				    site.hasPad() ? checkedU32(site.padIndex, "landing pad index") : NONE,
				});
			}
			record.padCount = checkedU32(function.exceptionRegion->pads.size(), "landing pad count");
			record.callSiteCount = checkedU32(function.exceptionRegion->callSites.size(), "call site count");
		}
		functions.push_back(record);
	}

	// ── layout ──────────────────────────────────────────────────────────────

	struct PendingSection {
		uint32_t tag;
		uint32_t stride;
		uint32_t count;
		const void* data;
	};

	template <typename T>
	void section(std::vector<PendingSection>& out, uint32_t tag, const std::vector<T>& records) {
		out.push_back(
		    {tag, static_cast<uint32_t>(sizeof(T)), checkedU32(records.size(), "section size"), records.data()});
	}

	std::vector<std::byte> assemble() {
		std::vector<ModuleRecord> moduleRecords {module};
		std::vector<PendingSection> pending;
		pending.push_back({TAG_STRINGS, 1, checkedU32(strings.size(), "string pool size"), strings.data()});
		section(pending, TAG_MODULE, moduleRecords);
		section(pending, TAG_POINTERS, pointers);
		section(pending, TAG_TARGETS, targets);
		section(pending, TAG_TYPES, types);
		section(pending, TAG_FUNCTIONS, functions);
		section(pending, TAG_BLOCKS, blocks);
		section(pending, TAG_OPERATIONS, ops);
		section(pending, TAG_OPERANDS, operands);
		section(pending, TAG_EDGES, edges);
		section(pending, TAG_CONSTANTS, constants);
		section(pending, TAG_CALLS, calls);
		section(pending, TAG_DESTRUCTORS, destructors);
		section(pending, TAG_ALLOCAS, allocas);
		section(pending, TAG_ATTRIBUTES, attributes);
		section(pending, TAG_REGIONS, regions);
		section(pending, TAG_STRING_REFS, stringRefs);
		section(pending, TAG_INDICES, indices);
		section(pending, TAG_CALL_SITES, callSites);

		const auto align = [](uint64_t offset) {
			return (offset + SECTION_ALIGNMENT - 1) / SECTION_ALIGNMENT * SECTION_ALIGNMENT;
		};
		const uint64_t dirOffset = sizeof(FileHeader);
		uint64_t cursor = align(dirOffset + pending.size() * sizeof(SectionEntry));
		std::vector<SectionEntry> directory;
		for (const auto& p : pending) {
			directory.push_back(SectionEntry {p.tag, SECTION_REQUIRED, cursor, p.stride, p.count});
			cursor = align(cursor + static_cast<uint64_t>(p.stride) * p.count);
		}

		std::vector<std::byte> out(cursor, std::byte {0});
		std::memcpy(out.data() + dirOffset, directory.data(), directory.size() * sizeof(SectionEntry));
		for (size_t i = 0; i < pending.size(); ++i) {
			const auto bytes = static_cast<size_t>(pending[i].stride) * pending[i].count;
			if (bytes != 0) {
				std::memcpy(out.data() + directory[i].offset, pending[i].data, bytes);
			}
		}

		FileHeader header {};
		std::memcpy(header.magic, MAGIC, sizeof(MAGIC));
		header.versionMajor = VERSION_MAJOR;
		header.versionMinor = VERSION_MINOR;
		header.headerSize = sizeof(FileHeader);
		header.totalSize = out.size();
		header.flags = options.pointerMode == PointerMode::ProcessLocal ? FLAG_PROCESS_LOCAL : 0;
		header.processToken = options.pointerMode == PointerMode::ProcessLocal ? currentProcessToken() : 0;
		header.sectionCount = checkedU32(directory.size(), "section count");
		header.sectionDirOffset = dirOffset;
		header.checksum = crc32c(std::span<const std::byte>(out).subspan(sizeof(FileHeader)));
		std::memcpy(out.data(), &header, sizeof(FileHeader));
		return out;
	}

	const IRGraph& graph;
	const SerializeOptions& options;
	std::unordered_map<const FunctionOperation*, uint32_t> functionIndex;

	std::string strings;
	std::unordered_map<std::string, uint32_t> stringOffsets;
	std::unordered_map<const void*, uint32_t> pointerIndex;

	ModuleRecord module {};
	std::vector<PointerRecord> pointers;
	std::vector<TargetRecord> targets;
	std::vector<uint8_t> types;
	std::vector<FunctionRecord> functions;
	std::vector<BlockRecord> blocks;
	std::vector<OpRecord> ops;
	std::vector<uint32_t> operands;
	std::vector<EdgeRecord> edges;
	std::vector<uint64_t> constants;
	std::vector<CallRecord> calls;
	std::vector<DestructorRecord> destructors;
	std::vector<AllocaRecord> allocas;
	std::vector<AttributeRecord> attributes;
	std::vector<RegionRecord> regions;
	std::vector<StringRef> stringRefs;
	std::vector<uint32_t> indices;
	std::vector<CallSiteRecord> callSites;
};

} // namespace

std::vector<std::byte> serialize(const IRGraph& graph, const SerializeOptions& options) {
	return Writer(graph, options).write();
}

std::optional<uint64_t> peekTotalSize(std::span<const std::byte> prefix) {
	constexpr auto sizeEnd = offsetof(FileHeader, totalSize) + sizeof(FileHeader::totalSize);
	if (prefix.size() < sizeEnd || std::memcmp(prefix.data(), MAGIC, sizeof(MAGIC)) != 0) {
		return std::nullopt;
	}
	uint64_t total = 0;
	std::memcpy(&total, prefix.data() + offsetof(FileHeader, totalSize), sizeof(total));
	return total;
}

} // namespace nautilus::serialization
