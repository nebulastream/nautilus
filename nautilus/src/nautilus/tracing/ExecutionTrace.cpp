
#include "nautilus/tracing/ExecutionTrace.hpp"
#include "nautilus/tracing/symbolic_execution/TraceTerminationException.hpp"
#include <algorithm>
#include <bit>
#include <cstring>
#include <fmt/format.h>
#include <nautilus/config.hpp>
#include <nautilus/exceptions/RuntimeException.hpp>
#include <nautilus/logging.hpp>
#include <unordered_set>

namespace nautilus::tracing {

ExecutionTrace& TraceModule::addNewFunction(std::string_view functionName, Arena& arena) {
	auto key = std::string(functionName);
	auto& def = functions[key];
	def.name = key;
	def.trace = std::make_unique<ExecutionTrace>(arena);
	return *def.trace;
}

void TraceModule::setFunctionAttributes(const std::string& functionName,
                                        const std::unordered_map<std::string, std::string>& attrs) {
	auto it = functions.find(functionName);
	if (it != functions.end()) {
		it->second.attributes = attrs;
	}
}

TraceFunctionDefinition* TraceModule::getFunctionDefinition(const std::string& functionName) {
	auto it = functions.find(functionName);
	return it != functions.end() ? &it->second : nullptr;
}

const TraceFunctionDefinition* TraceModule::getFunctionDefinition(const std::string& functionName) const {
	auto it = functions.find(functionName);
	return it != functions.end() ? &it->second : nullptr;
}

ExecutionTrace* TraceModule::getFunction(const std::string& functionName) {
	auto it = functions.find(functionName);
	return it != functions.end() ? it->second.trace.get() : nullptr;
}

const ExecutionTrace* TraceModule::getFunction(const std::string& functionName) const {
	auto it = functions.find(functionName);
	return it != functions.end() ? it->second.trace.get() : nullptr;
}

const std::unordered_map<std::string, std::string>&
TraceModule::getFunctionAttributes(const std::string& functionName) const {
	static const std::unordered_map<std::string, std::string> empty;
	auto it = functions.find(functionName);
	return it != functions.end() ? it->second.attributes : empty;
}

std::string TraceModule::toString() const {
	auto sortedNames = getFunctionNames();
	std::string result;
	for (const auto& name : sortedNames) {
		std::string functionName = name;
		std::transform(functionName.begin(), functionName.end(), functionName.begin(),
		               [](unsigned char c) { return std::toupper(c); });
		result += functionName + ":\n";
		result += functions.at(name).trace->toString();
		result += "\n";
	}
	return result;
}

bool TraceModule::hasFunction(const std::string& functionName) const {
	return functions.find(functionName) != functions.end();
}

std::vector<std::string> TraceModule::getFunctionNames() const {
	std::vector<std::string> names;
	names.reserve(functions.size());
	for (const auto& [name, _] : functions) {
		names.push_back(name);
	}
	std::sort(names.begin(), names.end());
	return names;
}

namespace {

// Mixing primes used by computeContentHash.  Distinct from the AliveVariableHash
// multiplier so identical bits in different roles do not collide.
constexpr uint64_t CONTENT_OP_MIX = 0x100000001b3ULL;
constexpr uint64_t CONTENT_TYPE_MIX = 0xbf58476d1ce4e5b9ULL;
constexpr uint64_t CONTENT_INPUT_MIX = 0x94d049bb133111ebULL;
constexpr uint64_t CONTENT_CONST_BASE = 0xc6a4a7935bd1e995ULL;
constexpr uint64_t CONTENT_BLOCKREF_MIX = 0x9e3779b97f4a7c15ULL;
constexpr uint64_t CONTENT_CALL_MIX = 0xff51afd7ed558ccdULL;
constexpr uint64_t CONTENT_INDCALL_MIX = 0xd6e8feb86659fd93ULL;
constexpr uint64_t CONTENT_ARGSEED = 0xa5a5a5a5a5a5a5a5ULL;

inline uint64_t mixBits(uint64_t h, uint64_t v) noexcept {
	h ^= v;
	h *= CONTENT_INPUT_MIX;
	h ^= h >> 33;
	return h;
}

inline uint64_t hashConstantLiteral(const ConstantLiteral& lit) noexcept {
	// Mix the alternative index with the raw literal bits.  Two CONSTs with
	// the same literal value share a content hash even if they were minted
	// from different source positions, which is exactly the structural
	// equivalence the snapshot wants.
	uint64_t h = CONTENT_CONST_BASE * (static_cast<uint64_t>(lit.index()) + 1);
	std::visit(
	    [&h](auto&& v) noexcept {
		    using V = std::decay_t<decltype(v)>;
		    if constexpr (std::is_same_v<V, void*>) {
			    h = mixBits(h, std::bit_cast<uint64_t>(v));
		    } else {
			    uint64_t bits = 0;
			    std::memcpy(&bits, &v, sizeof(v));
			    h = mixBits(h, bits);
		    }
	    },
	    lit);
	return h;
}

} // namespace

uint64_t ExecutionTrace::computeContentHash(Op op, Type resultType,
                                            std::span<const InputVariant> inputs) const noexcept {
	uint64_t h = CONTENT_OP_MIX * (static_cast<uint64_t>(op) + 1);
	h = mixBits(h, CONTENT_TYPE_MIX * (static_cast<uint64_t>(resultType) + 1));
	for (const auto& input : inputs) {
		uint64_t inputBits = std::visit(
		    [this](auto&& v) noexcept -> uint64_t {
			    using V = std::decay_t<decltype(v)>;
			    if constexpr (std::is_same_v<V, TypedValueRef>) {
				    return getContentHashForValueRef(v.ref);
			    } else if constexpr (std::is_same_v<V, ConstantLiteral>) {
				    return hashConstantLiteral(v);
			    } else if constexpr (std::is_same_v<V, BlockRef*>) {
				    return v == nullptr ? 0 : CONTENT_BLOCKREF_MIX * (static_cast<uint64_t>(v->block) + 1);
			    } else if constexpr (std::is_same_v<V, FunctionCall*>) {
				    if (v == nullptr) {
					    return 0;
				    }
				    uint64_t fh = CONTENT_CALL_MIX;
				    fh = mixBits(fh, std::hash<std::string> {}(v->functionName));
				    for (const auto& arg : v->arguments) {
					    fh = mixBits(fh, getContentHashForValueRef(arg.ref));
				    }
				    return fh;
			    } else if constexpr (std::is_same_v<V, IndirectFunctionCall*>) {
				    if (v == nullptr) {
					    return 0;
				    }
				    uint64_t fh = CONTENT_INDCALL_MIX;
				    fh = mixBits(fh, getContentHashForValueRef(v->fnPtr.ref));
				    for (const auto& arg : v->arguments) {
					    fh = mixBits(fh, getContentHashForValueRef(arg.ref));
				    }
				    return fh;
			    } else if constexpr (std::is_same_v<V, BranchProbability>) {
				    uint64_t bits;
				    std::memcpy(&bits, &v, sizeof(v));
				    return bits;
			    } else if constexpr (std::is_same_v<V, AllocaIndex>) {
				    // Each ALLOCA must produce a distinct value-content even though every
				    // alloca op shares the same op code; mixing the table index ensures
				    // alloca-derived pointer ValueRefs do not all collapse into one
				    // content class.
				    return CONTENT_CALL_MIX * (static_cast<uint64_t>(v) + 1);
			    } else {
				    return 0; // None
			    }
		    },
		    input);
		h = mixBits(h, inputBits);
	}
	return h;
}

void ExecutionTrace::storeContentHashForValueRef(ValueRef ref, uint64_t contentHash) {
	if (ref >= valueRefContentHashes.size()) {
		valueRefContentHashes.resize(ref + 1, 0);
	}
	valueRefContentHashes[ref] = contentHash;
}

ExecutionTrace::ExecutionTrace(Arena& arena) : arena(&arena), currentBlockIndex(0), currentOperationIndex(0), blocks() {
	// A typical short trace has only a handful of blocks; reserving a small
	// initial capacity eliminates the first few reallocations of the pointer
	// vector in the tracing hot path.
	blocks.reserve(8);
	createBlock();
}

ExecutionTrace::~ExecutionTrace() {
	// Blocks and TraceOperations are arena-allocated via Arena::create
	// (which registers their destructors with the arena).  The arena runs
	// those destructors when it is softReset() or destroyed, so there is
	// nothing to do here.  Doing it explicitly would double-destruct any
	// operations that the SSA phase removed from a block (e.g. ASSIGN ops
	// erased from block.operations whose objects still live in the arena).
}

Block& ExecutionTrace::getBlock(uint32_t blockIndex) {
	if (blockIndex >= blocks.size()) {
		throw RuntimeException("Block index out of bounds: " + std::to_string(blockIndex));
	}
	return *blocks[blockIndex];
}

uint32_t ExecutionTrace::getCurrentBlockIndex() const {
	return currentBlockIndex;
}

Block& ExecutionTrace::getCurrentBlock() {
	if (currentBlockIndex >= blocks.size()) {
		throw RuntimeException("Current block index out of bounds: " + std::to_string(currentBlockIndex));
	}
	return *blocks[currentBlockIndex];
}

void ExecutionTrace::setCurrentBlock(uint32_t index) {
	if (index >= blocks.size()) {
		throw RuntimeException("Cannot set current block to out of bounds index: " + std::to_string(index));
	}
	currentOperationIndex = 0;
	currentBlockIndex = index;
}

std::vector<Block*>& ExecutionTrace::getBlocks() {
	return blocks;
}

Arena& ExecutionTrace::getArena() {
	return *arena;
}

bool ExecutionTrace::checkTag(Snapshot& snapshot) {
	// check if operation is in global map -> we have a repeating operation ->
	// this is a control-flow merge
	auto globalTabIter = globalTagMap.find(snapshot);
	if (globalTabIter != globalTagMap.end()) {
		auto& ref = globalTabIter->second;
		processControlFlowMerge(ref);
		return false;
	}

	// check if we visited the same operation in this execution -> loop
	auto localTagIter = localTagMap.find(snapshot);
	if (localTagIter != localTagMap.end()) {
		auto& ref = localTagIter->second;
		processControlFlowMerge(ref);
		return false;
	}
	return true;
}

void ExecutionTrace::addReturn(Snapshot& snapshot, Type resultType, const TypedValueRef& ref) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex]->operations;
	auto op = Op::RETURN;
	TraceOperation* newOp;
	if (ref.type == Type::v) {
		newOp = makeTraceOp(*arena, snapshot, op, resultType, TypedValueRef(0, Type::v));
	} else {
		newOp = makeTraceOp(*arena, snapshot, op, resultType, TypedValueRef(0, Type::v), ref);
	}
	operations.push_back(newOp);
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);

	returnRefs.emplace_back(operationIdentifier);
}

