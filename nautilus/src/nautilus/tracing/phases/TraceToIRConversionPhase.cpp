#include "nautilus/tracing/phases/TraceToIRConversionPhase.hpp"
#include "nautilus/common/Arena.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/IfOperation.hpp"
#include "nautilus/compiler/ir/operations/ReturnOperation.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/tracing/phases/IRConversionSupport.hpp"
#include <algorithm>
#include <deque>
#include <fmt/format.h>
#include <span>
#include <unordered_map>

namespace nautilus::tracing {

namespace {

class SSAConversionContext;

/// The current definition of each variable per block, keyed by
/// `(block << 32) | variable`. Open addressing with linear probing; key 0
/// marks a free slot (value ref 0 is never a variable). Entries are never
/// removed.
class DefinitionTable {
public:
	void reserve(size_t expected) {
		size_t capacity = 16;
		while (capacity < expected * 2) {
			capacity *= 2;
		}
		slots.assign(capacity, Slot {});
		size = 0;
	}

	[[nodiscard]] Operation* find(uint64_t key) const {
		for (size_t slot = home(key);; slot = (slot + 1) & mask()) {
			if (slots[slot].key == key) {
				return slots[slot].value;
			}
			if (slots[slot].key == 0) {
				return nullptr;
			}
		}
	}

	void set(uint64_t key, Operation* value) {
		if ((size + 1) * 2 > slots.size()) {
			grow();
		}
		size_t slot = home(key);
		while (slots[slot].key != 0 && slots[slot].key != key) {
			slot = (slot + 1) & mask();
		}
		if (slots[slot].key == 0) {
			slots[slot].key = key;
			++size;
		}
		slots[slot].value = value;
	}

private:
	struct Slot {
		uint64_t key = 0;
		Operation* value = nullptr;
	};

	[[nodiscard]] size_t mask() const {
		return slots.size() - 1;
	}

	[[nodiscard]] size_t home(uint64_t key) const {
		const auto hash = key * 0x9E3779B97F4A7C15ull;
		return static_cast<size_t>(hash ^ (hash >> 32)) & mask();
	}

	void grow() {
		std::vector<Slot> old(slots.size() * 2);
		old.swap(slots);
		size = 0;
		for (const auto& entry : old) {
			if (entry.key != 0) {
				set(entry.key, entry.value);
			}
		}
	}

	std::vector<Slot> slots;
	size_t size = 0;
};

/// The value frame of one block: reads and definitions go through the SSA
/// construction of the context.
struct SSAFrame {
	SSAConversionContext& context;
	uint32_t block;

	Operation* getValue(OperationIdentifier identifier);
	void setValue(OperationIdentifier identifier, Operation* value);
};

class SSAConversionContext : public IRConversionSupport {
public:
	SSAConversionContext(ExecutionTrace* trace, std::shared_ptr<IRGraph> ir)
	    : IRConversionSupport(trace, std::move(ir)), arena(this->ir->getArena()) {
	}

	FunctionOperation* processFunction(const std::string& functionName,
	                                   const std::unordered_map<std::string, std::string>& attributes,
	                                   const SourceLocation& location);

	Operation* read(uint32_t block, ValueRef ref);
	void define(uint32_t block, ValueRef ref, Operation* value);

private:
	/// A control-flow edge into a block: the invocation that carries the
	/// block's arguments, and the block it leaves from.
	struct Edge {
		BasicBlockInvocation* invocation;
		uint32_t predecessor;
	};

	/// A block argument standing for the value of a variable where control
	/// flow merges (a phi).
	struct Phi {
		BasicBlockArgument* argument;
		uint32_t block;
		uint32_t slot;
		ValueRef variable;
		/// Set once the argument turned out to be redundant: the value every
		/// edge passes for it.
		Operation* replacement = nullptr;
		/// Whether every edge passes a value for it yet.
		bool complete = false;
		/// Phis that take this one as an incoming value.
		std::vector<Phi*> users;
	};

