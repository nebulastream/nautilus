
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
#include "nautilus/compiler/ir/passes/LoopInfo.hpp"
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

/// Resolves the induction-variable operand of a multiply: either the header
/// argument directly, or a CastOperation whose sole input is that argument.
/// Returns {nullptr, false} if @p operand doesn't match either shape.
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

void findCandidatesInFunction(FunctionOperation& fn, const std::unordered_map<Operation*, BasicBlock*>& definingBlock,
                              std::vector<Candidate>& out) {
	for (auto& loop : findNaturalLoops(fn)) {
		auto* header = loop.header;
		const auto& loopBody = loop.body;
		auto* latchInv = loop.latchInv;
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
							out.push_back(Candidate {header, ivArg, j, step, loop.preheader, loop.preheaderInv,
							                         loop.latch, latchInv, B, mul, elemSizeConst, ivUse.castOp, add2,
							                         basePtr});
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
		const auto definingBlock = computeDefiningBlocks(*fn);

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
