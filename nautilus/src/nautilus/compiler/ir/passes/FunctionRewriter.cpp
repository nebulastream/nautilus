#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/OperationProperties.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <algorithm>
#include <unordered_set>

namespace nautilus::compiler::ir {

namespace {

/// Every invocation on @p terminator's outgoing edges that targets @p block,
/// deduplicating repeat scans of the same predecessor's terminator (a
/// predecessor may appear more than once in @p block's predecessor list --
/// once per edge -- but its terminator only needs to be inspected once to
/// discover every one of those edges).
std::vector<BasicBlockInvocation*> invocationsTargeting(BasicBlock* block) {
	std::vector<BasicBlockInvocation*> result;
	std::unordered_set<BasicBlock*> visitedPreds;
	for (auto* pred : block->getPredecessors()) {
		if (pred == nullptr || !visitedPreds.insert(pred).second) {
			continue;
		}
		auto* terminator = pred->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			if (inv->getBlock() == block) {
				result.push_back(inv);
			}
		}
	}
	return result;
}

} // namespace

FunctionRewriter::FunctionRewriter(FunctionOperation& fn, common::Arena& arena) : fn_(fn), arena_(arena) {
	uint32_t maxId = 0;
	for (auto* block : fn_.getBasicBlocks()) {
		for (auto* arg : block->getArguments()) {
			defBlock_[arg] = block;
			maxId = std::max(maxId, arg->getIdentifier().getId());
		}
		for (auto* op : block->getOperations()) {
			defBlock_[op] = block;
			maxId = std::max(maxId, op->getIdentifier().getId());
			registerUses(op);
			for (auto* inv : getSuccessorInvocations(*op)) {
				registerUses(inv);
			}
		}
	}
	nextId_ = maxId + 1;
}

void FunctionRewriter::registerUses(Operation* user) {
	auto ins = user->getInputs();
	for (uint32_t i = 0; i < ins.size(); ++i) {
		if (ins[i] != nullptr) {
			uses_[ins[i]].push_back(Use {user, i});
		}
	}
}

void FunctionRewriter::unregisterUser(Operation* user) {
	auto ins = user->getInputs();
	for (uint32_t i = 0; i < ins.size(); ++i) {
		Operation* operand = ins[i];
		if (operand == nullptr) {
			continue;
		}
		auto it = uses_.find(operand);
		if (it == uses_.end()) {
			continue;
		}
		auto& vec = it->second;
		vec.erase(
		    std::remove_if(vec.begin(), vec.end(), [&](const Use& u) { return u.user == user && u.operandIndex == i; }),
		    vec.end());
		if (vec.empty()) {
			uses_.erase(it);
		}
	}
}

std::span<const FunctionRewriter::Use> FunctionRewriter::usesOf(const Operation* op) const {
	auto it = uses_.find(op);
	if (it == uses_.end()) {
		return {};
	}
	return std::span<const Use>(it->second);
}

size_t FunctionRewriter::useCount(const Operation* op) const {
	auto it = uses_.find(op);
	return it == uses_.end() ? 0 : it->second.size();
}

bool FunctionRewriter::hasUses(const Operation* op) const {
	return useCount(op) != 0;
}

BasicBlock* FunctionRewriter::definingBlock(const Operation* op) const {
	auto it = defBlock_.find(op);
	return it == defBlock_.end() ? nullptr : it->second;
}

void FunctionRewriter::replaceAllUses(Operation* from, Operation* to) {
	auto it = uses_.find(from);
	if (it == uses_.end()) {
		return;
	}
	std::vector<Use> moved = std::move(it->second);
	uses_.erase(it);
	for (auto& u : moved) {
		u.user->setInput(u.operandIndex, to);
	}
	if (to != nullptr) {
		auto& toUses = uses_[to];
		toUses.insert(toUses.end(), moved.begin(), moved.end());
	}
}

OperationIdentifier FunctionRewriter::freshId() {
	return OperationIdentifier {nextId_++};
}

void FunctionRewriter::erase(Operation* op) {
	if (isTerminatorOp(op->getOperationType())) {
		throw RuntimeException("FunctionRewriter::erase: cannot erase terminator operation " +
		                       op->getIdentifier().toString());
	}
	if (useCount(op) != 0) {
		throw RuntimeException("FunctionRewriter::erase: operation " + op->getIdentifier().toString() + " still has " +
		                       std::to_string(useCount(op)) + " use(s)");
	}
	auto it = defBlock_.find(op);
	if (it == defBlock_.end()) {
		throw RuntimeException("FunctionRewriter::erase: operation " + op->getIdentifier().toString() +
		                       " is not tracked by this rewriter session");
	}
	unregisterUser(op);
	it->second->removeOperation(op);
	defBlock_.erase(it);
	uses_.erase(op);
}

size_t FunctionRewriter::eraseIfDead(Operation* op) {
	size_t erasedCount = 0;
	std::vector<Operation*> worklist {op};
	while (!worklist.empty()) {
		Operation* cur = worklist.back();
		worklist.pop_back();
		if (cur == nullptr || !defBlock_.contains(cur)) {
			continue; // not tracked: already erased, or never part of the function.
		}
		if (useCount(cur) != 0 || !isPureOp(cur->getOperationType())) {
			continue;
		}
		std::vector<Operation*> operands(cur->getInputs().begin(), cur->getInputs().end());
		erase(cur);
		++erasedCount;
		worklist.insert(worklist.end(), operands.begin(), operands.end());
	}
	return erasedCount;
}