	/// Every block argument this conversion creates -- function parameters,
	/// the merged return value, phis -- so a phi's bookkeeping is one load away
	/// from its argument. `phi` is null for the arguments that are not phis.
	struct Argument : BasicBlockArgument {
		Argument(OperationIdentifier identifier, Type stamp, Phi* phi)
		    : BasicBlockArgument(identifier, stamp), phi(phi) {
		}
		Phi* phi;
	};

	struct BlockState {
		BasicBlock* irBlock = nullptr;
		uint32_t predecessorCount = 0;
		uint32_t filledPredecessors = 0;
		bool visited = false;
		/// All predecessors are converted: no edge into the block is still missing.
		bool sealed = false;
		/// The block's incoming edges are `edges[firstEdge, firstEdge + edgeCount)`.
		uint32_t firstEdge = 0;
		uint32_t edgeCount = 0;
		/// Phis created while the block was not sealed yet; their incoming
		/// values are added when it is.
		std::vector<Phi*> incompletePhis;
	};

	void scanTrace();
	/// Where an edge to @p block really leads: past every block that holds
	/// nothing but a jump.
	uint32_t threadedTarget(uint32_t block);
	BasicBlock* irBlockOf(uint32_t block);
	void processBlock(uint32_t block);
	void processJump(uint32_t block, TraceOperation& operation);
	void processBranch(uint32_t block, TraceOperation& operation);
	void processReturn(uint32_t block, TraceOperation& operation);
	/// Records that @p block is converted, sealing the successors whose last
	/// predecessor it was.
	void addEdge(uint32_t from, uint32_t to, BasicBlockInvocation& invocation);
	void fillPredecessor(uint32_t successor);

	void writeVariable(ValueRef variable, uint32_t block, Operation* value);
	Operation* readVariable(ValueRef variable, uint32_t block);
	Operation* readVariableRecursive(ValueRef variable, uint32_t block);
	Phi* createPhi(uint32_t block, ValueRef variable);
	Operation* addPhiOperands(Phi* phi);
	Operation* tryRemoveTrivialPhi(Phi* phi);
	void sealBlock(uint32_t block);
	Phi* phiOf(Operation* value);
	Operation* resolve(Operation* value);
	/// Points every use of a removed phi at its replacement and drops the
	/// removed phis' argument slots.
	void removeTrivialPhis();

	std::span<Edge> incoming(uint32_t block) {
		const auto& state = blocks[block];
		return {edges.data() + state.firstEdge, state.edgeCount};
	}

	static uint64_t key(uint32_t block, ValueRef variable) {
		return (static_cast<uint64_t>(block) << 32) | variable;
	}

	common::Arena& arena;
	std::vector<BlockState> blocks;
	/// Per trace block: the target of its jump if the jump is all it holds,
	/// otherwise the block itself.
	std::vector<uint32_t> forwardTo;
	/// Every block's incoming edges, grouped by target block.
	std::vector<Edge> edges;
	std::vector<BasicBlock*> currentBasicBlocks;
	/// What the conversion knows about one value ref.
	struct ValueState {
		/// The definition of a value defined once; unused for variables.
		Operation* definition = nullptr;
		/// Whether some ASSIGN redefines the value.
		bool isVariable = false;
		/// The variable's type, for the phis it gets.
		Type type = Type::v;
	};
	std::vector<ValueState> valueStates;
	/// The current definition of a variable at the end of a block.
	DefinitionTable currentDefinitions;
	std::deque<Phi> phis;
	size_t removedPhis = 0;