TypedValueRef& ExecutionTrace::addAssignmentOperation(Snapshot& snapshot, const TypedValueRef& targetRef,
                                                      const TypedValueRef& srcRef, Type resultType) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex]->operations;
	auto op = ASSIGN;
	auto* operation = makeTraceOp(*arena, snapshot, op, resultType, targetRef, srcRef);
	// An assignment forwards its source's value to the target; the target's
	// content-class is therefore the source's content hash, not a new mixture.
	// We deliberately do not overwrite valueRefContentHashes[targetRef.ref]: the
	// alive hash for an already-allocated target must stay stable for the
	// XOR-rollover scheme to remain self-cancelling on free.  The pass-through
	// here only affects fresh result refs (e.g. traceCopy, the const-aliasing
	// path in traceConstant) where targetRef was minted by getNextValueRef().
	operation->contentHash = getContentHashForValueRef(srcRef.ref);
	if (targetRef.ref >= valueRefContentHashes.size() || valueRefContentHashes[targetRef.ref] == 0) {
		storeContentHashForValueRef(targetRef.ref, operation->contentHash);
	}
	operations.push_back(operation);
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return operation->resultRef;
}

void ExecutionTrace::addOperation(Snapshot& snapshot, Op& operation, std::initializer_list<InputVariant> inputs) {
	if (blocks.empty()) {
		createBlock();
	}
	auto& operations = blocks[currentBlockIndex]->operations;
	auto* newOp = makeTraceOp(*arena, snapshot, operation, Type::v, TypedValueRef(0, Type::v), inputs);
	operations.push_back(newOp);
}

