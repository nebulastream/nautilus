
#include "nautilus/compiler/backends/tbc/TBCLoweringProvider.hpp"
#include "nautilus/compiler/Frame.hpp"
#include "nautilus/compiler/backends/tbc/TBCTrampoline.hpp"
#include "nautilus/compiler/ir/OperationDispatcher.hpp"
#include "nautilus/compiler/ir/blocks/BasicBlock.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include <cassert>
#include <cstring>
#include <span>
#include <unordered_map>
#include <unordered_set>

namespace nautilus::compiler::tbc {

namespace {

// Index into the canonical 10-type family order (i8..ui64, f32, f64). Type::b
// and Type::ptr are mapped by the callers according to each family's rules.
int numTypeIndex(Type t) {
	switch (t) {
	case Type::i8:
		return 0;
	case Type::i16:
		return 1;
	case Type::i32:
		return 2;
	case Type::i64:
		return 3;
	case Type::ui8:
		return 4;
	case Type::ui16:
		return 5;
	case Type::ui32:
		return 6;
	case Type::ui64:
		return 7;
	case Type::f32:
		return 8;
	case Type::f64:
		return 9;
	default:
		return -1;
	}
}

// Integer-only families (MOD, bitwise, shifts) share the first 8 entries.
int intTypeIndex(Type t) {
	const int idx = numTypeIndex(t);
	return idx >= 0 && idx <= 7 ? idx : -1;
}

Op typedOp(Op base, int typeIndex, const char* what) {
	if (typeIndex < 0) {
		throw NotImplementedException(std::string("tbc: unsupported type for ") + what);
	}
	return static_cast<Op>(opIndex(base) + typeIndex);
}

/// Map an arithmetic op with a foldable small-constant right operand to its
/// immediate form (Op::COUNT = not foldable).
Op immFormOf(Op op) {
	switch (op) {
	case Op::ADD_i32:
		return Op::ADD_imm_i32;
	case Op::ADD_i64:
		return Op::ADD_imm_i64;
	case Op::SUB_i32:
		return Op::SUB_imm_i32;
	case Op::SUB_i64:
		return Op::SUB_imm_i64;
	case Op::MUL_i32:
		return Op::MUL_imm_i32;
	case Op::MUL_i64:
		return Op::MUL_imm_i64;
	default:
		return Op::COUNT;
	}
}

/// Map a LOAD/STORE opcode to its memory-offset form (LOAD_off/STORE_off),
/// which encodes `base + imm16` instead of reading the offset from a separate
/// register (Op::COUNT = not a LOAD/STORE opcode).
Op memOffsetFormOf(Op op) {
	if (opIndex(op) >= opIndex(Op::LOAD_i8) && opIndex(op) <= opIndex(Op::LOAD_b)) {
		return static_cast<Op>(opIndex(Op::LOAD_off_i8) + (opIndex(op) - opIndex(Op::LOAD_i8)));
	}
	if (opIndex(op) >= opIndex(Op::STORE_i8) && opIndex(op) <= opIndex(Op::STORE_b)) {
		return static_cast<Op>(opIndex(Op::STORE_off_i8) + (opIndex(op) - opIndex(Op::STORE_i8)));
	}
	return Op::COUNT;
}

/// Map an i32/i64 comparison opcode to the fused compare-and-branch form
/// (Op::COUNT = not fusable).
Op fusedFormOf(Op op) {
	const int rel = opIndex(op) - opIndex(Op::EQ_i8);
	if (rel < 0 || rel >= 60) {
		return Op::COUNT;
	}
	const int pred = rel / 10;
	const int typeIdx = rel % 10;
	if (typeIdx != 2 && typeIdx != 3) { // only i32 / i64
		return Op::COUNT;
	}
	return static_cast<Op>(opIndex(Op::CJMP_EQ_i32) + pred * 2 + (typeIdx == 3 ? 1 : 0));
}

/// Write a constant into an image slot with the same normalization the
/// interpreter's writeReg applies (bools 0/1, narrow ints zero-extended,
/// floats bit-exact).
uint64_t normalizedConst(Type t, int64_t value) {
	switch (t) {
	case Type::b:
		return value != 0 ? 1 : 0;
	case Type::i8:
	case Type::ui8:
		return static_cast<uint64_t>(static_cast<uint8_t>(value));
	case Type::i16:
	case Type::ui16:
		return static_cast<uint64_t>(static_cast<uint16_t>(value));
	case Type::i32:
	case Type::ui32:
		return static_cast<uint64_t>(static_cast<uint32_t>(value));
	default:
		return static_cast<uint64_t>(value);
	}
}

struct Terminator {
	enum Kind : uint8_t { None, Jump, CondJump, Ret } kind = None;
	uint16_t condReg = kNoReg;
	int trueBlock = -1;
	int falseBlock = -1;
	uint16_t retReg = kNoReg;
};

/// A foldable arithmetic op (index into `code`, immediate), plus — when the
/// folded constant was materialized by a standalone MOV_imm with no other
/// use — where that MOV_imm lives, so the flattener can drop it too.
struct FoldableImmediate {
	uint32_t codeIndex;
	int16_t immediate;
	bool hasDeadMovSite = false;
	uint32_t deadMovBlock = 0;
	uint32_t deadMovIndex = 0;
};

struct BlockCode {
	std::vector<Instr> code;
	Terminator term;
	// Set when this block ends in a conditional jump whose condition is a
	// single-use comparison; the flattener may fuse the trailing compare into
	// the branch (mirrors bc's fuseCompareIntoBranch).
	bool fuseCompareIntoBranch = false;
	// Arithmetic ops whose right operand is a small integer constant.
	std::vector<FoldableImmediate> foldableImmediates;
	// (LOAD/STORE codeIndex, ptr-add codeIndex) pairs where the add is this
	// block's immediate predecessor instruction and computes this op's
	// address; eligible for LOAD_off/STORE_off fusion once the add's
	// immediate has been folded.
	std::vector<std::pair<uint32_t, uint32_t>> fusableMemOps;
	// Instruction indices the flattener must drop: dead MOV_imms (immediate
	// folding absorbed their only use) and ptr-adds absorbed into a
	// LOAD_off/STORE_off.
	std::unordered_set<uint32_t> deadIndices;
};

using RegisterFrame = Frame<ir::OperationIdentifier, uint16_t>;

class RegisterProvider {
public:
	uint16_t allocRegister() {
		if (!freeList.empty()) {
			const uint16_t reg = freeList.back();
			freeList.pop_back();
			return reg;
		}
		return nextRegister();
	}