	/// Multiple returns branch to one return block taking the result as its argument.
	bool mergeReturns = false;
	uint32_t returnBlockId = 0;
	BasicBlock* returnBlock = nullptr;
	const TraceOperation* firstReturn = nullptr;
	Type returnType = Type::v;
};

Operation* SSAFrame::getValue(OperationIdentifier identifier) {
	return context.read(block, identifier.getId());
}

void SSAFrame::setValue(OperationIdentifier identifier, Operation* value) {
	context.define(block, identifier.getId(), value);
}

void SSAConversionContext::scanTrace() {
	auto& traceBlocks = trace->getBlocks();
	const auto numberOfValueRefs = static_cast<size_t>(trace->lastValueRef) + 1;
	valueStates.assign(numberOfValueRefs, ValueState {});
	blocks.resize(traceBlocks.size() + 1);

	const auto returns = trace->getReturn();
	if (returns.size() > 1) {
		mergeReturns = true;
		returnBlockId = static_cast<uint32_t>(traceBlocks.size());
		firstReturn = trace->getBlock(returns.front().blockIndex).operations[returns.front().operationIndex];
	}

	auto checkBlock = [&](uint32_t id) {
		if (id >= traceBlocks.size()) {
			throw RuntimeException(
			    fmt::format("Invalid trace: block id {} is outside the {}-block trace.", id, traceBlocks.size()));
		}
	};
	auto checkRef = [&](ValueRef ref) {
		if (ref >= numberOfValueRefs) {
			throw RuntimeException(fmt::format("Invalid trace: value ref ${} exceeds the maximum value ref ${}.", ref,
			                                   trace->lastValueRef));
		}
	};
	for (const auto& argument : traceBlocks.front()->arguments) {
		checkRef(argument.ref);
	}

	size_t assignments = 0;
	// A trace block holding nothing but a jump -- what the tracer leaves where
	// control flow from several places meets a loop header -- is threaded:
	// edges into it lead straight to its target. Converting it would make it a
	// second loop header whose phis the blocks behind it read directly, so it
	// could not be cleaned up later, and LLVM leaves a loop with two headers
	// alone (binarySearch in tools/backend-explorer runs 3x slower).
	forwardTo.resize(traceBlocks.size());
	const auto entryId = traceBlocks.front()->blockId;
	for (const auto* block : traceBlocks) {
		checkBlock(block->blockId);
		const auto& operations = block->operations;
		const bool onlyJump = block->blockId != entryId && operations.size() == 1 && operations.front()->op == Op::JMP;
		forwardTo[block->blockId] =
		    onlyJump ? std::get<BlockRef*>(operations.front()->input[0])->block : block->blockId;
	}

	// Only blocks reachable from the entry are converted, so only their edges count.
	std::vector<uint32_t> worklist {traceBlocks.front()->blockId};
	checkBlock(worklist.back());
	std::vector<uint8_t> seen(traceBlocks.size(), 0);
	seen[worklist.back()] = 1;
	auto reach = [&](uint32_t rawTarget) {
		checkBlock(rawTarget);
		const auto target = threadedTarget(rawTarget);
		++blocks[target].predecessorCount;
		if (seen[target] == 0) {
			seen[target] = 1;
			worklist.push_back(target);
		}
	};
	while (!worklist.empty()) {
		auto& block = trace->getBlock(worklist.back());
		worklist.pop_back();
		for (const auto* operation : block.operations) {
			if (operation->resultRef.ref != 0) {
				checkRef(operation->resultRef.ref);
			}
			if (operation->op == Op::ASSIGN) {
				++assignments;
				valueStates[operation->resultRef.ref].isVariable = true;
				valueStates[operation->resultRef.ref].type = operation->resultType;
			}
		}
		if (block.operations.empty()) {
			continue;
		}
		const auto& terminator = *block.operations.back();
		if (terminator.op == Op::JMP) {
			reach(std::get<BlockRef*>(terminator.input[0])->block);
		} else if (terminator.op == Op::CMP) {
			reach(std::get<BlockRef*>(terminator.input[1])->block);
			reach(std::get<BlockRef*>(terminator.input[2])->block);
		}
	}
	uint32_t edgeTotal = 0;
	for (auto& state : blocks) {
		state.sealed = state.predecessorCount == 0;
		state.firstEdge = edgeTotal;
		edgeTotal += state.predecessorCount;
	}
	edges.resize(edgeTotal);
	currentDefinitions.reserve(assignments + blocks.size());
}

uint32_t SSAConversionContext::threadedTarget(uint32_t block) {
	// A cycle of jump-only blocks never reaches anything else: stop after as
	// many steps as there are blocks and keep the block reached.
	auto target = block;
	for (size_t steps = 0; forwardTo[target] != target && steps < forwardTo.size(); ++steps) {
		target = forwardTo[target];
		if (target >= forwardTo.size()) {
			throw RuntimeException(
			    fmt::format("Invalid trace: block id {} is outside the {}-block trace.", target, forwardTo.size()));
		}
	}
	return target;
}

BasicBlock* SSAConversionContext::irBlockOf(uint32_t block) {
	auto& state = blocks[block];
	if (state.irBlock == nullptr) {
		state.irBlock = arena.create<BasicBlock>(arena, block, std::vector<BasicBlockArgument*> {});
	}
	return state.irBlock;
}

FunctionOperation* SSAConversionContext::processFunction(const std::string& functionName,
                                                         const std::unordered_map<std::string, std::string>& attributes,
                                                         const SourceLocation& location) {
	scanTrace();

	// The entry block's arguments are the function's parameters, in value-ref order.
	auto& entry = *trace->getBlocks().front();
	auto parameters = entry.arguments;
	std::sort(parameters.begin(), parameters.end());
	std::vector<BasicBlockArgument*> entryArguments;
	entryArguments.reserve(parameters.size());
	for (const auto& parameter : parameters) {
		entryArguments.push_back(arena.create<Argument>(createValueIdentifier(parameter), parameter.type, nullptr));
	}
	blocks[entry.blockId].irBlock = arena.create<BasicBlock>(arena, entry.blockId, std::move(entryArguments));
	for (auto* argument : blocks[entry.blockId].irBlock->getArguments()) {
		define(entry.blockId, argument->getIdentifier().getId(), argument);
	}

	processBlock(entry.blockId);
	if (removedPhis != 0) {
		removeTrivialPhis();
	}

	return arena.create<FunctionOperation>(functionName, std::move(currentBasicBlocks), std::vector<Type> {},
	                                       std::vector<std::string> {}, returnType, collectAllocaSpecs(), attributes,
	                                       std::move(currentRegionSpecs), location);
}

void SSAConversionContext::processBlock(uint32_t blockId) {
	auto& traceBlock = trace->getBlock(blockId);
	auto* irBlock = irBlockOf(blockId);
	blocks[blockId].visited = true;
	irBlock->setRegionIndex(mapRegion(traceBlock.regionIndex));
	currentBasicBlocks.push_back(irBlock);

	SSAFrame frame {*this, blockId};
	for (auto* operation : traceBlock.operations) {
		switch (operation->op) {
		case Op::ASSIGN:
			writeVariable(operation->resultRef.ref, blockId,
			              read(blockId, std::get<TypedValueRef>(operation->input[0]).ref));
			break;
		case Op::JMP:
			processJump(blockId, *operation);
			break;
		case Op::CMP:
			processBranch(blockId, *operation);
			break;
		case Op::RETURN:
			processReturn(blockId, *operation);
			break;
		default:
			if (!convertValueOperation(frame, irBlock, *operation)) {
				throw NotImplementedException("Operation type is not implemented.");
			}
		}
	}
}

void SSAConversionContext::processJump(uint32_t block, TraceOperation& operation) {
	const auto target = threadedTarget(std::get<BlockRef*>(operation.input[0])->block);
	auto* branch = irBlockOf(block)->addNextBlock(irBlockOf(target), {});
	stampProvenance(*branch, operation);
	addEdge(block, target, static_cast<BranchOperation*>(branch)->getNextBlockInvocation());
	fillPredecessor(target);
	if (!blocks[target].visited) {
		processBlock(target);
	}
}

void SSAConversionContext::processBranch(uint32_t block, TraceOperation& operation) {
	auto* condition = read(block, std::get<TypedValueRef>(operation.input[0]).ref);
	const auto trueTarget = threadedTarget(std::get<BlockRef*>(operation.input[1])->block);
	const auto falseTarget = threadedTarget(std::get<BlockRef*>(operation.input[2])->block);
	auto probability = std::get<BranchProbability>(operation.input[3]);

	auto* ifOperation = arena.create<IfOperation>(arena, condition, probability);
	stampProvenance(*ifOperation, operation);
	ifOperation->getTrueBlockInvocation().setBlock(irBlockOf(trueTarget));
	ifOperation->getFalseBlockInvocation().setBlock(irBlockOf(falseTarget));
	irBlockOf(block)->addOperation(ifOperation);
	addEdge(block, trueTarget, ifOperation->getTrueBlockInvocation());
	addEdge(block, falseTarget, ifOperation->getFalseBlockInvocation());
	fillPredecessor(trueTarget);
	fillPredecessor(falseTarget);
	if (!blocks[trueTarget].visited) {
		processBlock(trueTarget);
	}
	if (!blocks[falseTarget].visited) {
		processBlock(falseTarget);
	}
}

void SSAConversionContext::processReturn(uint32_t block, TraceOperation& operation) {
	Operation* value =
	    operation.input.empty() ? nullptr : read(block, createValueIdentifier(operation.input[0]).getId());
	returnType = value == nullptr ? Type::v : value->getStamp();
	if (!mergeReturns) {
		if (value == nullptr) {
			irBlockOf(block)->addTaggedOperation<ReturnOperation>(provenanceOf(operation));
		} else {
			irBlockOf(block)->addTaggedOperation<ReturnOperation>(provenanceOf(operation), value);
		}
		return;
	}
	if (returnBlock == nullptr) {
		std::vector<BasicBlockArgument*> arguments;
		if (value != nullptr) {
			arguments.push_back(arena.create<Argument>(
			    createValueIdentifier(std::get<TypedValueRef>(firstReturn->input[0])), value->getStamp(), nullptr));
		}
		returnBlock = arena.create<BasicBlock>(arena, returnBlockId, std::move(arguments));
		currentBasicBlocks.push_back(returnBlock);
		if (value == nullptr) {
			returnBlock->addTaggedOperation<ReturnOperation>(provenanceOf(*firstReturn));
		} else {
			returnBlock->addTaggedOperation<ReturnOperation>(provenanceOf(*firstReturn),
			                                                 returnBlock->getArguments().front());
		}
	}
	if (value == nullptr) {
		irBlockOf(block)->addNextBlock(returnBlock, {});
	} else {
		Operation* const arguments[] = {value};
		irBlockOf(block)->addNextBlock(returnBlock, arguments);
	}
}

void SSAConversionContext::addEdge(uint32_t from, uint32_t to, BasicBlockInvocation& invocation) {
	auto& state = blocks[to];
	edges[state.firstEdge + state.edgeCount++] = {&invocation, from};
}

void SSAConversionContext::fillPredecessor(uint32_t successor) {
	auto& state = blocks[successor];
	if (++state.filledPredecessors == state.predecessorCount) {
		sealBlock(successor);
	}
}

Operation* SSAConversionContext::read(uint32_t block, ValueRef ref) {
	const auto& state = valueStates[ref];
	if (state.isVariable) {
		return readVariable(ref, block);
	}
	auto* value = state.definition;
	if (value == nullptr) {
		throw RuntimeException(fmt::format("Invalid trace: B{} uses ${} before its definition.", block, ref));
	}
	return value;
}

void SSAConversionContext::define(uint32_t block, ValueRef ref, Operation* value) {
	auto& state = valueStates[ref];
	if (state.isVariable) {
		writeVariable(ref, block, value);
	} else {
		state.definition = value;
	}
}

void SSAConversionContext::writeVariable(ValueRef variable, uint32_t block, Operation* value) {
	currentDefinitions.set(key(block, variable), value);
}

Operation* SSAConversionContext::readVariable(ValueRef variable, uint32_t block) {
	if (auto* value = currentDefinitions.find(key(block, variable))) {
		return resolve(value);
	}
	return readVariableRecursive(variable, block);
}

Operation* SSAConversionContext::readVariableRecursive(ValueRef variable, uint32_t block) {
	auto& state = blocks[block];
	Operation* value;
	if (!state.sealed) {
		// Not every edge into the block exists yet: the incoming values are
		// added once the last predecessor is converted.
		auto* phi = createPhi(block, variable);
		state.incompletePhis.push_back(phi);
		value = phi->argument;
	} else if (state.predecessorCount == 1) {
		value = readVariable(variable, edges[state.firstEdge].predecessor);
	} else if (state.predecessorCount == 0) {
		throw RuntimeException(fmt::format("Invalid trace: B{} uses ${} before its definition.", block, variable));
	} else {
		auto* phi = createPhi(block, variable);
		// Defined before its operands are read, so a cycle through the block finds it.
		writeVariable(variable, block, phi->argument);
		value = addPhiOperands(phi);
	}
	writeVariable(variable, block, value);
	return value;
}

SSAConversionContext::Phi* SSAConversionContext::createPhi(uint32_t block, ValueRef variable) {
	auto* irBlock = irBlockOf(block);
	auto& phi = phis.emplace_back(
	    Phi {nullptr, block, static_cast<uint32_t>(irBlock->getArguments().size()), variable, nullptr, false, {}});
	phi.argument = arena.create<Argument>(OperationIdentifier(variable), valueStates[variable].type, &phi);
	irBlock->addArgument(phi.argument);
	return &phi;
}

Operation* SSAConversionContext::addPhiOperands(Phi* phi) {
	for (const auto& edge : incoming(phi->block)) {
		auto* value = readVariable(phi->variable, edge.predecessor);
		edge.invocation->addArgument(arena, value);
		if (auto* operandPhi = phiOf(value)) {
			operandPhi->users.push_back(phi);
		}
	}
	phi->complete = true;
	return tryRemoveTrivialPhi(phi);
}

Operation* SSAConversionContext::tryRemoveTrivialPhi(Phi* phi) {
	Operation* same = nullptr;
	for (const auto& edge : incoming(phi->block)) {
		auto* value = resolve(edge.invocation->getArguments()[phi->slot]);
		if (value == same || value == phi->argument) {
			continue;
		}
		if (same != nullptr) {
			return phi->argument; // merges two different values
		}
		same = value;
	}
	if (same == nullptr) {
		return phi->argument; // only ever passed itself: unreachable, keep it
	}
	phi->replacement = same;
	++removedPhis;
	// Phis that took this one as an operand may have become trivial too.
	for (size_t i = 0; i < phi->users.size(); ++i) {
		auto* user = phi->users[i];
		if (user != phi && user->complete && user->replacement == nullptr) {
			tryRemoveTrivialPhi(user);
		}
	}
	return resolve(same);
}

void SSAConversionContext::sealBlock(uint32_t block) {
	auto& state = blocks[block];
	for (size_t i = 0; i < state.incompletePhis.size(); ++i) {
		addPhiOperands(state.incompletePhis[i]);
	}
	state.incompletePhis.clear();
	state.sealed = true;
}

SSAConversionContext::Phi* SSAConversionContext::phiOf(Operation* value) {
	// Every block argument of the function under construction is an `Argument`.
	if (value->getOperationType() != Operation::OperationType::BasicBlockArgument) {
		return nullptr;
	}
	return static_cast<Argument*>(value)->phi;
}

Operation* SSAConversionContext::resolve(Operation* value) {
	if (removedPhis == 0) {
		return value;
	}
	while (auto* phi = phiOf(value)) {
		if (phi->replacement == nullptr) {
			break;
		}
		value = phi->replacement;
	}
	return value;
}

void SSAConversionContext::removeTrivialPhis() {
	auto resolveInputs = [&](Operation& operation) {
		const auto inputs = operation.getInputs();
		for (size_t i = 0; i < inputs.size(); ++i) {
			if (auto* replacement = resolve(inputs[i]); replacement != inputs[i]) {
				operation.setInput(i, replacement);
			}
		}
	};
	for (auto* block : currentBasicBlocks) {
		for (auto* operation : block->getOperations()) {
			resolveInputs(*operation);
		}
		if (auto* terminator = block->getTerminatorOp()) {
			for (auto* invocation : compiler::ir::getSuccessorInvocations(*terminator)) {
				resolveInputs(*invocation);
			}
		}
	}

	std::unordered_map<uint32_t, std::vector<size_t>> removedSlots;
	for (const auto& phi : phis) {
		if (phi.replacement != nullptr) {
			removedSlots[phi.block].push_back(phi.slot);
		}
	}
	for (auto& [block, slots] : removedSlots) {
		std::sort(slots.begin(), slots.end());
		blocks[block].irBlock->removeArguments(slots);
		for (const auto& edge : incoming(block)) {
			edge.invocation->removeArguments(slots);
		}
	}
}

std::shared_ptr<IRGraph> convertModule(TraceModule& traceModule, std::shared_ptr<IRGraph> ir) {
	for (const auto& functionName : traceModule.getFunctionNames()) {
		auto* trace = traceModule.getFunction(functionName);
		SSAConversionContext context(trace, ir);
		auto* functionOperation = context.processFunction(functionName, traceModule.getFunctionAttributes(functionName),
		                                                  traceModule.getFunctionLocation(functionName));
		ir->addFunctionOperation(functionOperation);
		bindDefinition(*ir, traceModule, functionName, functionOperation);
	}
	return ir;
}

/// A lone trace becomes a module of one entry function, "execute". Nothing
/// calls it, so it interns with a null identity key and gets a fresh entry.
std::shared_ptr<IRGraph> convertTrace(ExecutionTrace& trace, std::shared_ptr<IRGraph> ir) {
	SSAConversionContext context(&trace, ir);
	auto* functionOperation = context.processFunction("execute", {{"entry", "true"}}, {});
	ir->addFunctionOperation(functionOperation);
	compiler::ir::CalleeDescriptor descriptor;
	descriptor.kind = compiler::ir::CalleeDescriptor::Kind::Internal;
	descriptor.customName = "execute";
	ir->defineFunction(ir->internCallee(descriptor), functionOperation);
	return ir;
}

} // namespace

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         const compiler::CompilationUnitID& id) {
	return convertModule(*traceModule, std::make_shared<IRGraph>(id));
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<TraceModule> traceModule,
                                                         common::ArenaPool& pool,
                                                         const compiler::CompilationUnitID& id) {
	return convertModule(*traceModule, std::make_shared<IRGraph>(pool.acquire(), id));
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<ExecutionTrace> trace,
                                                         const compiler::CompilationUnitID& id) {
	return convertTrace(*trace, std::make_shared<IRGraph>(id));
}

std::shared_ptr<IRGraph> TraceToIRConversionPhase::apply(std::shared_ptr<ExecutionTrace> trace, common::ArenaPool& pool,
                                                         const compiler::CompilationUnitID& id) {
	return convertTrace(*trace, std::make_shared<IRGraph>(pool.acquire(), id));
}

} // namespace nautilus::tracing