TypedValueRef& ExecutionTrace::addOperationWithResult(Snapshot& snapshot, Op& operation, Type& resultType,
                                                      std::initializer_list<InputVariant> inputs) {
	if (blocks.empty()) {
		createBlock();
	}

	auto& operations = blocks[currentBlockIndex]->operations;
	auto* to =
	    makeTraceOp(*arena, snapshot, operation, resultType, TypedValueRef(getNextValueRef(), resultType), inputs);
	to->contentHash = computeContentHash(operation, resultType, to->input);
	storeContentHashForValueRef(to->resultRef.ref, to->contentHash);
	operations.push_back(to);

	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
	return to->resultRef;
}

// Adds a comparison operation to the execution trace
// This consists of a snapshot, the comparison input, two blocks for true and false branches, and the branch probability
void ExecutionTrace::addCmpOperation(Snapshot& snapshot, const TypedValueRef& condition, const double probability) {
	if (blocks.empty()) {
		createBlock();
	}

	// create if and else blocks
	auto trueBlock = createBlock();
	getBlock(trueBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto falseBlock = createBlock();
	getBlock(falseBlock).predecessors.emplace_back(getCurrentBlockIndex());
	auto& operations = blocks[currentBlockIndex]->operations;
	auto* trueBlockRef = arena->create<BlockRef>(trueBlock);
	auto* falseBlockRef = arena->create<BlockRef>(falseBlock);
	auto* cmpOp = makeTraceOp(*arena, snapshot, CMP, Type::v, TypedValueRef(getNextValueRef(), Type::v), condition,
	                          trueBlockRef, falseBlockRef, probability);
	operations.push_back(cmpOp);
	auto operationIdentifier = getNextOperationIdentifier();
	addTag(snapshot, operationIdentifier);
}

void ExecutionTrace::nextOperation() {
	this->currentOperationIndex++;
	auto& block = getCurrentBlock();
	if (currentOperationIndex >= block.operations.size()) {
		throw RuntimeException("Operation index out of bounds: " + std::to_string(currentOperationIndex));
	}
	auto* currentOp = block.operations[currentOperationIndex];
	if (currentOp->op == JMP) {
		auto* nextBlock = std::get<BlockRef*>(currentOp->input[0]);
		setCurrentBlock(nextBlock->block);
	}
}

TraceOperation& ExecutionTrace::getCurrentOperation() {
	if (currentOperationIndex >= getCurrentBlock().operations.size()) {
		throw RuntimeException("Current operation index out of bounds: " + std::to_string(currentOperationIndex));
	}
	while (getCurrentBlock().operations[currentOperationIndex]->op == JMP) {
		auto* nextBlock = std::get<BlockRef*>(getCurrentBlock().operations[currentOperationIndex]->input[0]);
		setCurrentBlock(nextBlock->block);
		if (currentOperationIndex >= getCurrentBlock().operations.size()) {
			throw RuntimeException("Current operation index out of bounds after JMP: " +
			                       std::to_string(currentOperationIndex));
		}
	}
	return *getCurrentBlock().operations[currentOperationIndex];
}

uint32_t ExecutionTrace::createBlock() {
	auto blockId = static_cast<uint32_t>(blocks.size());
	auto* block = arena->create<Block>(blockId);
	blocks.push_back(block);
	return block->blockId;
}

Block& ExecutionTrace::processControlFlowMerge(operation_identifier oi) {
	if (oi.blockIndex == currentBlockIndex) {
		throw RuntimeException("Invalid trace. This is maybe caused by a constant loop.");
	}

	// create new merge block
	auto mergedBlockId = createBlock();
	// perform a control flow merge and merge the current block with operations in
	// some other block.
	auto& referenceBlock = *blocks[oi.blockIndex];
	auto& currentBlock = *blocks[currentBlockIndex];

	auto& mergeBlock = getBlock(mergedBlockId);
	mergeBlock.type = Block::Type::ControlFlowMerge;

	// 1. move operation pointers to new block
	// Pointers are stable (arena-allocated), so we only move the entries in
	// the operations vector; the TraceOperation objects themselves stay put.
	for (uint32_t opIndex = oi.operationIndex; opIndex < referenceBlock.operations.size(); opIndex++) {
		auto* sourceOperation = referenceBlock.operations[opIndex];
		auto opType = sourceOperation->op;
		auto opTag = sourceOperation->tag;
		auto operationReference = mergeBlock.addOperation(sourceOperation);
		// update in global and local tag map

		if (opType == RETURN) {
			for (auto& returnRef : returnRefs) {
				if (returnRef.blockIndex == referenceBlock.blockId && returnRef.operationIndex == opIndex) {
					returnRef = operationReference;
				}
			}
		} else {
			globalTagMap[opTag] = operationReference;
			localTagMap[opTag] = operationReference;
		}
	}

	// remove content beyond opID
	referenceBlock.operations.erase(referenceBlock.operations.begin() + oi.operationIndex,
	                                referenceBlock.operations.end());

	// add jump from referenced block to merge block.  Each JMP gets its own
	// arena-allocated BlockRef so that later phases (e.g. SSA construction)
	// can mutate the arguments lists independently.
	referenceBlock.addOperation(makeTraceOp(*arena, Op::JMP, arena->create<BlockRef>(mergedBlockId)));

	// add jump from current block to merge block
	currentBlock.addOperation(makeTraceOp(*arena, Op::JMP, arena->create<BlockRef>(mergedBlockId)));

	mergeBlock.predecessors.emplace_back(oi.blockIndex);
	mergeBlock.predecessors.emplace_back(currentBlockIndex);
	setCurrentBlock(mergedBlockId);

	// update predecessors of merge merge block
	auto* lastMergeOperation = mergeBlock.operations[mergeBlock.operations.size() - 1];
	if (lastMergeOperation->op == Op::CMP || lastMergeOperation->op == Op::JMP) {
		for (auto& input : lastMergeOperation->input) {
			if (auto* blockRefPtr = std::get_if<BlockRef*>(&input)) {
				auto& blockPredecessor = getBlock((*blockRefPtr)->block).predecessors;
				std::replace(blockPredecessor.begin(), blockPredecessor.end(), oi.blockIndex, mergedBlockId);
				std::replace(blockPredecessor.begin(), blockPredecessor.end(), currentBlockIndex, mergedBlockId);
			}
		}
	}
	return mergeBlock;
}

void ExecutionTrace::bridgeMergeBlockNonLocals(Block& mergeBlock, Block& currentBlock) {
	// Locals defined inside mergeBlock itself are already SSA-bridgeable.
	std::unordered_set<ValueRef> definedInMerge;
	for (auto* op : mergeBlock.operations) {
		if (op->resultRef.ref != 0) {
			definedInMerge.insert(op->resultRef.ref);
		}
	}

	// Refs already defined locally in currentBlock — either by a path-Y op
	// that minted them, or by an earlier ASSIGN bridge added by this routine
	// or by traceConstant's aliasing path.
	std::unordered_set<ValueRef> definedInCurrent;
	for (auto* op : currentBlock.operations) {
		if (op->resultRef.ref != 0) {
			definedInCurrent.insert(op->resultRef.ref);
		}
	}

	// Walk mergeBlock collecting non-local ValueRef uses.  We dedupe via
	// `bridged` so a given (refToBridge, bridgeSource) only gets one ASSIGN
	// even if the merge block references the same ref many times.
	std::unordered_set<ValueRef> bridged;
	for (auto* op : mergeBlock.operations) {
		for (auto& input : op->input) {
			auto* v = std::get_if<TypedValueRef>(&input);
			if (v == nullptr || v->ref == 0) {
				continue;
			}
			ValueRef refUsed = v->ref;
			if (definedInMerge.contains(refUsed) || definedInCurrent.contains(refUsed) || bridged.contains(refUsed)) {
				continue;
			}
			uint64_t targetHash = getContentHashForValueRef(refUsed);
			if (targetHash == 0) {
				// No content fingerprint recorded for this ref (transient
				// sentinel or a ref minted before contentHash was wired up).
				// Cannot pick a safe bridge candidate.
				continue;
			}
			// Find a currentBlock-local op with the matching content hash.
			TraceOperation* match = nullptr;
			for (auto* candidate : currentBlock.operations) {
				if (candidate->resultRef.ref == 0 || candidate->contentHash != targetHash) {
					continue;
				}
				if (candidate->resultRef.ref == refUsed) {
					// Already locally defined under the canonical ref — no
					// bridge needed.
					definedInCurrent.insert(refUsed);
					break;
				}
				match = candidate;
				break;
			}
			if (match == nullptr || definedInCurrent.contains(refUsed)) {
				continue;
			}
			// Emit ASSIGN <refUsed> <match-ref> using a synthetic snapshot
			// (no tag-map registration needed; bridges are intentionally
			// outside the dedup machinery).
			Snapshot bridgeTag {};
			Type bridgeType = match->resultType;
			auto* bridgeOp = makeTraceOp(*arena, bridgeTag, Op::ASSIGN, bridgeType, TypedValueRef(refUsed, bridgeType),
			                             TypedValueRef(match->resultRef.ref, bridgeType));
			// Re-derive contentHash so downstream users of this bridge value
			// continue to see the same hash class.
			bridgeOp->contentHash = targetHash;
			// We add to block.operations directly: the public add* helpers
			// would also push the synthetic snapshot into the global / local
			// tag maps, which we deliberately avoid (the tag has no source-
			// level meaning and would confuse subsequent checkTag matches).
			currentBlock.operations.push_back(bridgeOp);
			definedInCurrent.insert(refUsed);
			bridged.insert(refUsed);
		}
	}
}

TypedValueRef& ExecutionTrace::setArgument(Type type, size_t index) {
	++lastValueRef;
	ValueRef argRef = index + 1;
	auto& arguments = blocks[0]->arguments;
	if (arguments.size() < argRef) {
		arguments.resize(argRef);
	}
	arguments[index] = TypedValueRef(argRef, type);
	// Seed the argument's content hash deterministically from (index, type).
	// Re-tracing the same function across iterations therefore produces the
	// same argument content hash, which is the precondition that lets the
	// alive-hash mixing cancel cleanly on freeValRef.
	uint64_t argHash = CONTENT_ARGSEED ^ (static_cast<uint64_t>(index + 1) * CONTENT_OP_MIX);
	argHash = mixBits(argHash, CONTENT_TYPE_MIX * (static_cast<uint64_t>(type) + 1));
	storeContentHashForValueRef(argRef, argHash);
	return arguments[index];
}

std::vector<operation_identifier> ExecutionTrace::getReturn() {
	return returnRefs;
}

ValueRef ExecutionTrace::getNextValueRef() {
	return ++lastValueRef;
}

operation_identifier ExecutionTrace::getNextOperationIdentifier() {
	currentOperationIndex = getCurrentBlock().operations.size() - 1;
	return {currentBlockIndex, currentOperationIndex};
}

void ExecutionTrace::resetExecution() {
	currentBlockIndex = 0;
	currentOperationIndex = 0;
	globalTagMap.merge(localTagMap);
	localTagMap.clear();
}

const std::vector<TypedValueRef>& ExecutionTrace::getArguments() {
	return blocks[0]->arguments;
}

void ExecutionTrace::addTag(Snapshot& snapshot, operation_identifier& identifier) {
	globalTagMap[snapshot] = identifier;
	localTagMap[snapshot] = identifier;
}

AllocaIndex ExecutionTrace::addAllocaSpec(size_t size, size_t align) {
	auto index = static_cast<AllocaIndex>(allocaSpecs.size());
	allocaSpecs.push_back({size, align});
	return index;
}

} // namespace nautilus::tracing

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
	for (size_t i = 0; i < trace.blocks.size(); i++) {
		fmt::format_to(out, "B{}{}", i, *trace.blocks[i]);
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
	for (const auto* operation : block.operations) {
		fmt::format_to(out, "{}\n", *operation);
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
		} else if (auto blockRefPtr = std::get_if<nautilus::tracing::BlockRef*>(&opInput)) {
			fmt::format_to(out, "{}\t", **blockRefPtr);
		} else if (auto fCallPtr = std::get_if<nautilus::tracing::FunctionCall*>(&opInput)) {
			fmt::format_to(out, "{}\t", **fCallPtr);
		} else if (auto constant = std::get_if<nautilus::ConstantLiteral>(&opInput)) {
			fmt::format_to(out, "{}", *constant);
		}
	}
	fmt::format_to(out, ":{}", toString(operation.resultType));
	return out;
}

} // namespace fmt

std::string nautilus::tracing::ExecutionTrace::toString() const {
	return fmt::to_string(*this);
}
