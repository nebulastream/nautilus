#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/OperationEffects.hpp"
#include "nautilus/compiler/ir/operations/BranchOperation.hpp"
#include "nautilus/compiler/ir/operations/DestructorOperands.hpp"
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

namespace {

/// Block arguments plus operations: the number of `defBlock_` entries, and an
/// upper bound on the number of distinct values with a use list.
size_t countDefinitions(const FunctionOperation& fn) {
	size_t count = 0;
	for (const auto* block : fn.getBasicBlocks()) {
		count += block->getArguments().size() + block->getOperations().size();
	}
	return count;
}

/// Rough bytes the two tables need for @p definitions entries, so the first
/// buffer already holds a typical session and growth stays geometric from
/// there. A hash node, its use list's first storage and a bucket slot come to
/// well under this per definition.
size_t initialTableBytes(size_t definitions) {
	constexpr size_t bytesPerDefinition = 128;
	constexpr size_t minimum = 4 * 1024;
	return std::max(minimum, definitions * bytesPerDefinition);
}

} // namespace

FunctionRewriter::FunctionRewriter(FunctionOperation& fn, common::Arena& arena, const IRGraph* ir)
    : fn_(fn), arena_(arena), ir_(ir), tableMemory_(initialTableBytes(countDefinitions(fn))), uses_(&tableMemory_),
      defBlock_(&tableMemory_) {
	const size_t definitions = countDefinitions(fn);
	// Sized once: a rehash would abandon the old bucket array inside the
	// monotonic buffer.
	uses_.reserve(definitions);
	defBlock_.reserve(definitions);
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

namespace {

/// Calls @p fn(operandIndex, operand) for every operand of @p user: its
/// `Operation::inputs`, then its destructor addresses (see `Use`).
template <typename Fn>
void forEachOperand(Operation* user, Fn&& fn) {
	auto ins = user->getInputs();
	for (uint32_t i = 0; i < ins.size(); ++i) {
		fn(i, ins[i]);
	}
	const auto destructorCount = static_cast<uint32_t>(getDestructorOperandCount(*user));
	for (uint32_t i = 0; i < destructorCount; ++i) {
		fn(FunctionRewriter::destructorOperandBase + i, getDestructorOperand(*user, i));
	}
}

void setOperand(Operation* user, uint32_t operandIndex, Operation* value) {
	if (operandIndex >= FunctionRewriter::destructorOperandBase) {
		setDestructorOperand(*user, operandIndex - FunctionRewriter::destructorOperandBase, value);
	} else {
		user->setInput(operandIndex, value);
	}
}

} // namespace

void FunctionRewriter::registerUses(Operation* user) {
	forEachOperand(user, [&](uint32_t i, Operation* operand) {
		if (operand != nullptr) {
			uses_[operand].push_back(Use {user, i});
		}
	});
}

void FunctionRewriter::unregisterUser(Operation* user) {
	forEachOperand(user, [&](uint32_t i, Operation* operand) {
		if (operand == nullptr) {
			return;
		}
		auto it = uses_.find(operand);
		if (it == uses_.end()) {
			return;
		}
		auto& vec = it->second;
		vec.erase(
		    std::remove_if(vec.begin(), vec.end(), [&](const Use& u) { return u.user == user && u.operandIndex == i; }),
		    vec.end());
		if (vec.empty()) {
			uses_.erase(it);
		}
	});
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
	auto moved = std::move(it->second);
	uses_.erase(it);
	for (auto& u : moved) {
		setOperand(u.user, u.operandIndex, to);
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
		// Purity per operation: a call whose callee provably touches no memory
		// and returns is removable when its result is unused; every other
		// opcode gets the same answer isPureOp(type) gave.
		if (useCount(cur) != 0 ||
		    (ir_ != nullptr ? !isPureOperation(*ir_, *cur) : !isPureOp(cur->getOperationType()))) {
			continue;
		}
		std::vector<Operation*> operands;
		forEachOperand(cur, [&](uint32_t, Operation* operand) { operands.push_back(operand); });
		erase(cur);
		++erasedCount;
		worklist.insert(worklist.end(), operands.begin(), operands.end());
	}
	return erasedCount;
}

void FunctionRewriter::removeBlockArgument(BasicBlock* block, size_t index) {
	const BlockArgumentSlot slot {block, index};
	removeBlockArguments(std::span<const BlockArgumentSlot>(&slot, 1));
}

void FunctionRewriter::removeBlockArguments(std::span<const BlockArgumentSlot> slots) {
	// Group by block, in first-seen order, so the result does not depend on
	// hash iteration.
	std::vector<BasicBlock*> blocks;
	std::unordered_map<BasicBlock*, std::vector<size_t>> indicesOf;
	for (const auto& slot : slots) {
		const auto& args = slot.block->getArguments();
		if (slot.index >= args.size()) {
			throw RuntimeException("FunctionRewriter::removeBlockArguments: index " + std::to_string(slot.index) +
			                       " out of range for block " + std::to_string(slot.block->getIdentifier().getId()));
		}
		BasicBlockArgument* arg = args[slot.index];
		if (useCount(arg) != 0) {
			throw RuntimeException("FunctionRewriter::removeBlockArguments: argument " +
			                       arg->getIdentifier().toString() + " of block " +
			                       std::to_string(slot.block->getIdentifier().getId()) + " still has " +
			                       std::to_string(useCount(arg)) + " use(s)");
		}
		auto [it, inserted] = indicesOf.try_emplace(slot.block);
		if (inserted) {
			blocks.push_back(slot.block);
		}
		it->second.push_back(slot.index);
	}

	static constexpr uint32_t removedSlot = ~0u;
	// Per rewritten invocation: old operand index -> new operand index, or
	// `removedSlot`. Shared by every invocation targeting the same block.
	std::vector<std::vector<uint32_t>> remaps;
	remaps.reserve(blocks.size());
	std::unordered_map<const Operation*, const std::vector<uint32_t>*> remapOf;
	// Every value some rewritten invocation passes, in first-seen order.
	std::vector<const Operation*> touched;
	std::unordered_set<const Operation*> touchedSet;

	for (auto* block : blocks) {
		auto& indices = indicesOf[block];
		std::sort(indices.begin(), indices.end());
		indices.erase(std::unique(indices.begin(), indices.end()), indices.end());

		// Sized by the widest argument list in play so that, should an
		// invocation carry more arguments than the block declares (arity
		// drift the verifier reports), its surplus entries still shift
		// consistently instead of being dropped.
		auto targetInvocations = invocationsTargeting(block);
		size_t arity = block->getArguments().size();
		for (auto* inv : targetInvocations) {
			arity = std::max(arity, inv->getArguments().size());
		}
		auto& remap = remaps.emplace_back(arity, removedSlot);
		uint32_t next = 0;
		size_t nextRemoved = 0;
		for (size_t i = 0; i < arity; ++i) {
			if (nextRemoved < indices.size() && indices[nextRemoved] == i) {
				++nextRemoved;
				continue;
			}
			remap[i] = next++;
		}

		for (auto* inv : targetInvocations) {
			for (auto* value : inv->getArguments()) {
				if (value != nullptr && touchedSet.insert(value).second) {
					touched.push_back(value);
				}
			}
			remapOf.emplace(inv, &remap);
			inv->removeArguments(indices);
		}

		for (const size_t index : indices) {
			BasicBlockArgument* arg = block->getArguments()[index];
			defBlock_.erase(arg);
			uses_.erase(arg);
		}
		block->removeArguments(indices);
	}

	// One pass over each affected value's use list: entries of a rewritten
	// invocation move to their new index or disappear with their slot; every
	// other entry is untouched.
	for (const auto* value : touched) {
		auto it = uses_.find(value);
		if (it == uses_.end()) {
			continue;
		}
		auto& vec = it->second;
		size_t kept = 0;
		for (size_t i = 0; i < vec.size(); ++i) {
			Use use = vec[i];
			if (auto remapIt = remapOf.find(use.user); remapIt != remapOf.end()) {
				const auto& remap = *remapIt->second;
				const uint32_t mapped = use.operandIndex < remap.size() ? remap[use.operandIndex] : removedSlot;
				if (mapped == removedSlot) {
					continue;
				}
				use.operandIndex = mapped;
			}
			vec[kept++] = use;
		}
		vec.resize(kept);
		if (vec.empty()) {
			uses_.erase(it);
		}
	}
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

void FunctionRewriter::mergeIntoPredecessor(BasicBlock* pred, BasicBlock* succ) {
	if (succ == pred) {
		throw RuntimeException("FunctionRewriter::mergeIntoPredecessor: cannot merge block " +
		                       std::to_string(succ->getIdentifier().getId()) + " into itself");
	}
	if (succ == fn_.getEntryBlock()) {
		throw RuntimeException("FunctionRewriter::mergeIntoPredecessor: cannot merge the function's entry block");
	}
	Operation* predTerminator = pred->getTerminatorOp();
	auto* branch = predTerminator != nullptr ? dyn_cast<BranchOperation>(predTerminator) : nullptr;
	if (branch == nullptr || branch->getNextBlockInvocation().getBlock() != succ) {
		throw RuntimeException(
		    "FunctionRewriter::mergeIntoPredecessor: block " + std::to_string(pred->getIdentifier().getId()) +
		    " does not end in an unconditional branch to block " + std::to_string(succ->getIdentifier().getId()));
	}
	const auto& preds = succ->getPredecessors();
	if (preds.size() != 1 || preds[0] != pred) {
		throw RuntimeException("FunctionRewriter::mergeIntoPredecessor: block " +
		                       std::to_string(succ->getIdentifier().getId()) + " has " + std::to_string(preds.size()) +
		                       " predecessor edge(s), expected exactly one");
	}
	auto& inv = branch->getNextBlockInvocation();
	const auto& args = succ->getArguments();
	if (inv.getArguments().size() != args.size()) {
		throw RuntimeException("FunctionRewriter::mergeIntoPredecessor: invocation passes " +
		                       std::to_string(inv.getArguments().size()) + " argument(s) but block " +
		                       std::to_string(succ->getIdentifier().getId()) + " declares " +
		                       std::to_string(args.size()));
	}
	// A value defined in `succ` can only reach the branch's argument list when
	// `pred` and `succ` sit on an unreachable cycle (a reachable use would
	// violate SSA dominance); splicing would leave the substituted uses
	// dangling, so such a merge is rejected outright.
	for (auto* value : inv.getArguments()) {
		if (value != nullptr && definingBlock(value) == succ) {
			throw RuntimeException("FunctionRewriter::mergeIntoPredecessor: invocation argument " +
			                       value->getIdentifier().toString() + " is defined in the block being merged");
		}
	}

	// Rewire every use of succ's block arguments to the value the branch
	// passes on the (single) incoming edge; the arguments die with the block.
	std::vector<Operation*> invocationArgs(inv.getArguments().begin(), inv.getArguments().end());
	for (size_t i = 0; i < args.size(); ++i) {
		BasicBlockArgument* arg = args[i];
		replaceAllUses(arg, invocationArgs[i]);
		defBlock_.erase(arg);
		uses_.erase(arg);
	}

	// Detach pred's branch. `removeOperation` does no CFG bookkeeping, so the
	// pred -> succ edge is unlisted explicitly.
	unregisterUser(branch);
	unregisterUser(&inv);
	defBlock_.erase(branch);
	uses_.erase(branch);
	pred->removeOperation(branch);
	succ->removePredecessor(pred);

	// pred now holds code from both blocks, so its region has to widen to the innermost
	// region containing both -- merging a region's entry block into its predecessor is
	// exactly how a region's seams disappear (docs/region.md). Each operation keeps its
	// own region, so nothing is lost, only the block-level summary is coarsened.
	pred->setRegionIndex(fn_.commonRegionAncestor(pred->getRegionIndex(), succ->getRegionIndex()));

	// Splice succ's operations (terminator included) onto the end of pred.
	// Operation identity is preserved, so the use table only needs new
	// defining-block entries.
	const auto ops = succ->getOperations();
	for (auto* op : ops) {
		succ->removeOperation(op);
		pred->addOperation(op);
		defBlock_[op] = pred;
	}

	// The adopted terminator's targets list succ as a predecessor, once per
	// invocation edge (a shared-target if contributes two entries); each
	// entry now belongs to pred.
	Operation* newTerminator = pred->getTerminatorOp();
	if (newTerminator != nullptr) {
		for (auto* outgoing : getSuccessorInvocations(*newTerminator)) {
			if (auto* target = const_cast<BasicBlock*>(outgoing->getBlock())) {
				target->removePredecessor(succ);
				target->addPredecessor(pred);
			}
		}
	}

	fn_.detachBasicBlock(succ);
}

} // namespace nautilus::compiler::ir