void FunctionRewriter::removeBlockArgument(BasicBlock* block, size_t index) {
	const auto& args = block->getArguments();
	if (index >= args.size()) {
		throw RuntimeException("FunctionRewriter::removeBlockArgument: index " + std::to_string(index) +
		                       " out of range for block " + std::to_string(block->getIdentifier().getId()));
	}
	BasicBlockArgument* arg = args[index];
	if (useCount(arg) != 0) {
		throw RuntimeException("FunctionRewriter::removeBlockArgument: argument " + arg->getIdentifier().toString() +
		                       " of block " + std::to_string(block->getIdentifier().getId()) + " still has " +
		                       std::to_string(useCount(arg)) + " use(s)");
	}

	auto targetInvocations = invocationsTargeting(block);
	for (auto* inv : targetInvocations) {
		// Removing a middle argument shifts every later operand index, so
		// the recorded use edges for this invocation are re-derived from
		// scratch rather than patched in place.
		unregisterUser(inv);
		inv->removeArgument(index);
		registerUses(inv);
	}
	block->removeArgument(index);
	defBlock_.erase(arg);
	uses_.erase(arg);
}

BasicBlockArgument*
FunctionRewriter::addBlockArgument(BasicBlock* block, Type stamp,
                                   const std::function<Operation*(BasicBlockInvocation&)>& valueForEdge) {
	auto* newArg = arena_.create<BasicBlockArgument>(freshId(), stamp);
	block->addArgument(newArg);
	defBlock_[newArg] = block;

	auto targetInvocations = invocationsTargeting(block);
	for (auto* inv : targetInvocations) {
		Operation* value = valueForEdge(*inv);
		const auto newIndex = static_cast<uint32_t>(inv->getArguments().size());
		inv->addArgument(arena_, value);
		if (value != nullptr) {
			uses_[value].push_back(Use {inv, newIndex});
		}
	}
	return newArg;
}

void FunctionRewriter::setInvocationArguments(BasicBlockInvocation& inv, std::span<Operation* const> newArgs) {
	unregisterUser(&inv);
	inv.clearArguments();
	for (auto* arg : newArgs) {
		inv.addArgument(arena_, arg);
	}
	registerUses(&inv);
}

void FunctionRewriter::setInvocationTarget(BasicBlockInvocation& inv, BasicBlock* newTarget) {
	auto* oldTarget = const_cast<BasicBlock*>(inv.getBlock());
	if (oldTarget == newTarget) {
		return;
	}
	// An invocation has no back-pointer to its owning block (by design, see
	// BasicBlockInvocation's class comment), so the source block is found by
	// searching the old target's predecessors for the one whose terminator
	// contains this exact invocation object. Bounded by the old target's
	// predecessor count, not a function-wide scan.
	BasicBlock* source = nullptr;
	if (oldTarget != nullptr) {
		for (auto* pred : oldTarget->getPredecessors()) {
			if (pred == nullptr) {
				continue;
			}
			auto* terminator = pred->getTerminatorOp();
			if (terminator == nullptr) {
				continue;
			}
			auto invocations = getSuccessorInvocations(*terminator);
			if (std::find(invocations.begin(), invocations.end(), &inv) != invocations.end()) {
				source = pred;
				break;
			}
		}
	}
	inv.setBlock(newTarget);
	if (source != nullptr) {
		if (oldTarget != nullptr) {
			oldTarget->removePredecessor(source);
		}
		if (newTarget != nullptr) {
			newTarget->addPredecessor(source);
		}
	}
}

void FunctionRewriter::replaceTerminator(BasicBlock* block, Operation* newTerminator) {
	if (!block->getOperations().empty()) {
		Operation* oldTerminator = block->getTerminatorOp();
		unregisterUser(oldTerminator);
		for (auto* inv : getSuccessorInvocations(*oldTerminator)) {
			unregisterUser(inv);
		}
		defBlock_.erase(oldTerminator);
		uses_.erase(oldTerminator);
	}
	// Handles predecessor-list consistency for both the old and new
	// terminator's successors.
	block->replaceTerminatorOperation(newTerminator);
	defBlock_[newTerminator] = block;
	registerUses(newTerminator);
	for (auto* inv : getSuccessorInvocations(*newTerminator)) {
		registerUses(inv);
	}
}

void FunctionRewriter::eraseBlock(BasicBlock* block) {
	if (block == fn_.getEntryBlock()) {
		throw RuntimeException("FunctionRewriter::eraseBlock: cannot erase the function's entry block");
	}
	if (!block->getPredecessors().empty()) {
		throw RuntimeException("FunctionRewriter::eraseBlock: block " + std::to_string(block->getIdentifier().getId()) +
		                       " still has predecessors");
	}

	for (auto* arg : block->getArguments()) {
		defBlock_.erase(arg);
		uses_.erase(arg);
	}
	for (auto* op : block->getOperations()) {
		unregisterUser(op);
		for (auto* inv : getSuccessorInvocations(*op)) {
			unregisterUser(inv);
			auto* target = const_cast<BasicBlock*>(inv->getBlock());
			if (target != nullptr) {
				// `block` is being detached: its outgoing edges must no
				// longer be listed as incoming edges on their targets.
				target->removePredecessor(block);
			}
		}
		defBlock_.erase(op);
		uses_.erase(op);
	}
	fn_.detachBasicBlock(block);
}

} // namespace nautilus::compiler::ir