	/// Pinned registers hold values pre-initialized in the init image
	/// (constants, function pointers) or written once in the frame prologue
	/// (alloca pointers); they must never be reused by another operation.
	uint16_t allocPinnedRegister() {
		const uint16_t reg = nextRegister();
		pinned.insert(reg);
		return reg;
	}

	/// Bypass the free list (block-invocation temporaries must not alias
	/// already-bound target registers that were freed earlier); the slot may
	/// still be freed later like any other allocation.
	uint16_t allocFreshRegister() {
		return nextRegister();
	}

	void freeRegister(uint16_t reg) {
		if (pinned.count(reg) > 0) {
			return;
		}
		freeList.push_back(reg);
	}

	uint32_t getRegisterCount() const {
		return currentRegister;
	}

private:
	uint16_t nextRegister() {
		if (currentRegister >= kNoReg) {
			throw RuntimeException("tbc: function requires more than 65534 registers");
		}
		return static_cast<uint16_t>(currentRegister++);
	}

	uint32_t currentRegister = 0;
	std::vector<uint16_t> freeList;
	std::unordered_set<uint16_t> pinned;
};

class LoweringContext : public ir::OperationDispatcher<LoweringContext> {
public:
	LoweringContext(const ir::FunctionOperation* func, TBCProgram& program, TBCFunction& out,
	                const TBCLoweringOptions& options)
	    : func(func), program(program), out(out), options(options) {
	}

	void process() {
		RegisterFrame rootFrame;
		const auto& functionBasicBlock = func->getFunctionBasicBlock();
		// argTypes/returnType were pre-filled by the backend (they must be
		// available before this function is lowered, e.g. for trampolines).
		for (auto* argument : functionBasicBlock.getArguments()) {
			const auto argumentRegister = registerProvider.allocRegister();
			rootFrame.setValue(argument->getIdentifier(), argumentRegister);
			out.argRegs.emplace_back(argumentRegister);
			functionArgs.insert(argument->getIdentifier());
		}

		// Materialize the alloca table: one pinned register per entry, laid
		// out contiguously (with per-spec alignment) in the frame's alloca
		// area. The pointers themselves are computed at frame-push time.
		uint32_t allocaOffset = 0;
		for (const auto& spec : func->getAllocaSpecs()) {
			const auto align = static_cast<uint32_t>(spec.align == 0 ? 1 : spec.align);
			if (align > 16) {
				throw NotImplementedException("tbc: alloca alignment > 16 is not supported");
			}
			allocaOffset = (allocaOffset + align - 1) & ~(align - 1);
			const auto slotRegister = registerProvider.allocPinnedRegister();
			out.allocaRegs.emplace_back(slotRegister, allocaOffset);
			functionAllocaSlots.emplace_back(slotRegister);
			allocaOffset += static_cast<uint32_t>(spec.size);
		}
		out.allocaBytes = allocaOffset;

		if (options.enableRegisterAllocator) {
			countAllUsages(&functionBasicBlock);
		}
		processBlock(&functionBasicBlock, rootFrame);

		finalize();
	}

private:
	friend class ir::OperationDispatcher<LoweringContext>;

	const ir::FunctionOperation* func;
	TBCProgram& program;
	TBCFunction& out;
	TBCLoweringOptions options;
	RegisterProvider registerProvider;
	std::vector<BlockCode> blocks;
	std::vector<std::pair<uint16_t, uint64_t>> constInits;
	std::unordered_map<ir::BlockIdentifier, int> activeBlocks;
	std::unordered_map<ir::OperationIdentifier, int> usageCounts;
	std::unordered_set<ir::OperationIdentifier> functionArgs;
	/// Identifiers with at least one use outside their defining block (IR
	/// passes such as block-argument pruning may replace a block argument
	/// with a dominating value from another block). Such values stay live
	/// across block boundaries -- possibly across a backward branch into an
	/// earlier-emitted block -- so emission-order use-count freeing is
	/// unsound for them: their registers are allocated pinned (never
	/// recycled). Registers in the free-list pool therefore only ever back
	/// block-locally-used values, whose def and uses execute contiguously
	/// within one atomic block execution, which is what keeps reuse (and
	/// re-executed stale defs of a reused slot) unobservable. Empty whenever
	/// the IR keeps the strict block-argument threading discipline.
	std::unordered_set<ir::OperationIdentifier> crossBlockIds;
	std::vector<uint16_t> functionAllocaSlots;
	// Where a small-constant's standalone MOV_imm landed (block, codeIndex);
	// consulted when the constant is later folded into an immediate to see if
	// the MOV_imm became dead.
	std::unordered_map<ir::OperationIdentifier, std::pair<uint32_t, uint32_t>> movImmSites;
	// Where a pointer-typed ADD's own instruction landed (block, codeIndex);
	// consulted by a directly-following LOAD/STORE to detect a fusable
	// `ptr + const` address computation.
	std::unordered_map<ir::OperationIdentifier, std::pair<uint32_t, uint32_t>> ptrAddSites;

	void emit(int block, Op op, uint16_t a, uint16_t b = 0, uint16_t c = 0) {
		blocks[block].code.push_back(Instr {opIndex(op), a, b, c});
	}

	int processBlock(const ir::BasicBlock* block, RegisterFrame& frame) {
		auto entry = activeBlocks.find(block->getIdentifier());
		if (entry != activeBlocks.end()) {
			return entry->second;
		}
		const int blockIndex = static_cast<int>(blocks.size());
		activeBlocks.emplace(block->getIdentifier(), blockIndex);
		blocks.emplace_back();
		for (auto* opt : block->getOperations()) {
			this->dispatch(opt, blockIndex, frame);
		}
		return blockIndex;
	}

	// ── Register bookkeeping (ports of the bc allocator helpers) ────────────

	/// When the identifier is already bound, this definition's SSA name doubles
	/// as a downstream merge-block parameter whose register was chosen by an
	/// earlier-emitted predecessor edge (issue #321); write straight into it.
	uint16_t getResultRegister(ir::Operation* opt, RegisterFrame& frame) {
		if (frame.contains(opt->getIdentifier())) {
			return frame.getValue(opt->getIdentifier());
		}
		// Values read outside their defining block must never enter the
		// reuse pool (see the `crossBlockIds` policy).
		if (crossBlockIds.contains(opt->getIdentifier())) {
			return registerProvider.allocPinnedRegister();
		}
		return registerProvider.allocRegister();
	}

