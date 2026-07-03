
#include "nautilus/compiler/ir/passes/StrengthReductionPass.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockArgument.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlockInvocation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/AddOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/MulOperation.hpp"
#include "nautilus/compiler/ir/operations/CastOperation.hpp"
#include "nautilus/compiler/ir/operations/ConstIntOperation.hpp"
#include "nautilus/compiler/ir/operations/FunctionOperation.hpp"
#include "nautilus/compiler/ir/operations/Operation.hpp"
#include "nautilus/compiler/ir/passes/FunctionRewriter.hpp"
#include "nautilus/compiler/ir/util/ControlFlowUtil.hpp"
#include <algorithm>
#include <cstdint>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace nautilus::compiler::ir {

namespace {

/// Counts how many times @p target appears as an operand anywhere in @p fn:
/// either as a plain operation input (covers every fixed-arity op kind
/// generically, since Operation::inputs is where all of them store their
/// operands) or as a block-invocation argument (a separate embedded
/// Operation, not reachable through its owning terminator's own getInputs()).
int countConsumers(FunctionOperation& fn, Operation* target) {
	int count = 0;
	for (auto* block : fn.getBasicBlocks()) {
		for (auto* op : block->getOperations()) {
			for (auto* input : op->getInputs()) {
				if (input == target) {
					count++;
				}
			}
			for (auto* inv : getSuccessorInvocations(*op)) {
				for (auto* arg : inv->getArguments()) {
					if (arg == target) {
						count++;
					}
				}
			}
		}
	}
	return count;
}

/// A confirmed rewrite candidate: multiply-by-constant-stride of a loop
/// induction variable (optionally through one cast), feeding an add of a
/// loop-invariant base pointer, inside a simple single-latch/single-preheader
/// loop.
struct Candidate {
	BasicBlock* header;
	BasicBlockArgument* ivArg;
	size_t ivSlot;
	int64_t step;
	BasicBlock* preheader;
	BasicBlockInvocation* preheaderInv;
	BasicBlock* latch;
	BasicBlockInvocation* latchInv;
	BasicBlock* mulBlock;
	MulOperation* mul;
	ConstIntOperation* elemSizeConst;
	CastOperation* castOp; // nullptr if the multiply reads ivArg directly
	AddOperation* add2;
	Operation* basePtr;
};

/// Forward-reachability (via successor edges, >= 1 step) from every block in
/// @p fn, memoized per block. Function CFGs here are small (tens of blocks),
/// so a plain per-block BFS is more than fast enough.
class Reachability {
public:
	const std::unordered_set<BasicBlock*>& from(BasicBlock* start) {
		auto it = cache_.find(start);
		if (it != cache_.end()) {
			return it->second;
		}
		std::unordered_set<BasicBlock*> visited;
		std::vector<BasicBlock*> frontier = start->getSuccessors();
		while (!frontier.empty()) {
			auto* b = frontier.back();
			frontier.pop_back();
			if (b == nullptr || !visited.insert(b).second) {
				continue;
			}
			for (auto* s : b->getSuccessors()) {
				frontier.push_back(s);
			}
		}
		return cache_.emplace(start, std::move(visited)).first->second;
	}

private:
	std::unordered_map<BasicBlock*, std::unordered_set<BasicBlock*>> cache_;
};

/// Resolves the induction-variable operand of a multiply: either the header
/// argument directly, or a CastOperation whose sole input is that argument.
/// Returns {nullptr, nullptr} if @p operand doesn't match either shape.
struct IvUse {
	CastOperation* castOp = nullptr; // non-null iff matched through a cast
	bool matched = false;
};

IvUse matchIvOperand(Operation* operand, const std::unordered_set<Operation*>& ivAliases) {
	if (ivAliases.contains(operand)) {
		return {nullptr, true};
	}
	if (auto* c = dyn_cast<CastOperation>(operand)) {
		if (ivAliases.contains(c->getInput())) {
			return {c, true};
		}
	}
	return {nullptr, false};
}

/// Tries to read the multiply's (indexOperandMatch, constant) pair regardless
/// of operand order. Returns false if neither operand is a ConstIntOperation.
bool splitMulOperands(MulOperation& mul, Operation*& indexOperand, ConstIntOperation*& elemSizeConst) {
	if (auto* c = dyn_cast<ConstIntOperation>(mul.getRightInput())) {
		indexOperand = mul.getLeftInput();
		elemSizeConst = c;
		return true;
	}
	if (auto* c = dyn_cast<ConstIntOperation>(mul.getLeftInput())) {
		indexOperand = mul.getRightInput();
		elemSizeConst = c;
		return true;
	}
	return false;
}

/// Finds the natural-loop back edge / preheader edge into @p header, if
/// header has exactly the shape this pass handles: exactly two predecessor
/// edges, one of which is a back edge (header is reachable from it) and one
/// a forward edge (the preheader). Returns false (leaving the out-params
/// untouched) for anything else -- multiple latches, multiple preheaders,
/// irreducible control flow, etc. are all conservatively skipped.
bool findSimpleLoopEdges(BasicBlock* header, Reachability& reach, BasicBlock*& latch, BasicBlockInvocation*& latchInv,
                         BasicBlock*& preheader, BasicBlockInvocation*& preheaderInv) {
	const auto& preds = header->getPredecessors();
	if (preds.size() != 2) {
		return false;
	}
	BasicBlock* back = nullptr;
	BasicBlock* fwd = nullptr;
	for (auto* p : preds) {
		if (p == nullptr) {
			return false;
		}
		const bool isBack = reach.from(header).contains(p);
		if (isBack) {
			if (back != nullptr) {
				return false;
			}
			back = p;
		} else {
			if (fwd != nullptr) {
				return false;
			}
			fwd = p;
		}
	}
	if (back == nullptr || fwd == nullptr) {
		return false;
	}

	auto findSingleInvocationTo = [&](BasicBlock* pred, BasicBlockInvocation*& out) {
		BasicBlockInvocation* found = nullptr;
		int count = 0;
		for (auto* inv : getSuccessorInvocations(*pred->getTerminatorOp())) {
			if (inv->getBlock() == header) {
				found = inv;
				count++;
			}
		}
		if (count != 1) {
			return false;
		}
		out = found;
		return true;
	};
	if (!findSingleInvocationTo(back, latchInv) || !findSingleInvocationTo(fwd, preheaderInv)) {
		return false;
	}
	if (latchInv->getArguments().size() != header->getArguments().size() ||
	    preheaderInv->getArguments().size() != header->getArguments().size()) {
		return false;
	}
	latch = back;
	preheader = fwd;
	return true;
}

/// Computes the natural-loop body for the (header, latch) pair: header
/// itself plus every block reachable from header that can also reach latch.
std::unordered_set<BasicBlock*> computeLoopBody(BasicBlock* header, BasicBlock* latch, Reachability& reach) {
	std::unordered_set<BasicBlock*> body;
	body.insert(header);
	for (auto* b : reach.from(header)) {
		if (b == latch || reach.from(b).contains(latch)) {
			body.insert(b);
		}
	}
	body.insert(latch);
	return body;
}

/// A control-flow edge into a successor block rebinds every argument to a
/// *fresh* BasicBlockArgument object owned by that successor -- even a plain
/// pass-through edge (e.g. the header handing its own induction variable
/// straight into the loop body block) mints a new object that is only
/// value-equivalent to the source, not pointer-equal. Operations inside the
/// loop body therefore never reference the header's own argument object
/// directly; they reference whichever block-local alias was bound on the
/// edge that reached them. This walks every pass-through edge inside
/// @p loopBody starting from @p ivArg and returns the full set of objects
/// that all represent the same loop-carried value (including @p ivArg
/// itself). The insert-based visited check naturally stops the walk at the
/// back edge (whose argument is the *incremented* value, a different object)
/// without needing to special-case it.
std::unordered_set<Operation*> computeIvAliases(BasicBlock* header, BasicBlockArgument* ivArg,
                                                const std::unordered_set<BasicBlock*>& loopBody) {
	std::unordered_set<Operation*> aliases;
	aliases.insert(ivArg);
	std::vector<std::pair<BasicBlock*, Operation*>> frontier = {{header, ivArg}};
	while (!frontier.empty()) {
		auto [block, val] = frontier.back();
		frontier.pop_back();
		auto* terminator = block->getTerminatorOp();
		if (terminator == nullptr) {
			continue;
		}
		for (auto* inv : getSuccessorInvocations(*terminator)) {
			auto* target = const_cast<BasicBlock*>(inv->getBlock());
			if (target == nullptr || !loopBody.contains(target)) {
				continue;
			}
			const auto args = inv->getArguments();
			const auto& targetArgs = target->getArguments();
			for (size_t k = 0; k < args.size() && k < targetArgs.size(); k++) {
				if (args[k] == val && aliases.insert(targetArgs[k]).second) {
					frontier.emplace_back(target, targetArgs[k]);
				}
			}
		}
	}
	return aliases;
}

/// For every header argument slot, determines whether the loop actually
/// changes it or just threads the same value through unchanged every
/// iteration (a "pass-through" phi, e.g. a pointer parameter that flows into
/// the body block and back out to the header via the back edge without any
/// arithmetic on it). A slot is a pass-through iff the latch's supplied
/// value for that slot is itself a member of the slot's own alias set (see
/// computeIvAliases) -- i.e. closing the back edge doesn't introduce a new
/// value, it just confirms the cycle is stable. Returns a map from every
/// alias object of every pass-through slot to `true`; members of a slot
/// whose latch value is *not* in the alias set (a real induction/accumulator
/// variable) are omitted, so callers should treat "not present" as unknown
/// rather than "not invariant".
std::unordered_map<Operation*, bool> computeHeaderPassThroughMap(BasicBlock* header, BasicBlockInvocation* latchInv,
                                                                 const std::unordered_set<BasicBlock*>& loopBody) {
	std::unordered_map<Operation*, bool> result;
	const auto& headerArgs = header->getArguments();
	const auto latchArgs = latchInv->getArguments();
	for (size_t k = 0; k < headerArgs.size() && k < latchArgs.size(); k++) {
		auto aliases = computeIvAliases(header, headerArgs[k], loopBody);
		if (aliases.contains(latchArgs[k])) {
			for (auto* a : aliases) {
				result[a] = true;
			}
		}
	}
	return result;
}

/// Returns true if @p op is loop-invariant: either its defining block is
/// outside @p loopBody outright, or it is a member of a header argument slot
/// that @p passThroughMap identified as a pass-through (see
/// computeHeaderPassThroughMap). Operations with no entry in @p definingBlock
/// (e.g. a function argument on the entry block, already covered by the
/// first check) are treated as invariant.
bool isLoopInvariant(Operation* op, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                     const std::unordered_set<BasicBlock*>& loopBody,
                     const std::unordered_map<Operation*, bool>& passThroughMap) {
	if (passThroughMap.contains(op)) {
		return true;
	}
	auto it = definingBlock.find(op);
	if (it == definingBlock.end()) {
		return true;
	}
	return !loopBody.contains(it->second);
}

void findCandidatesInFunction(FunctionOperation& fn, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                              std::vector<Candidate>& out) {
	Reachability reach;

	for (auto* header : fn.getBasicBlocks()) {
		BasicBlock* latch = nullptr;
		BasicBlockInvocation* latchInv = nullptr;
		BasicBlock* preheader = nullptr;
		BasicBlockInvocation* preheaderInv = nullptr;
		if (!findSimpleLoopEdges(header, reach, latch, latchInv, preheader, preheaderInv)) {
			continue;
		}
		auto loopBody = computeLoopBody(header, latch, reach);
		if (loopBody.contains(preheader)) {
			continue;
		}
		auto passThroughMap = computeHeaderPassThroughMap(header, latchInv, loopBody);

		const auto& headerArgs = header->getArguments();
		for (size_t j = 0; j < headerArgs.size(); j++) {
			auto* ivArg = headerArgs[j];
			if (ivArg->getStamp() != Type::i32 && ivArg->getStamp() != Type::i64 && ivArg->getStamp() != Type::ui32 &&
			    ivArg->getStamp() != Type::ui64) {
				continue;
			}
			// Every use of the induction variable inside the loop body reads a
			// block-local alias bound by a pass-through edge, not `ivArg`
			// itself (see computeIvAliases) -- resolve the full equivalence
			// set once per (header, slot) before matching anything against it.
			auto ivAliases = computeIvAliases(header, ivArg, loopBody);

			auto* backVal = latchInv->getArguments()[j];
			auto* addBack = dyn_cast<AddOperation>(backVal);
			if (addBack == nullptr) {
				continue;
			}
			int64_t step = 0;
			bool haveStep = false;
			if (ivAliases.contains(addBack->getLeftInput())) {
				if (auto* c = dyn_cast<ConstIntOperation>(addBack->getRightInput())) {
					step = c->getValue();
					haveStep = true;
				}
			} else if (ivAliases.contains(addBack->getRightInput())) {
				if (auto* c = dyn_cast<ConstIntOperation>(addBack->getLeftInput())) {
					step = c->getValue();
					haveStep = true;
				}
			}
			if (!haveStep) {
				continue;
			}

			for (auto* B : loopBody) {
				for (auto* op : B->getOperations()) {
					auto* mul = dyn_cast<MulOperation>(op);
					if (mul == nullptr) {
						continue;
					}
					Operation* indexOperand = nullptr;
					ConstIntOperation* elemSizeConst = nullptr;
					if (!splitMulOperands(*mul, indexOperand, elemSizeConst)) {
						continue;
					}
					if (elemSizeConst->getValue() <= 0) {
						continue;
					}
					auto ivUse = matchIvOperand(indexOperand, ivAliases);
					if (!ivUse.matched) {
						continue;
					}

					// Find the (single, expected) Add that consumes this mul.
					for (auto* B2 : loopBody) {
						for (auto* op2 : B2->getOperations()) {
							auto* add2 = dyn_cast<AddOperation>(op2);
							if (add2 == nullptr) {
								continue;
							}
							Operation* basePtr = nullptr;
							if (add2->getLeftInput() == mul) {
								basePtr = add2->getRightInput();
							} else if (add2->getRightInput() == mul) {
								basePtr = add2->getLeftInput();
							} else {
								continue;
							}
							if (basePtr->getStamp() != Type::ptr) {
								continue;
							}
							if (!isLoopInvariant(basePtr, definingBlock, loopBody, passThroughMap)) {
								continue;
							}
							if (countConsumers(fn, mul) != 1) {
								continue;
							}
							out.push_back(Candidate {header, ivArg, j, step, preheader, preheaderInv, latch, latchInv,
							                         B, mul, elemSizeConst, ivUse.castOp, add2, basePtr});
						}
					}
				}
			}
		}
	}
}

size_t findOperationIndex(BasicBlock* block, Operation* op) {
	const auto& ops = block->getOperations();
	auto it = std::find(ops.begin(), ops.end(), op);
	return static_cast<size_t>(std::distance(ops.begin(), it));
}

int64_t wrappingMul(int64_t a, int64_t b) {
	return static_cast<int64_t>(static_cast<uint64_t>(a) * static_cast<uint64_t>(b));
}

void applyCandidate(FunctionRewriter& rewriter, common::Arena& arena, const Candidate& c) {
	// New loop-carried pointer argument on the header. Built directly
	// (rather than through `FunctionRewriter::addBlockArgument`) because
	// `latchAdd` below must reference `ptrArg` itself, so `ptrArg` has to
	// exist before the per-edge invocation values are computed.
	auto* ptrArg = arena.create<BasicBlockArgument>(rewriter.freshId(), Type::ptr);
	c.header->addArgument(ptrArg);

	// Preheader: initPtr = basePtr + maybeCast(preheaderIVValue) * elemSize.
	// A constant is an *operation* that must be computed by whichever block
	// uses it -- the original elemSizeConst lives inside the loop body, which
	// does not dominate (and may not even have executed before) the
	// preheader, so a fresh copy of the same value is minted here rather
	// than reusing that operation across blocks.
	Operation* preheaderIvValue = c.preheaderInv->getArguments()[c.ivSlot];
	Operation* preheaderIndexOperand = preheaderIvValue;
	if (c.castOp != nullptr) {
		preheaderIndexOperand = rewriter.createBeforeTerminator<CastOperation>(c.preheader, rewriter.freshId(),
		                                                                       preheaderIvValue, c.castOp->getStamp());
	}
	auto* preheaderElemSizeConst = rewriter.createBeforeTerminator<ConstIntOperation>(
	    c.preheader, rewriter.freshId(), c.elemSizeConst->getValue(), c.elemSizeConst->getStamp());
	auto* preheaderMul = rewriter.createBeforeTerminator<MulOperation>(c.preheader, rewriter.freshId(),
	                                                                   preheaderIndexOperand, preheaderElemSizeConst);
	auto* preheaderAdd =
	    rewriter.createBeforeTerminator<AddOperation>(c.preheader, rewriter.freshId(), c.basePtr, preheaderMul);
	c.preheaderInv->addArgument(arena, preheaderAdd);

	// Latch: nextPtr = ptrArg + (step * elemSize).
	const int64_t stride = wrappingMul(c.step, c.elemSizeConst->getValue());
	auto* strideConst = rewriter.createBeforeTerminator<ConstIntOperation>(c.latch, rewriter.freshId(), stride,
	                                                                       c.elemSizeConst->getStamp());
	auto* latchAdd = rewriter.createBeforeTerminator<AddOperation>(c.latch, rewriter.freshId(), ptrArg, strideConst);
	c.latchInv->addArgument(arena, latchAdd);

	// Mutate the original base+index*stride add in place so every existing
	// consumer (whatever operation kind it is) sees the new value through
	// the same pointer it already holds -- no need to find/rewrite them.
	// Its new right operand doubles as the multiply's neutralized
	// replacement below: that is the *same* object as what add2 now reads,
	// so it only needs to be inserted into one block's operation list (the
	// multiply's -- which dominates add2, since add2 originally consumed the
	// multiply's result) to be computed before add2 reads it. Reusing the
	// multiply's own identifier means any (unexpected) stray reference to it
	// still resolves to a valid, harmless value instead of a dangling one.
	auto* zeroConst = arena.create<ConstIntOperation>(arena, c.mul->getIdentifier(), 0, c.mul->getStamp());
	c.add2->setLeftInput(ptrArg);
	c.add2->setRightInput(zeroConst);
	c.mulBlock->replaceOperation(findOperationIndex(c.mulBlock, c.mul), zeroConst);
}

} // namespace

bool StrengthReductionPass::apply(IRGraph& ir) {
	common::Arena& arena = ir.getArena();
	bool changed = false;

	for (auto* fn : ir.getFunctionOperations()) {
		if (fn == nullptr) {
			continue;
		}
		std::unordered_map<Operation*, BasicBlock*> definingBlock;
		for (auto* block : fn->getBasicBlocks()) {
			for (auto* arg : block->getArguments()) {
				definingBlock[arg] = block;
			}
			for (auto* op : block->getOperations()) {
				definingBlock[op] = block;
			}
		}

		std::vector<Candidate> candidates;
		findCandidatesInFunction(*fn, definingBlock, candidates);
		if (candidates.empty()) {
			continue;
		}
		changed = true;
		// Identifiers minted for one function's candidates must never
		// collide with anything already in that function; a fresh session
		// per function (rather than one global counter reused across
		// functions) gives that guarantee via `freshId()`.
		FunctionRewriter rewriter(*fn, arena);
		for (const auto& c : candidates) {
			applyCandidate(rewriter, arena, c);
		}
	}
	return changed;
}

} // namespace nautilus::compiler::ir