	/// Same walk as bc's countAllUsages, additionally classifying
	/// cross-block reads for the pinning policy documented at
	/// `crossBlockIds`: a block's arguments and definitions are recorded
	/// before its uses are scanned, and SSA dominance guarantees a use's
	/// defining block is walked no later than the use itself.
	void countAllUsages(const ir::BasicBlock* entryBlock) {
		std::unordered_set<ir::BlockIdentifier> visited;
		std::vector<const ir::BasicBlock*> worklist;
		worklist.push_back(entryBlock);
		visited.insert(entryBlock->getIdentifier());

		std::unordered_map<const ir::Operation*, const ir::BasicBlock*> definingBlock;
		const ir::BasicBlock* currentBlock = nullptr;

		auto countInput = [this, &definingBlock, &currentBlock](const ir::Operation* input) {
			if (input != nullptr) {
				usageCounts[input->getIdentifier()]++;
				auto it = definingBlock.find(input);
				// An unknown defining block cannot be proven block-local, so
				// it is conservatively treated as a cross-block read.
				if (it == definingBlock.end() || it->second != currentBlock) {
					crossBlockIds.insert(input->getIdentifier());
				}
			}
		};

		while (!worklist.empty()) {
			const auto* block = worklist.back();
			worklist.pop_back();
			currentBlock = block;
			for (auto* argument : block->getArguments()) {
				definingBlock[argument] = block;
			}
			for (auto* opt : block->getOperations()) {
				definingBlock[opt] = block;
			}
			for (auto* opt : block->getOperations()) {
				for (auto* input : opt->getInputs()) {
					countInput(input);
				}
				if (auto* ifOp = ir::dyn_cast<ir::IfOperation>(opt)) {
					for (auto* input : ifOp->getTrueBlockInvocation().getInputs()) {
						countInput(input);
					}
					for (auto* input : ifOp->getFalseBlockInvocation().getInputs()) {
						countInput(input);
					}
					auto* trueBlk = ifOp->getTrueBlockInvocation().getBlock();
					auto* falseBlk = ifOp->getFalseBlockInvocation().getBlock();
					if (trueBlk != nullptr && visited.insert(trueBlk->getIdentifier()).second) {
						worklist.push_back(trueBlk);
					}
					if (falseBlk != nullptr && visited.insert(falseBlk->getIdentifier()).second) {
						worklist.push_back(falseBlk);
					}
				} else if (auto* brOp = ir::dyn_cast<ir::BranchOperation>(opt)) {
					for (auto* input : brOp->getNextBlockInvocation().getInputs()) {
						countInput(input);
					}
					auto* next = brOp->getNextBlockInvocation().getBlock();
					if (next != nullptr && visited.insert(next->getIdentifier()).second) {
						worklist.push_back(next);
					}
				}
			}
		}
	}

	void useValue(const ir::OperationIdentifier& identifier, RegisterFrame& frame) {
		if (!options.enableRegisterAllocator) {
			return;
		}
		if (functionArgs.count(identifier) > 0) {
			return;
		}
		auto it = usageCounts.find(identifier);
		if (it != usageCounts.end() && it->second > 0) {
			it->second--;
			if (it->second == 0 && frame.contains(identifier)) {
				registerProvider.freeRegister(frame.getValue(identifier));
			}
		}
	}

	// ── Constants ────────────────────────────────────────────────────────────

	/// Allocate a pinned register whose slot is pre-set in the init image.
	uint16_t constSlot(uint64_t rawValue) {
		const auto reg = registerProvider.allocPinnedRegister();
		constInits.emplace_back(reg, rawValue);
		return reg;
	}

	void materializeConst(ir::Operation* opt, int block, RegisterFrame& frame, Type type, int64_t value) {
		const auto target = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), target);
		const int idx = intTypeIndex(type == Type::b ? Type::ui8 : type);
		if (idx >= 0 && value >= -32768 && value <= 32767) {
			movImmSites[opt->getIdentifier()] = {static_cast<uint32_t>(block),
			                                     static_cast<uint32_t>(blocks[block].code.size())};
			emit(block, typedOp(Op::MOV_imm_i8, idx, "const"), target, 0, static_cast<uint16_t>(value));
			return;
		}
		emit(block, Op::MOV, target, constSlot(normalizedConst(type, value)));
	}

	void visitConstInt(ir::ConstIntOperation* constInt, int block, RegisterFrame& frame) {
		materializeConst(constInt, block, frame, constInt->getStamp(), constInt->getValue());
	}

	void visitConstBoolean(ir::ConstBooleanOperation* constBool, int block, RegisterFrame& frame) {
		materializeConst(constBool, block, frame, Type::b, constBool->getValue() ? 1 : 0);
	}

	void visitConstFloat(ir::ConstFloatOperation* constFloat, int block, RegisterFrame& frame) {
		uint64_t raw = 0;
		if (constFloat->getStamp() == Type::f32) {
			const auto value = static_cast<float>(constFloat->getValue());
			std::memcpy(&raw, &value, sizeof(value));
		} else {
			const auto value = static_cast<double>(constFloat->getValue());
			std::memcpy(&raw, &value, sizeof(value));
		}
		const auto target = getResultRegister(constFloat, frame);
		frame.setValue(constFloat->getIdentifier(), target);
		emit(block, Op::MOV, target, constSlot(raw));
	}

	void visitConstPtr(ir::ConstPtrOperation* constPtr, int block, RegisterFrame& frame) {
		const auto target = getResultRegister(constPtr, frame);
		frame.setValue(constPtr->getIdentifier(), target);
		emit(block, Op::MOV, target, constSlot(reinterpret_cast<uint64_t>(constPtr->getValue())));
	}

	// ── Arithmetic / logic ───────────────────────────────────────────────────

	/// Record the arithmetic op about to be emitted as immediate-foldable when
	/// its right operand is an i32/i64 constant fitting 16 bits (port of bc's
	/// recordFoldableImmediate). Pointer arithmetic runs as i64 (see
	/// arithTypeIndex) and emits the identical ADD_i64/SUB_i64 opcodes, so
	/// `ptr + const`/`ptr - const` fold the same way — this is what lets
	/// LOAD_off/STORE_off fusion below find an already-folded ADD_imm_i64. If
	/// the folded constant was materialized by a standalone MOV_imm and this
	/// is its only use, folding will leave the MOV_imm dead — record where it
	/// is so the flattener can drop it once the fold actually happens.
	void recordFoldableImmediate(int block, ir::Operation* rightInput, Type type) {
		if (type != Type::i32 && type != Type::i64 && type != Type::ptr) {
			return;
		}
		const auto* constInt = ir::dyn_cast<ir::ConstIntOperation>(rightInput);
		if (constInt == nullptr) {
			return;
		}
		const int64_t value = constInt->getValue();
		if (value < -32768 || value > 32767) {
			return;
		}
		FoldableImmediate entry;
		entry.codeIndex = static_cast<uint32_t>(blocks[block].code.size());
		entry.immediate = static_cast<int16_t>(value);
		const auto siteIt = movImmSites.find(constInt->getIdentifier());
		const auto usageIt = usageCounts.find(constInt->getIdentifier());
		if (siteIt != movImmSites.end() && usageIt != usageCounts.end() && usageIt->second == 1) {
			entry.hasDeadMovSite = true;
			entry.deadMovBlock = siteIt->second.first;
			entry.deadMovIndex = siteIt->second.second;
		}
		blocks[block].foldableImmediates.push_back(entry);
	}

	template <class OperationType>
	void lowerBinary(OperationType* opt, int block, RegisterFrame& frame, Op base, int typeIndex, const char* what) {
		const auto left = frame.getValue(opt->getLeftInput()->getIdentifier());
		const auto right = frame.getValue(opt->getRightInput()->getIdentifier());
		useValue(opt->getLeftInput()->getIdentifier(), frame);
		useValue(opt->getRightInput()->getIdentifier(), frame);
		const auto result = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), result);
		emit(block, typedOp(base, typeIndex, what), result, left, right);
	}

	int arithTypeIndex(Type t) const {
		// Pointer arithmetic runs as i64, matching bc.
		return numTypeIndex(t == Type::ptr ? Type::i64 : t);
	}

	void visitAdd(ir::AddOperation* opt, int block, RegisterFrame& frame) {
		recordFoldableImmediate(block, opt->getRightInput(), opt->getStamp());
		const auto codeIndex = static_cast<uint32_t>(blocks[block].code.size());
		lowerBinary(opt, block, frame, Op::ADD_i8, arithTypeIndex(opt->getStamp()), "add");
		if (opt->getStamp() == Type::ptr) {
			ptrAddSites[opt->getIdentifier()] = {static_cast<uint32_t>(block), codeIndex};
		}
	}

	void visitSub(ir::SubOperation* opt, int block, RegisterFrame& frame) {
		recordFoldableImmediate(block, opt->getRightInput(), opt->getStamp());
		lowerBinary(opt, block, frame, Op::SUB_i8, arithTypeIndex(opt->getStamp()), "sub");
	}

	void visitMul(ir::MulOperation* opt, int block, RegisterFrame& frame) {
		recordFoldableImmediate(block, opt->getRightInput(), opt->getStamp());
		lowerBinary(opt, block, frame, Op::MUL_i8, arithTypeIndex(opt->getStamp()), "mul");
	}

	void visitDiv(ir::DivOperation* opt, int block, RegisterFrame& frame) {
		lowerBinary(opt, block, frame, Op::DIV_i8, arithTypeIndex(opt->getStamp()), "div");
	}

	void visitMod(ir::ModOperation* opt, int block, RegisterFrame& frame) {
		lowerBinary(opt, block, frame, Op::MOD_i8, intTypeIndex(opt->getStamp()), "mod");
	}

	void visitCompare(ir::CompareOperation* opt, int block, RegisterFrame& frame) {
		auto type = opt->getLeftInput()->getStamp();
		if (type == Type::ptr) {
			type = Type::i64; // pointer compares run as i64, matching bc
		} else if (type == Type::b) {
			type = Type::ui8; // bool slots are normalized 0/1
		}
		const int comparator = static_cast<int>(opt->getComparator());
		if (comparator < 0 || comparator > 5) {
			throw NotImplementedException("tbc: unsupported comparator");
		}
		const int idx = numTypeIndex(type);
		if (idx < 0) {
			throw NotImplementedException("tbc: unsupported compare type");
		}
		const auto base = static_cast<Op>(opIndex(Op::EQ_i8) + comparator * 10);
		lowerBinary(opt, block, frame, base, idx, "compare");
	}

	void visitAnd(ir::AndOperation* opt, int block, RegisterFrame& frame) {
		lowerBinary(opt, block, frame, Op::AND_b, 0, "and");
	}

	void visitOr(ir::OrOperation* opt, int block, RegisterFrame& frame) {
		lowerBinary(opt, block, frame, Op::OR_b, 0, "or");
	}

	void visitBinaryComp(ir::BinaryCompOperation* opt, int block, RegisterFrame& frame) {
		Op base;
		switch (opt->getType()) {
		case ir::BinaryCompOperation::BAND:
			base = Op::BAND_i8;
			break;
		case ir::BinaryCompOperation::BOR:
			base = Op::BOR_i8;
			break;
		case ir::BinaryCompOperation::XOR:
			base = Op::BXOR_i8;
			break;
		default:
			throw NotImplementedException("tbc: unsupported binary op");
		}
		lowerBinary(opt, block, frame, base, intTypeIndex(opt->getStamp()), "bitwise");
	}

	void visitShift(ir::ShiftOperation* opt, int block, RegisterFrame& frame) {
		const Op base = opt->getType() == ir::ShiftOperation::LS ? Op::SHL_i8 : Op::SHR_i8;
		lowerBinary(opt, block, frame, base, intTypeIndex(opt->getStamp()), "shift");
	}

	void visitNot(ir::NotOperation* opt, int block, RegisterFrame& frame) {
		const auto input = frame.getValue(opt->getInput()->getIdentifier());
		useValue(opt->getInput()->getIdentifier(), frame);
		const auto result = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), result);
		emit(block, Op::NOT_b, result, input);
	}

	void visitNegate(ir::NegateOperation* opt, int block, RegisterFrame& frame) {
		const auto input = frame.getValue(opt->getInput()->getIdentifier());
		useValue(opt->getInput()->getIdentifier(), frame);
		const auto result = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), result);
		if (opt->getStamp() == Type::f32) {
			emit(block, Op::NEG_f32, result, input);
		} else if (opt->getStamp() == Type::f64) {
			emit(block, Op::NEG_f64, result, input);
		} else {
			// Bitwise negation runs on the full 64-bit slot (bc parity: consumers
			// always read their exact width, so high garbage is unobservable).
			emit(block, Op::BNOT_i64, result, input);
		}
	}

	// ── Memory ───────────────────────────────────────────────────────────────

	int memTypeIndex(Type t, const char* what) {
		if (t == Type::b) {
			return 10; // the LOAD_b / STORE_b slot directly after the 10-type family
		}
		const int idx = numTypeIndex(t == Type::ptr ? Type::i64 : t);
		if (idx < 0) {
			throw NotImplementedException(std::string("tbc: unsupported type for ") + what);
		}
		return idx;
	}

	/// Record the LOAD/STORE about to be emitted at `memIndex` as eligible for
	/// memory-offset fusion when its address is a single-use `ptr + const`
	/// that is this block's immediate predecessor instruction — adjacency
	/// guarantees nothing could have reused the base pointer's register in
	/// between, and the single-use check (via usageCounts, checked before
	/// useValue() below decrements it) guarantees no other consumer still
	/// needs the add's result register once it is fused away.
	void recordFusableMemOp(int block, uint32_t memIndex, const ir::Operation* address) {
		if (!options.enableSuperinstructions || !options.enableImmediates || memIndex == 0) {
			return;
		}
		const auto* addOp = ir::dyn_cast<ir::AddOperation>(address);
		if (addOp == nullptr || addOp->getStamp() != Type::ptr) {
			return;
		}
		const auto* constInt = ir::dyn_cast<ir::ConstIntOperation>(addOp->getRightInput());
		if (constInt == nullptr) {
			return;
		}
		const int64_t value = constInt->getValue();
		if (value < -32768 || value > 32767) {
			return;
		}
		const auto siteIt = ptrAddSites.find(addOp->getIdentifier());
		if (siteIt == ptrAddSites.end() || siteIt->second.first != static_cast<uint32_t>(block) ||
		    siteIt->second.second != memIndex - 1) {
			return;
		}
		const auto usageIt = usageCounts.find(addOp->getIdentifier());
		if (usageIt == usageCounts.end() || usageIt->second != 1) {
			return;
		}
		blocks[block].fusableMemOps.emplace_back(memIndex, memIndex - 1);
	}

	void visitLoad(ir::LoadOperation* opt, int block, RegisterFrame& frame) {
		const auto address = frame.getValue(opt->getAddress()->getIdentifier());
		const auto codeIndex = static_cast<uint32_t>(blocks[block].code.size());
		recordFusableMemOp(block, codeIndex, opt->getAddress());
		useValue(opt->getAddress()->getIdentifier(), frame);
		const auto result = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), result);
		emit(block, typedOp(Op::LOAD_i8, memTypeIndex(opt->getStamp(), "load"), "load"), result, address);
	}

	void visitStore(ir::StoreOperation* opt, int block, RegisterFrame& frame) {
		const auto address = frame.getValue(opt->getAddress()->getIdentifier());
		const auto value = frame.getValue(opt->getValue()->getIdentifier());
		const auto codeIndex = static_cast<uint32_t>(blocks[block].code.size());
		recordFusableMemOp(block, codeIndex, opt->getAddress());
		useValue(opt->getAddress()->getIdentifier(), frame);
		useValue(opt->getValue()->getIdentifier(), frame);
		emit(block, typedOp(Op::STORE_i8, memTypeIndex(opt->getValue()->getStamp(), "store"), "store"), address, value);
	}

	void visitAlloca(ir::AllocaOperation* opt, int block, RegisterFrame& frame) {
		// The slot register was pinned in the prologue; the pointer itself is
		// computed at frame-push time. Rebind the identifier — unless it is
		// already bound as a downstream merge-block parameter, in which case
		// copy the pointer into the parameter's register (setValue is
		// emplace-only and would silently keep the old binding).
		const auto index = opt->getIndex();
		assert(index < functionAllocaSlots.size() && "AllocaOperation index out of range for function");
		const auto slotRegister = functionAllocaSlots[index];
		if (frame.contains(opt->getIdentifier())) {
			const auto target = frame.getValue(opt->getIdentifier());
			if (target != slotRegister) {
				emit(block, Op::MOV, target, slotRegister);
			}
		} else {
			frame.setValue(opt->getIdentifier(), slotRegister);
		}
	}

	void visitCast(ir::CastOperation* opt, int block, RegisterFrame& frame) {
		const auto input = frame.getValue(opt->getInput()->getIdentifier());
		useValue(opt->getInput()->getIdentifier(), frame);
		const auto result = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), result);

		const auto srcType = opt->getInput()->getStamp();
		const auto dstType = opt->getStamp();
		// Identity casts (two C++ types mapping to the same nautilus Type,
		// e.g. size_t and uint64_t on macOS) and ui64 <-> ptr are register
		// moves. Pointer-valued targets convert as ui64 (bc parity).
		if (srcType == dstType || (srcType == Type::ui64 && dstType == Type::ptr)) {
			emit(block, Op::MOV, result, input);
			return;
		}
		const int srcIdx = numTypeIndex(srcType == Type::ptr ? Type::ui64 : srcType);
		const int dstIdx = numTypeIndex(dstType == Type::ptr ? Type::ui64 : dstType);
		if (srcIdx < 0 || dstIdx < 0) {
			throw NotImplementedException("tbc: unsupported cast");
		}
		if (srcIdx == dstIdx) {
			emit(block, Op::MOV, result, input);
			return;
		}
		emit(block, static_cast<Op>(opIndex(Op::CAST_i8_i8) + srcIdx * 10 + dstIdx), result, input);
	}

	void visitSelect(ir::SelectOperation* opt, int block, RegisterFrame& frame) {
		const auto condition = frame.getValue(opt->getCondition()->getIdentifier());
		const auto trueValue = frame.getValue(opt->getTrueValue()->getIdentifier());
		const auto falseValue = frame.getValue(opt->getFalseValue()->getIdentifier());
		useValue(opt->getCondition()->getIdentifier(), frame);
		useValue(opt->getTrueValue()->getIdentifier(), frame);
		useValue(opt->getFalseValue()->getIdentifier(), frame);
		const auto result = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), result);
		// Slots are normalized by their producers, so SELECT is a typeless
		// 64-bit slot pick: word0 {dst, cond, trueReg}, word1 {falseReg}.
		emit(block, Op::SELECT, result, condition, trueValue);
		// Operand word: never dispatched, so it carries TRAP defensively.
		blocks[block].code.push_back(Instr {opIndex(Op::TRAP), falseValue, 0, 0});
	}

	// ── Control flow ─────────────────────────────────────────────────────────

	void processInvocation(const ir::BasicBlockInvocation& invocation, int block, RegisterFrame& parentFrame) {
		// Direct port of bc's block-invocation lowering (see
		// BCLoweringProvider.cpp for the full rationale): reads strictly
		// before writes; fresh targets get fresh registers; with coalescing,
		// already-bound targets are sequenced as a parallel copy.
		auto blockInputArguments = invocation.getArguments();
		auto& blockTargetArguments = invocation.getBlock()->getArguments();

		if (!options.enableRegisterAllocator || !options.enableRegisterCoalescing) {
			std::vector<uint16_t> tempArgs;
			tempArgs.reserve(blockInputArguments.size());
			for (std::size_t i = 0; i < blockInputArguments.size(); ++i) {
				auto* input = blockInputArguments[i];
				const auto sourceReg = parentFrame.getValue(input->getIdentifier());
				// A temp that will become the binding of a cross-block target
				// argument must be pinned (see `crossBlockIds`); a temp that
				// only stages into an already-bound target stays recyclable.
				const auto blockTargetArgument = blockTargetArguments[i]->getIdentifier();
				const bool becomesCrossBlockBinding =
				    !parentFrame.contains(blockTargetArgument) && crossBlockIds.contains(blockTargetArgument);
				const auto tempReg = becomesCrossBlockBinding ? registerProvider.allocPinnedRegister()
				                                              : registerProvider.allocFreshRegister();
				tempArgs.push_back(tempReg);
				emit(block, Op::MOV, tempReg, sourceReg);
				useValue(input->getIdentifier(), parentFrame);
			}
			for (std::size_t i = 0; i < blockInputArguments.size(); ++i) {
				const auto blockTargetArgument = blockTargetArguments[i]->getIdentifier();
				if (!parentFrame.contains(blockTargetArgument)) {
					parentFrame.setValue(blockTargetArgument, tempArgs[i]);
				} else {
					const auto targetReg = parentFrame.getValue(blockTargetArgument);
					if (targetReg != tempArgs[i]) {
						emit(block, Op::MOV, targetReg, tempArgs[i]);
					}
					if (options.enableRegisterAllocator) {
						registerProvider.freeRegister(tempArgs[i]);
					}
				}
			}
			return;
		}

		std::vector<std::pair<uint16_t, uint16_t>> boundPairs;
		std::vector<std::pair<ir::OperationIdentifier, uint16_t>> freshBindings;
		for (std::size_t i = 0; i < blockInputArguments.size(); ++i) {
			auto* input = blockInputArguments[i];
			const auto sourceReg = parentFrame.getValue(input->getIdentifier());
			const auto blockTargetArgument = blockTargetArguments[i]->getIdentifier();
			if (!parentFrame.contains(blockTargetArgument)) {
				// Cross-block target arguments get a pinned register (see
				// `crossBlockIds`); both alloc kinds bypass the free list.
				const auto freshReg = crossBlockIds.contains(blockTargetArgument)
				                          ? registerProvider.allocPinnedRegister()
				                          : registerProvider.allocFreshRegister();
				emit(block, Op::MOV, freshReg, sourceReg);
				freshBindings.emplace_back(blockTargetArgument, freshReg);
			} else {
				const auto targetReg = parentFrame.getValue(blockTargetArgument);
				if (targetReg != sourceReg) {
					boundPairs.emplace_back(targetReg, sourceReg);
				}
			}
			useValue(input->getIdentifier(), parentFrame);
		}
		for (auto& [identifier, reg] : freshBindings) {
			parentFrame.setValue(identifier, reg);
		}
		emitParallelCopy(block, std::move(boundPairs));
	}

	/// Classic parallel-copy sequentialization (port of bc's emitParallelCopy):
	/// emit any move whose destination is no longer needed as a source; break
	/// genuine cycles by stashing one destination in a fresh temp.
	void emitParallelCopy(int block, std::vector<std::pair<uint16_t, uint16_t>> pairs) {
		if (pairs.empty()) {
			return;
		}
		std::unordered_map<uint16_t, int> pendingReads;
		for (auto& p : pairs) {
			pendingReads[p.second]++;
		}
		std::vector<char> done(pairs.size(), 0);
		std::vector<uint16_t> cycleTemps;
		std::size_t remaining = pairs.size();
		while (remaining > 0) {
			bool progressed = false;
			for (std::size_t i = 0; i < pairs.size(); i++) {
				if (done[i]) {
					continue;
				}
				const auto [dst, src] = pairs[i];
				if (pendingReads[dst] == 0) {
					emit(block, Op::MOV, dst, src);
					done[i] = 1;
					remaining--;
					progressed = true;
					pendingReads[src]--;
				}
			}
			if (progressed || remaining == 0) {
				continue;
			}
			for (std::size_t i = 0; i < pairs.size(); i++) {
				if (done[i]) {
					continue;
				}
				const uint16_t dst = pairs[i].first;
				const uint16_t temp = registerProvider.allocFreshRegister();
				emit(block, Op::MOV, temp, dst);
				cycleTemps.push_back(temp);
				for (std::size_t j = 0; j < pairs.size(); j++) {
					if (!done[j] && pairs[j].second == dst) {
						pairs[j].second = temp;
					}
				}
				pendingReads[dst] = 0;
				break;
			}
		}
		for (const uint16_t temp : cycleTemps) {
			registerProvider.freeRegister(temp);
		}
	}

	void visitIf(ir::IfOperation* ifOpt, int block, RegisterFrame& frame) {
		const auto conditionalReg = frame.getValue(ifOpt->getValue()->getIdentifier());

		// Mark for compare-and-branch fusion when the condition is a
		// comparison used nowhere but this branch (checked before useValue so
		// the count still reflects the original number of uses).
		if (const auto* cmp = ir::dyn_cast<ir::CompareOperation>(ifOpt->getValue())) {
			const auto it = usageCounts.find(cmp->getIdentifier());
			if (it != usageCounts.end() && it->second == 1) {
				blocks[block].fuseCompareIntoBranch = true;
			}
		}
		useValue(ifOpt->getValue()->getIdentifier(), frame);

		// Route each arm through a landing-pad block so the two arms cannot
		// clobber shared merge-block parameter registers (see bc's visitIf for
		// the full ordering rationale).
		const int trueLandingPad = static_cast<int>(blocks.size());
		blocks.emplace_back();
		processInvocation(ifOpt->getTrueBlockInvocation(), trueLandingPad, frame);
		const int trueBlockIndex = processBlock(ifOpt->getTrueBlockInvocation().getBlock(), frame);
		blocks[trueLandingPad].term = Terminator {Terminator::Jump, kNoReg, trueBlockIndex, -1, kNoReg};

		const int falseLandingPad = static_cast<int>(blocks.size());
		blocks.emplace_back();
		processInvocation(ifOpt->getFalseBlockInvocation(), falseLandingPad, frame);
		const int falseBlockIndex = processBlock(ifOpt->getFalseBlockInvocation().getBlock(), frame);
		blocks[falseLandingPad].term = Terminator {Terminator::Jump, kNoReg, falseBlockIndex, -1, kNoReg};

		blocks[block].term = Terminator {Terminator::CondJump, conditionalReg, trueLandingPad, falseLandingPad, kNoReg};
	}

	void visitBranch(ir::BranchOperation* branchOp, int block, RegisterFrame& frame) {
		processInvocation(branchOp->getNextBlockInvocation(), block, frame);
		const int blockIndex = processBlock(branchOp->getNextBlockInvocation().getBlock(), frame);
		blocks[block].term = Terminator {Terminator::Jump, kNoReg, blockIndex, -1, kNoReg};
	}

	void visitReturn(ir::ReturnOperation* returnOpt, int block, RegisterFrame& frame) {
		if (returnOpt->hasReturnValue()) {
			const auto returnReg = frame.getValue(returnOpt->getReturnValue()->getIdentifier());
			useValue(returnOpt->getReturnValue()->getIdentifier(), frame);
			blocks[block].term = Terminator {Terminator::Ret, kNoReg, -1, -1, returnReg};
		} else {
			blocks[block].term = Terminator {Terminator::Ret, kNoReg, -1, -1, kNoReg};
		}
	}

	// ── Calls ────────────────────────────────────────────────────────────────

	uint16_t addCallSite(CallSite&& site) {
		if (program.callsites.size() >= kNoReg) {
			throw RuntimeException("tbc: too many call sites in program");
		}
		program.callsites.emplace_back(std::move(site));
		return static_cast<uint16_t>(program.callsites.size() - 1);
	}

	CallSite buildCallSite(std::span<ir::Operation* const> arguments, Type returnType, RegisterFrame& frame) {
		CallSite site;
		site.returnType = returnType;
		site.argTypes.reserve(arguments.size());
		site.argRegs.reserve(arguments.size());
		for (auto* arg : arguments) {
			site.argTypes.push_back(arg->getStamp());
			site.argRegs.push_back(frame.getValue(arg->getIdentifier()));
			useValue(arg->getIdentifier(), frame);
		}
		return site;
	}

	void visitProxyCall(ir::ProxyCallOperation* opt, int block, RegisterFrame& frame) {
		auto site = buildCallSite(opt->getInputArguments(), opt->getStamp(), frame);

		uint16_t dst = kNoReg;
		if (opt->getStamp() != Type::v) {
			dst = getResultRegister(opt, frame);
			frame.setValue(opt->getIdentifier(), dst);
		}

		// Calls to functions in the same module run interpreter-native: no FFI,
		// just a frame push in the shared dispatch loop.
		const auto it = program.functionIndex.find(opt->getFunctionName());
		if (it != program.functionIndex.end()) {
			site.internalFnIdx = it->second;
			const auto siteIdx = addCallSite(std::move(site));
			emit(block, Op::CALL, dst, static_cast<uint16_t>(it->second), siteIdx);
			return;
		}
		site.target = opt->getFunctionPtr();
		const auto siteIdx = addCallSite(std::move(site));
		emit(block, Op::CALL_EXT, dst, siteIdx);
	}

	void visitIndirectCall(ir::IndirectCallOperation* opt, int block, RegisterFrame& frame) {
		auto site = buildCallSite(opt->getInputArguments(), opt->getStamp(), frame);
		const auto functionReg = frame.getValue(opt->getFunctionPtrOperand()->getIdentifier());
		useValue(opt->getFunctionPtrOperand()->getIdentifier(), frame);

		uint16_t dst = kNoReg;
		if (opt->getStamp() != Type::v) {
			dst = getResultRegister(opt, frame);
			frame.setValue(opt->getIdentifier(), dst);
		}
		const auto siteIdx = addCallSite(std::move(site));
		emit(block, Op::CALL_IND, dst, functionReg, siteIdx);
	}

	void visitFunctionAddressOf(ir::FunctionAddressOfOperation* opt, int block, RegisterFrame& frame) {
		// Internal functions have no machine code; hand out a pre-compiled
		// trampoline bound to the function, which is a genuine native pointer
		// usable both by CALL_IND and by external code (no runtime code
		// generation involved — iOS-safe).
		uint64_t value;
		const auto it = program.functionIndex.find(opt->getFunctionName());
		if (it != program.functionIndex.end()) {
			value = reinterpret_cast<uint64_t>(acquireTrampoline(&program, it->second));
		} else {
			value = reinterpret_cast<uint64_t>(opt->getFunctionPtr());
		}
		const auto target = getResultRegister(opt, frame);
		frame.setValue(opt->getIdentifier(), target);
		emit(block, Op::MOV, target, constSlot(value));
	}

	// ── Flattening ───────────────────────────────────────────────────────────

	bool shouldFuse(const BlockCode& blk) const {
		if (!options.enableSuperinstructions || blk.term.kind != Terminator::CondJump || !blk.fuseCompareIntoBranch ||
		    blk.code.empty()) {
			return false;
		}
		const Instr& last = blk.code.back();
		return last.a == blk.term.condReg && fusedFormOf(static_cast<Op>(last.op)) != Op::COUNT;
	}

	void finalize() {
		const auto regCount = registerProvider.getRegisterCount();
		out.regSlots = regCount;
		out.initImage.assign(regCount, 0);
		for (const auto& [reg, value] : constInits) {
			out.initImage[reg] = value;
		}
		// +2 slack slots so the alloca area can be 16-aligned at runtime.
		const uint32_t allocaSlots = out.allocaBytes > 0 ? (out.allocaBytes + 7) / 8 + 2 : 0;
		out.frameSlots = 3 + regCount + allocaSlots;

		// Fold recorded immediates into the arithmetic ops. A folded constant
		// that was materialized by its own now-unused MOV_imm drops that
		// instruction too.
		if (options.enableImmediates) {
			for (auto& blk : blocks) {
				for (const auto& imm : blk.foldableImmediates) {
					Instr& inst = blk.code[imm.codeIndex];
					const Op immOp = immFormOf(static_cast<Op>(inst.op));
					if (immOp != Op::COUNT) {
						inst.op = opIndex(immOp);
						inst.c = static_cast<uint16_t>(imm.immediate);
						if (imm.hasDeadMovSite) {
							blocks[imm.deadMovBlock].deadIndices.insert(imm.deadMovIndex);
						}
					}
				}
			}
		}

		// Fuse a single-use `ptr + const` immediately preceding a LOAD/STORE
		// into LOAD_off/STORE_off, once the add's own immediate has been
		// folded above (memory-offset superinstructions).
		if (options.enableSuperinstructions && options.enableImmediates) {
			for (auto& blk : blocks) {
				for (const auto& [memIndex, addIndex] : blk.fusableMemOps) {
					const Instr& addInst = blk.code[addIndex];
					if (static_cast<Op>(addInst.op) != Op::ADD_imm_i64) {
						continue; // the add's constant didn't end up folded; leave both instructions intact
					}
					Instr& memInst = blk.code[memIndex];
					const Op offOp = memOffsetFormOf(static_cast<Op>(memInst.op));
					if (offOp == Op::COUNT) {
						continue;
					}
					const bool isLoad = opIndex(static_cast<Op>(memInst.op)) <= opIndex(Op::LOAD_b);
					if (isLoad) {
						memInst.b = addInst.b;
					} else {
						memInst.a = addInst.b;
					}
					memInst.c = addInst.c;
					memInst.op = opIndex(offOp);
					blk.deadIndices.insert(addIndex);
				}
			}
		}

		// Flatten the block-structured code into one contiguous stream:
		// terminators become JMP/CJMP/RET, jumps to the next block in emission
		// order fall through, and fusable trailing compares merge into 2-word
		// compare-and-branch instructions. Branch targets are patched once all
		// block start offsets are known.
		struct Fixup {
			uint32_t at;         // instruction word the branch executes from
			uint32_t offsetWord; // word holding the packed offset
			bool loHi;           // packed in fields a|b (2-word forms) instead of b|c
			int targetBlock;
		};
		std::vector<uint32_t> blockStart(blocks.size(), 0);
		std::vector<Fixup> fixups;
		auto& codeOut = out.code;

		for (std::size_t b = 0; b < blocks.size(); ++b) {
			blockStart[b] = static_cast<uint32_t>(codeOut.size());
			auto& blk = blocks[b];
			const bool fuse = shouldFuse(blk);
			const std::size_t bodyCount = blk.code.size() - (fuse ? 1 : 0);
			for (std::size_t i = 0; i < bodyCount; ++i) {
				if (blk.deadIndices.count(static_cast<uint32_t>(i)) > 0) {
					continue;
				}
				codeOut.push_back(blk.code[i]);
			}

			const int next = static_cast<int>(b) + 1;
			switch (blk.term.kind) {
			case Terminator::Ret:
				codeOut.push_back(Instr {opIndex(Op::RET), blk.term.retReg, 0, 0});
				break;
			case Terminator::Jump:
				if (blk.term.trueBlock != next) {
					const auto at = static_cast<uint32_t>(codeOut.size());
					codeOut.push_back(Instr {opIndex(Op::JMP), 0, 0, 0});
					fixups.push_back({at, at, false, blk.term.trueBlock});
				}
				break;
			case Terminator::CondJump: {
				if (fuse) {
					const Instr& cmp = blk.code.back();
					const Op fusedOp = fusedFormOf(static_cast<Op>(cmp.op));
					const auto at = static_cast<uint32_t>(codeOut.size());
					codeOut.push_back(Instr {opIndex(fusedOp), cmp.b, cmp.c, 0});
					codeOut.push_back(Instr {opIndex(Op::TRAP), 0, 0, 0});
					fixups.push_back({at, at + 1, true, blk.term.trueBlock});
				} else {
					const auto at = static_cast<uint32_t>(codeOut.size());
					codeOut.push_back(Instr {opIndex(Op::CJMP), blk.term.condReg, 0, 0});
					fixups.push_back({at, at, false, blk.term.trueBlock});
				}
				if (blk.term.falseBlock != next) {
					const auto at = static_cast<uint32_t>(codeOut.size());
					codeOut.push_back(Instr {opIndex(Op::JMP), 0, 0, 0});
					fixups.push_back({at, at, false, blk.term.falseBlock});
				}
				break;
			}
			case Terminator::None:
				throw RuntimeException("tbc: block without terminator");
			}
		}

		for (const auto& fixup : fixups) {
			const auto offset = static_cast<int32_t>(blockStart[fixup.targetBlock]) - static_cast<int32_t>(fixup.at);
			const auto packed = static_cast<uint32_t>(offset);
			Instr& word = codeOut[fixup.offsetWord];
			if (fixup.loHi) {
				word.a = static_cast<uint16_t>(packed & 0xFFFF);
				word.b = static_cast<uint16_t>(packed >> 16);
			} else {
				word.b = static_cast<uint16_t>(packed & 0xFFFF);
				word.c = static_cast<uint16_t>(packed >> 16);
			}
		}
	}
};

} // namespace

void TBCLoweringProvider::lower(const ir::FunctionOperation* func, TBCProgram& program, TBCFunction& out,
                                const TBCLoweringOptions& options) {
	out.name = func->getName();
	LoweringContext context(func, program, out, options);
	context.process();
}

} // namespace nautilus::compiler::tbc
