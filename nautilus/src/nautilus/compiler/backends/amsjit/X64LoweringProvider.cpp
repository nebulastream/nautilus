
#include "nautilus/compiler/backends/amsjit/X64LoweringProvider.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/ir/Usages.hpp"
#include "nautilus/compiler/ir/operations/DestructorOperands.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <algorithm>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;
using namespace ::asmjit::x86;

// ── AsmJitLoweringProvider ────────────────────────────────────────────────────

namespace {
// Custom error handler that throws instead of aborting.
class ThrowOnError : public ::asmjit::ErrorHandler {
public:
	void handleError(::asmjit::Error err, const char* message, ::asmjit::BaseEmitter* /*origin*/) override {
		fprintf(stderr, "[AsmJit] Error %u: %s\n", err, message ? message : "(null)");
		throw std::runtime_error(std::string("AsmJit error: ") + (message ? message : "unknown"));
	}
};
} // anonymous namespace

AsmJitLoweringProvider::LowerResult AsmJitLoweringProvider::lower(std::shared_ptr<ir::IRGraph> ir,
                                                                  ::asmjit::JitRuntime& runtime,
                                                                  const engine::Options& options,
                                                                  const DumpHandler& dumpHandler,
                                                                  CompilationStatistics* statistics) {
	CodeHolder code;
	code.init(runtime.environment(), runtime.cpuFeatures());
	ThrowOnError errHandler;
	code.setErrorHandler(&errHandler);

	// Only pay the formatting/logging cost when the corresponding dump is requested.
	const bool dumpAsmjitIR = dumpHandler.shouldDump("after_asmjit_generation");
	const bool dumpAssembly = dumpHandler.shouldDump("after_asmjit_assembly");

	// When requested, attach a StringLogger so finalize() records the emitted assembly.
	StringLogger asmLogger;
	if (dumpAssembly) {
		code.setLogger(&asmLogger);
	}

	// Build the intrinsic manager from the global plugin registry. Gated by
	// `asmjit.enableIntrinsics` (default true), mirroring `mlir.enableIntrinsics`.
	// When disabled, the manager stays empty and every Call falls through
	// to the regular scalar invoke path.
	AsmJitIntrinsicManager intrinsicManager;
	if (options.getOptionOrDefault<bool>("asmjit.enableIntrinsics", true)) {
		AsmJitIntrinsicPluginRegistry::instance().registerAllIntrinsics(intrinsicManager);
	}

	LoweringContext ctx(std::move(ir), code, options, statistics, intrinsicManager);
	std::string asmjitIR;
	ctx.processAll(dumpAsmjitIR ? &asmjitIR : nullptr);

	// Capture label offsets while code is still in the CodeHolder (before rt.add resets it).
	std::unordered_map<std::string, uint64_t> offsets;
	for (const auto& [name, funcNode] : ctx.getFuncNodes()) {
		offsets[name] = code.labelOffset(funcNode->label());
	}
	// Capture size before rt.add — CodeHolder is consumed by add().
	const uint64_t codeSize = code.codeSize();

	void* basePtr = nullptr;
	if (runtime.add(&basePtr, &code)) {
		return {};
	}

	LowerResult result;
	result.basePtr = basePtr;
	result.codeSize = codeSize;
	for (const auto& [name, offset] : offsets) {
		result.jitPtrs[name] = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(basePtr) + offset);
	}
	result.asmjitIR = std::move(asmjitIR);
	if (dumpAssembly) {
		result.assembly.assign(asmLogger.data(), asmLogger.dataSize());
	}
	return result;
}

// ── LoweringContext construction ──────────────────────────────────────────────

AsmJitLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir, CodeHolder& code,
                                                         const engine::Options& options,
                                                         CompilationStatistics* statistics,
                                                         const AsmJitIntrinsicManager& intrinsicManager)
    : cc(&code), ir(std::move(ir)), intrinsicManager_(intrinsicManager), statistics_(statistics) {
	// Register the optional post-RA peephole pass. It appends to the
	// Compiler's pass list *after* X86RAPass (which was installed during
	// `cc(&code)` via x86::Compiler::onAttach), so it sees physical-register
	// operands. Gated by an option so it can be toggled off for benchmarks
	// and regression investigations.
	if (options.getOptionOrDefault<bool>("asmjit.enablePostRAPeephole", true)) {
		cc.addPassT<X64PostRAPeepholePass>(statistics);
	}
	enableBranchFusion_ = options.getOptionOrDefault<bool>("asmjit.enableBranchFusion", true);
	enableConstFolding_ = options.getOptionOrDefault<bool>("asmjit.enableConstFolding", true);
	enableSelectCmov_ = options.getOptionOrDefault<bool>("asmjit.enableSelectCmov", true);
	enableLazyNarrowing_ = options.getOptionOrDefault<bool>("asmjit.enableLazyNarrowing", true);
	enableAddressFusion_ = options.getOptionOrDefault<bool>("asmjit.enableAddressFusion", true);
	enableInlining_ = options.getOptionOrDefault<bool>("asmjit.enableInlining", true);
	enableLoopRotation_ = options.getOptionOrDefault<bool>("asmjit.enableLoopRotation", true);
	inliningMaxOperations_ = static_cast<size_t>(options.getOptionOrDefault<int>("asmjit.inliningMaxOperations", 24));
}

// ── Type helpers ──────────────────────────────────────────────────────────────

TypeId AsmJitLoweringProvider::LoweringContext::getTypeId(Type t) {
	switch (t) {
	case Type::b:
		return TypeId::kUInt8;
	case Type::i8:
		return TypeId::kInt8;
	case Type::i16:
		return TypeId::kInt16;
	case Type::i32:
		return TypeId::kInt32;
	case Type::i64:
		return TypeId::kInt64;
	case Type::ui8:
		return TypeId::kUInt8;
	case Type::ui16:
		return TypeId::kUInt16;
	case Type::ui32:
		return TypeId::kUInt32;
	case Type::ui64:
		return TypeId::kUInt64;
	case Type::ptr:
		return TypeId::kUIntPtr;
	case Type::f32:
		return TypeId::kFloat32;
	case Type::f64:
		return TypeId::kFloat64;
	case Type::v:
		return TypeId::kVoid;
	default:
		// Fail loudly: silently defaulting to kInt64 miscompiles (wrong
		// width/signedness) if the Type enum grows without this switch.
		throw NotImplementedException("getTypeId: unsupported Type");
	}
}

bool AsmJitLoweringProvider::LoweringContext::isFloatType(Type t) {
	return t == Type::f32 || t == Type::f64;
}

bool AsmJitLoweringProvider::LoweringContext::isUnsignedType(Type t) {
	return t == Type::ui8 || t == Type::ui16 || t == Type::ui32 || t == Type::ui64 || t == Type::b || t == Type::ptr;
}

void AsmJitLoweringProvider::LoweringContext::narrowToStamp(Gp reg, Type stamp) {
	switch (stamp) {
	case Type::i8:
		cc.movsx(reg.r64(), reg.r8());
		break;
	case Type::b:
	case Type::ui8:
		cc.movzx(reg.r32(), reg.r8());
		break;
	case Type::i16:
		cc.movsx(reg.r64(), reg.r16());
		break;
	case Type::ui16:
		cc.movzx(reg.r32(), reg.r16());
		break;
	case Type::i32:
		cc.movsxd(reg.r64(), reg.r32());
		break;
	case Type::ui32:
		cc.mov(reg.r32(), reg.r32());
		break; // zero-extends to 64
	default:
		break; // i64, ui64, ptr -- already full-width, no narrowing needed.
	}
}

// ── Lazy narrowing ────────────────────────────────────────────────────────────
// Narrow integer values are canonical (sign/zero-extended to 64 bits per stamp)
// unless mayBeDirty_ says otherwise. A dirty value's low stampBits() bits are
// exact; the bits above are garbage. Operations whose result's low bits depend
// only on the low bits of their inputs (add/sub/mul/not/shl/and/or/xor/select)
// accept dirty inputs at least as wide as their result and produce dirty
// results. Everything that reads the full register asks for a clean operand.

uint32_t AsmJitLoweringProvider::LoweringContext::stampBits(Type t) {
	switch (t) {
	case Type::i8:
	case Type::ui8:
		return 8;
	case Type::i16:
	case Type::ui16:
		return 16;
	case Type::i32:
	case Type::ui32:
		return 32;
	default:
		return 64;
	}
}

void AsmJitLoweringProvider::LoweringContext::computeDirtyValues(const ir::FunctionOperation* funcOp) {
	mayBeDirty_.clear();
	if (!enableLazyNarrowing_) {
		return;
	}
	const auto mark = [&](const ir::Operation* op) {
		if (stampBits(op->getStamp()) == 64) {
			return;
		}
		const auto id = op->getIdentifier().getId();
		if (id >= mayBeDirty_.size()) {
			mayBeDirty_.resize(id + 1, 0);
		}
		mayBeDirty_[id] = 1;
	};
	const auto* entryBlock = &funcOp->getFunctionBasicBlock();
	for (const auto* block : funcOp->getBasicBlocks()) {
		// A block parameter merges values from every incoming edge, any of which
		// may be dirty. Function arguments are extended in the prologue.
		if (block != entryBlock) {
			for (const auto* arg : block->getArguments()) {
				mark(arg);
			}
		}
		for (const auto* op : block->getOperations()) {
			const auto* shift = ir::dyn_cast<ir::ShiftOperation>(op);
			if (ir::dyn_cast<ir::AddOperation>(op) != nullptr || ir::dyn_cast<ir::SubOperation>(op) != nullptr ||
			    ir::dyn_cast<ir::MulOperation>(op) != nullptr || ir::dyn_cast<ir::NegateOperation>(op) != nullptr ||
			    ir::dyn_cast<ir::BinaryCompOperation>(op) != nullptr ||
			    ir::dyn_cast<ir::SelectOperation>(op) != nullptr ||
			    (shift != nullptr && shift->getType() == ir::ShiftOperation::LS)) {
				mark(op);
			}
		}
	}
}

bool AsmJitLoweringProvider::LoweringContext::isDirty(const ir::Operation* in) const {
	const auto id = in->getIdentifier().getId();
	return id < mayBeDirty_.size() && mayBeDirty_[id] != 0;
}

void AsmJitLoweringProvider::LoweringContext::extendFromStamp(Gp dst, Gp src, Type stamp) {
	switch (stamp) {
	case Type::i8:
		cc.movsx(dst.r64(), src.r8());
		break;
	case Type::b:
	case Type::ui8:
		cc.movzx(dst.r32(), src.r8());
		break;
	case Type::i16:
		cc.movsx(dst.r64(), src.r16());
		break;
	case Type::ui16:
		cc.movzx(dst.r32(), src.r16());
		break;
	case Type::i32:
		cc.movsxd(dst.r64(), src.r32());
		break;
	case Type::ui32:
		cc.mov(dst.r32(), src.r32());
		break;
	default:
		cc.mov(dst, src);
		break;
	}
}

Gp AsmJitLoweringProvider::LoweringContext::gpOperandAtWidth(const ir::Operation* in, uint32_t width,
                                                             RegisterFrame& frame) {
	auto reg = gpOperand(in, frame);
	// A rematerialised constant is always canonical.
	if (stampBits(in->getStamp()) < width && isDirty(in) && !(enableConstFolding_ && foldableConstValue(in))) {
		auto extended = cc.newInt64();
		extendFromStamp(extended, reg, in->getStamp());
		return extended;
	}
	return reg;
}

void AsmJitLoweringProvider::LoweringContext::narrowResult(Gp reg, Type stamp) {
	if (!enableLazyNarrowing_) {
		narrowToStamp(reg, stamp);
	}
}

// ── Address-mode fusion ───────────────────────────────────────────────────────
// Folding an add into its consumers' memory operands re-reads the add's inputs
// at each consumer. That is only sound while those registers still hold the
// values they held at the add, so every consumer must sit in the add's own
// block: a register only changes inside a block when its own identifier is
// (re)defined, while block-argument copies on the edges out of a block can
// overwrite registers that identifiers shared with merge parameters live in
// (issue #321).

namespace {
// log2(@p value) for 1, 2, 4 and 8 -- the scales of an x86 address.
std::optional<uint32_t> addressScaleShift(int64_t value) {
	switch (value) {
	case 1:
		return 0;
	case 2:
		return 1;
	case 4:
		return 2;
	case 8:
		return 3;
	default:
		return std::nullopt;
	}
}
} // anonymous namespace

std::optional<AsmJitLoweringProvider::LoweringContext::AddressParts>
AsmJitLoweringProvider::LoweringContext::matchAddress(const ir::Operation* op) {
	if (!enableAddressFusion_) {
		return std::nullopt;
	}
	const auto* add = ir::dyn_cast<ir::AddOperation>(op);
	if (add == nullptr) {
		return std::nullopt;
	}
	const Type stamp = add->getStamp();
	if (stamp != Type::ptr && stamp != Type::i64 && stamp != Type::ui64) {
		return std::nullopt;
	}
	const ir::Operation* base = add->getLeftInput();
	const ir::Operation* offset = add->getRightInput();
	// The base is the pointer operand of a pointer add.
	if (stamp == Type::ptr && base->getStamp() != Type::ptr) {
		std::swap(base, offset);
	}
	const auto isFullWidthInteger = [](const ir::Operation* value) {
		return stampBits(value->getStamp()) == 64 && !isFloatType(value->getStamp());
	};
	if (!isFullWidthInteger(base) || !isFullWidthInteger(offset)) {
		return std::nullopt;
	}

	// x * 2^k or x << k, with k <= 3; the scaled input must be a full-width
	// register value (a constant would have been folded by the IR passes).
	const auto scaledOf =
	    [&](const ir::Operation* candidate) -> std::optional<std::pair<const ir::Operation*, uint32_t>> {
		if (const auto* mul = ir::dyn_cast<ir::MulOperation>(candidate)) {
			for (int side = 0; side < 2; side++) {
				const auto* x = side == 0 ? mul->getLeftInput() : mul->getRightInput();
				const auto* c = side == 0 ? mul->getRightInput() : mul->getLeftInput();
				const auto value = imm32Operand(c);
				if (!value.has_value() || imm32Operand(x).has_value() || !isFullWidthInteger(x)) {
					continue;
				}
				if (const auto shift = addressScaleShift(*value)) {
					return std::make_pair(x, *shift);
				}
			}
		} else if (const auto* shl = ir::dyn_cast<ir::ShiftOperation>(candidate)) {
			const auto count = imm32Operand(shl->getRightInput());
			const auto* x = shl->getLeftInput();
			if (shl->getType() == ir::ShiftOperation::LS && count.has_value() && *count >= 0 && *count <= 3 &&
			    !imm32Operand(x).has_value() && isFullWidthInteger(x)) {
				return std::make_pair(x, static_cast<uint32_t>(*count));
			}
		}
		return std::nullopt;
	};

	AddressParts parts;
	if (const auto disp = imm32Operand(offset)) {
		if (imm32Operand(base).has_value()) {
			return std::nullopt;
		}
		parts.base = base;
		parts.disp = *disp;
		return parts;
	}
	// For an integer add either side may be the scaled one.
	if (stamp != Type::ptr && !scaledOf(offset).has_value() && scaledOf(base).has_value()) {
		std::swap(base, offset);
	}
	parts.base = base;
	parts.offset = offset;
	if (const auto scaled = scaledOf(offset)) {
		parts.scaledInput = scaled->first;
		parts.shift = scaled->second;
	}
	return parts;
}

void AsmJitLoweringProvider::LoweringContext::computeFusionUses(const ir::FunctionOperation* funcOp) {
	fusionUses_.clear();
	if (!enableAddressFusion_) {
		return;
	}
	std::vector<const ir::BasicBlock*> blocks(funcOp->getBasicBlocks().begin(), funcOp->getBasicBlocks().end());
	if (funcOp->exceptionRegion.has_value()) {
		for (const auto& pad : funcOp->exceptionRegion->pads) {
			blocks.push_back(pad.block);
		}
	}
	// Defining block per operation id.
	std::vector<const ir::BasicBlock*> defBlock;
	const auto growTo = [&](uint32_t id) {
		if (id >= defBlock.size()) {
			defBlock.resize(id + 1, nullptr);
			fusionUses_.resize(id + 1, 0x3);
		}
	};
	for (const auto* block : blocks) {
		for (const auto* op : block->getOperations()) {
			growTo(op->getIdentifier().getId());
			defBlock[op->getIdentifier().getId()] = block;
		}
	}
	const auto use = [&](const ir::Operation* in, const ir::BasicBlock* block, uint8_t allowedBits) {
		const auto id = in->getIdentifier().getId();
		growTo(id);
		if (defBlock[id] != block) {
			allowedBits = 0;
		}
		fusionUses_[id] &= allowedBits;
	};
	for (const auto* block : blocks) {
		for (const auto* op : block->getOperations()) {
			const auto* load = ir::dyn_cast<ir::LoadOperation>(op);
			const auto* store = ir::dyn_cast<ir::StoreOperation>(op);
			const auto parts = ir::dyn_cast<ir::AddOperation>(op) != nullptr ? matchAddress(op) : std::nullopt;
			for (const auto* in : op->getInputs()) {
				uint8_t allowed = 0;
				if ((load != nullptr && in == load->getAddress()) ||
				    (store != nullptr && in == store->getAddress() && in != store->getValue())) {
					allowed |= 0x1;
				}
				if (parts.has_value() && parts->scaledInput != nullptr && in == parts->offset) {
					allowed |= 0x2;
				}
				use(in, block, allowed);
			}
			for (size_t i = 0; i < ir::getDestructorOperandCount(*op); i++) {
				use(ir::getDestructorOperand(*op, i), block, 0);
			}
			if (const auto* ifOp = ir::dyn_cast<ir::IfOperation>(op)) {
				for (const auto* in : ifOp->getTrueBlockInvocation().getArguments()) {
					use(in, block, 0);
				}
				for (const auto* in : ifOp->getFalseBlockInvocation().getArguments()) {
					use(in, block, 0);
				}
			} else if (const auto* br = ir::dyn_cast<ir::BranchOperation>(op)) {
				for (const auto* in : br->getNextBlockInvocation().getArguments()) {
					use(in, block, 0);
				}
			}
		}
	}
}

bool AsmJitLoweringProvider::LoweringContext::hasFusionUse(const ir::Operation* op, uint8_t bit) const {
	const auto id = op->getIdentifier().getId();
	return id < fusionUses_.size() && (fusionUses_[id] & bit) != 0;
}

bool AsmJitLoweringProvider::LoweringContext::isDeferredAddressPart(const ir::Operation* op) const {
	const auto id = op->getIdentifier().getId();
	return id < deferredAddressParts_.size() && deferredAddressParts_[id] != 0;
}

void AsmJitLoweringProvider::LoweringContext::markDeferredAddressPart(const ir::Operation* op) {
	const auto id = op->getIdentifier().getId();
	if (id >= deferredAddressParts_.size()) {
		deferredAddressParts_.resize(id + 1, 0);
	}
	deferredAddressParts_[id] = 1;
}

bool AsmJitLoweringProvider::LoweringContext::deferAddressPart(const ir::Operation* op, uint8_t useBit,
                                                               RegisterFrame& frame) {
	// A bound identifier doubles as a merge-block parameter register that must
	// be written (issue #321).
	if (!hasFusionUse(op, useBit) || frame.contains(op->getIdentifier())) {
		return false;
	}
	markDeferredAddressPart(op);
	return true;
}

Mem AsmJitLoweringProvider::LoweringContext::memFromParts(const AddressParts& parts, RegisterFrame& frame) {
	auto base = gpOperand(parts.base, frame);
	if (parts.offset == nullptr) {
		return x86::ptr(base, parts.disp);
	}
	if (parts.scaledInput != nullptr && isDeferredAddressPart(parts.offset)) {
		return x86::ptr(base, gpOperand(parts.scaledInput, frame), parts.shift, parts.disp);
	}
	return x86::ptr(base, gpOperand(parts.offset, frame), 0, parts.disp);
}

Mem AsmJitLoweringProvider::LoweringContext::memOperand(const ir::Operation* addr, uint32_t size,
                                                        RegisterFrame& frame) {
	Mem mem;
	if (isDeferredAddressPart(addr)) {
		mem = memFromParts(*matchAddress(addr), frame);
		fusedAddresses_++;
	} else {
		mem = x86::ptr(gpOperand(addr, frame));
	}
	mem.setSize(size);
	return mem;
}

// ── Inlining ──────────────────────────────────────────────────────────────────
// A small leaf callee (e.g. a NautilusFunction wrapping one expression) costs
// far more in call overhead -- argument shuffling, the call/ret pair, and the
// caller's values forced into callee-saved registers -- than its body. Such a
// body is lowered straight into the caller: the callee's parameters are bound
// to the call's argument registers in a fresh frame (callee identifiers are a
// separate namespace), and the returned value becomes the call's result. The
// callee is still compiled on its own for other callers and exports.

bool AsmJitLoweringProvider::LoweringContext::isInlinable(const ir::FunctionOperation* callee) const {
	// No exception-region check needed: without calls nothing in the body can
	// throw (the region of a call-free function has no call sites).
	if (callee == nullptr || callee == currentFunction_ || callee->getBasicBlocks().size() != 1 ||
	    !callee->getAllocaSpecs().empty()) {
		return false;
	}
	const auto& ops = callee->getFunctionBasicBlock().getOperations();
	if (ops.empty() || ops.size() > inliningMaxOperations_ ||
	    ir::dyn_cast<ir::ReturnOperation>(ops.back()) == nullptr) {
		return false;
	}
	for (size_t i = 0; i + 1 < ops.size(); i++) {
		const auto* op = ops[i];
		const bool plainValueOp =
		    ir::dyn_cast<ir::ConstIntOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::ConstBooleanOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::ConstFloatOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::ConstPtrOperation>(op) != nullptr || ir::dyn_cast<ir::AddOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::SubOperation>(op) != nullptr || ir::dyn_cast<ir::MulOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::DivOperation>(op) != nullptr || ir::dyn_cast<ir::ModOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::CompareOperation>(op) != nullptr || ir::dyn_cast<ir::AndOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::OrOperation>(op) != nullptr || ir::dyn_cast<ir::NotOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::NegateOperation>(op) != nullptr || ir::dyn_cast<ir::ShiftOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::BinaryCompOperation>(op) != nullptr || ir::dyn_cast<ir::SelectOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::LoadOperation>(op) != nullptr || ir::dyn_cast<ir::StoreOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::CastOperation>(op) != nullptr;
		if (!plainValueOp) {
			return false;
		}
	}
	return true;
}

bool AsmJitLoweringProvider::LoweringContext::tryInlineCall(ir::CallOperation* op, RegisterFrame& frame) {
	if (!enableInlining_ || !funcNodes_.contains(op->getCalleeId()) || callNeedsCapture(op)) {
		return false;
	}
	const auto* callee = ir->getFunctionTable().get(op->getCalleeId()).getDefinition();
	if (!isInlinable(callee)) {
		return false;
	}
	const auto& params = callee->getFunctionBasicBlock().getArguments();
	const auto args = op->getInputArguments();
	if (params.size() != args.size()) {
		return false;
	}

	// Parameters arrive canonical, exactly as through the callee's prologue.
	RegisterFrame calleeFrame;
	for (size_t i = 0; i < params.size(); i++) {
		const AsmReg value =
		    isFloatType(args[i]->getStamp()) ? regOperand(args[i], frame) : AsmReg(cleanGpOperand(args[i], frame));
		calleeFrame.setValue(params[i]->getIdentifier(), value);
	}

	// The per-function analyses are keyed by operation id: swap in the
	// callee's for the duration of its body.
	auto savedDirty = std::move(mayBeDirty_);
	auto savedFusionUses = std::move(fusionUses_);
	auto savedDeferred = std::move(deferredAddressParts_);
	auto savedUsageCounts = std::move(usageCounts_);
	computeDirtyValues(callee);
	computeFusionUses(callee);
	deferredAddressParts_.clear();
	usageCounts_.clear();

	const auto& ops = callee->getFunctionBasicBlock().getOperations();
	for (size_t i = 0; i + 1 < ops.size(); i++) {
		dispatch(ops[i], calleeFrame);
	}
	std::optional<AsmReg> result;
	const auto* ret = ir::cast<ir::ReturnOperation>(ops.back());
	if (op->getStamp() != Type::v && ret->hasReturnValue()) {
		// The callee's return narrows the value to its stamp.
		auto value = regOperand(ret->getReturnValue(), calleeFrame);
		result = allocReg(op->getStamp());
		if (std::holds_alternative<Gp>(value)) {
			extendFromStamp(toGp(*result), toGp(value), ret->getReturnValue()->getStamp());
		} else {
			emitMove(*result, value);
		}
	}

	mayBeDirty_ = std::move(savedDirty);
	fusionUses_ = std::move(savedFusionUses);
	deferredAddressParts_ = std::move(savedDeferred);
	usageCounts_ = std::move(savedUsageCounts);

	if (result.has_value()) {
		bindResult(op->getIdentifier(), *result, frame);
	}
	inlinedCalls_++;
	return true;
}

// ── Register allocation ───────────────────────────────────────────────────────
// All integer/bool/ptr types are represented as 64-bit GP registers.
// This avoids size-mismatch issues when combining values across operations,
// at the cost of slightly over-wide arithmetic (acceptable for query compilation).

AsmJitLoweringProvider::AsmReg AsmJitLoweringProvider::LoweringContext::allocReg(Type t) {
	if (t == Type::f32)
		return cc.newXmmSs();
	if (t == Type::f64)
		return cc.newXmmSd();
	return cc.newInt64();
}

Gp AsmJitLoweringProvider::LoweringContext::toGp(const AsmReg& r) {
	return std::get<Gp>(r);
}

Xmm AsmJitLoweringProvider::LoweringContext::toXmm(const AsmReg& r) {
	return std::get<Xmm>(r);
}

// ── Label management ──────────────────────────────────────────────────────────

Label AsmJitLoweringProvider::LoweringContext::getOrCreateLabel(ir::BlockIdentifier blockId) {
	auto it = blockLabels.find(blockId);
	if (it != blockLabels.end())
		return it->second;
	auto label = cc.newLabel();
	blockLabels[blockId] = label;
	return label;
}

// ── Register move helper ──────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::emitMove(const AsmReg& dst, const AsmReg& src) {
	if (std::holds_alternative<Xmm>(dst)) {
		cc.movaps(toXmm(dst), toXmm(src));
	} else {
		cc.mov(toGp(dst), toGp(src));
	}
}

// See the header for the rationale (issue #321). When the identifier is
// already bound it is a downstream merge-block parameter register; copy the
// result into it rather than dropping the definition.
void AsmJitLoweringProvider::LoweringContext::bindResult(const ir::OperationIdentifier& id, const AsmReg& reg,
                                                         RegisterFrame& frame) {
	if (frame.contains(id)) {
		emitMove(frame.getValue(id), reg);
	} else {
		frame.setValue(id, reg);
	}
}

// ── Deferred-constant operand helpers ─────────────────────────────────────────
// With `asmjit.enableConstFolding` on, visitConstInt/Boolean/Ptr emit nothing;
// consumers recover the value from the input operation itself and either fold
// it as an immediate or rematerialise it. Values are canonicalised to the same
// 64-bit register pattern the eager materialisation would have produced, so
// the register-content invariant (sign/zero-extension per stamp) is identical
// on both paths.

namespace {
// Truncate @p value to @p stamp's width and re-extend per its signedness --
// the canonical 64-bit register pattern the materialising lowering produces.
int64_t canonicalizeToStamp(int64_t value, Type stamp) {
	switch (stamp) {
	case Type::i8:
		return static_cast<int64_t>(static_cast<int8_t>(value));
	case Type::i16:
		return static_cast<int64_t>(static_cast<int16_t>(value));
	case Type::i32:
		return static_cast<int64_t>(static_cast<int32_t>(value));
	case Type::b:
	case Type::ui8:
		return static_cast<int64_t>(static_cast<uint8_t>(value));
	case Type::ui16:
		return static_cast<int64_t>(static_cast<uint16_t>(value));
	case Type::ui32:
		return static_cast<int64_t>(static_cast<uint32_t>(value));
	default:
		return value; // i64/ui64/ptr -- full width already.
	}
}
} // anonymous namespace

std::optional<int64_t> AsmJitLoweringProvider::LoweringContext::foldableConstValue(const ir::Operation* in) {
	if (const auto* constInt = ir::dyn_cast<ir::ConstIntOperation>(in)) {
		return canonicalizeToStamp(constInt->getValue(), constInt->getStamp());
	}
	if (const auto* constBool = ir::dyn_cast<ir::ConstBooleanOperation>(in)) {
		return constBool->getValue() ? 1 : 0;
	}
	if (const auto* constPtr = ir::dyn_cast<ir::ConstPtrOperation>(in)) {
		return static_cast<int64_t>(reinterpret_cast<uint64_t>(constPtr->getValue()));
	}
	// Traced constants typically reach their consumer through an integer
	// cast ("$3 = 7 :i32; $4 = $3 cast_to i64"). An integer→integer cast of
	// a constant chain is itself a compile-time constant: re-canonicalising
	// the inner value per the destination stamp is exactly visitCast's
	// extend-then-narrow semantics.
	if (const auto* cast = ir::dyn_cast<ir::CastOperation>(in)) {
		const Type srcType = cast->getInput()->getStamp();
		const Type dstType = cast->getStamp();
		if (!isFloatType(srcType) && !isFloatType(dstType)) {
			if (const auto inner = foldableConstValue(cast->getInput())) {
				// A cast to bool is `value != 0`, not a truncation to the low byte.
				if (dstType == Type::b) {
					return *inner != 0 ? 1 : 0;
				}
				return canonicalizeToStamp(*inner, dstType);
			}
		}
	}
	return std::nullopt;
}

Gp AsmJitLoweringProvider::LoweringContext::gpOperand(const ir::Operation* in, RegisterFrame& frame) {
	// A constant operand is recovered from the operation itself, IGNORING any
	// frame binding: the constant's identifier can be shadowed by a
	// merge-block parameter register that carries a different value on other
	// paths (issue #321), while an SSA input edge to a constant operation
	// always means that constant.
	if (enableConstFolding_) {
		if (const auto value = foldableConstValue(in)) {
			auto reg = cc.newInt64();
			cc.mov(reg, *value);
			return reg;
		}
	}
	return toGp(frame.getValue(in->getIdentifier()));
}

AsmJitLoweringProvider::AsmReg AsmJitLoweringProvider::LoweringContext::regOperand(const ir::Operation* in,
                                                                                   RegisterFrame& frame) {
	if (enableConstFolding_ && foldableConstValue(in).has_value()) {
		return AsmReg(gpOperand(in, frame));
	}
	return frame.getValue(in->getIdentifier());
}

std::optional<int32_t> AsmJitLoweringProvider::LoweringContext::imm32Operand(const ir::Operation* in) {
	if (!enableConstFolding_) {
		return std::nullopt;
	}
	const auto value = foldableConstValue(in);
	if (!value.has_value() || *value < INT32_MIN || *value > INT32_MAX) {
		return std::nullopt;
	}
	return static_cast<int32_t>(*value);
}

void AsmJitLoweringProvider::LoweringContext::emitMoveFromOperand(const AsmReg& dst, const ir::Operation* src,
                                                                  RegisterFrame& frame) {
	if (enableConstFolding_) {
		if (const auto value = foldableConstValue(src)) {
			cc.mov(toGp(dst), *value);
			return;
		}
	}
	emitMove(dst, frame.getValue(src->getIdentifier()));
}

// ── Two-pass compilation ──────────────────────────────────────────────────────
//
// Pass 1: call cc.newFunc() for every FunctionOperation, storing each FuncNode*.
//         This allocates stable Labels before any code is emitted, enabling
//         forward and mutual references between functions.
//
// Pass 2: for each function, call cc.addFunc(funcNode), emit the body,
//         and call cc.endFunc(). cc.finalize() resolves all label references.

void AsmJitLoweringProvider::LoweringContext::processAll(std::string* asmjitIRDump) {
	const auto& functionOperations = ir->getFunctionOperations();
	if (functionOperations.empty()) {
		throw std::runtime_error("AsmJit: no functions found in IR graph");
	}

	// Pass 1: register all functions and obtain stable labels.
	for (const auto& funcOp : functionOperations) {
		const auto& funcBlock = funcOp->getFunctionBasicBlock();
		const auto& entryArgs = funcBlock.getArguments();
		FuncSignature sig;
		const Type retType = funcOp->getOutputArg();
		sig.setRet(retType == Type::v ? TypeId::kVoid : getTypeId(retType));
		for (const auto& arg : entryArgs) {
			sig.addArg(getTypeId(arg->getStamp()));
		}
		auto* funcNode = cc.newFunc(sig);
		funcNodes_[ir->getFunctionTable().findByDefinition(funcOp)] = funcNode;
		funcNodesByName_[funcOp->getName()] = funcNode;
	}

	// Pass 2: emit each function body.
	for (const auto& funcOp : functionOperations) {
		const auto& funcBlock = funcOp->getFunctionBasicBlock();
		const auto& entryArgs = funcBlock.getArguments();
		auto* funcNode = funcNodes_.at(ir->getFunctionTable().findByDefinition(funcOp));

		cc.addFunc(funcNode);

		// Reset per-function block tracking so each function starts clean.
		blockLabels.clear();
		processedBlocks.clear();
		functionAllocaSlots_.clear();
		padLabels_.clear();
		exceptionalExitLabel_.reset();
		currentFunction_ = funcOp;
		transport_ = CapturedExceptionTransport(*funcOp);
		computeDirtyValues(funcOp);
		computeFusionUses(funcOp);
		deferredAddressParts_.clear();

		// Static usage counts feed the compare→branch fusion decision; only
		// pay for the walk when the fusion is enabled.
		usageCounts_ = enableBranchFusion_ ? ir::countUsages(&funcBlock)
		                                   : std::unordered_map<ir::OperationIdentifier, uint32_t> {};

		// Materialise the function's alloca table into one stack slot per
		// entry, captured in a pointer register.  visitAlloca() then just
		// looks the slot up by index.  Doing this here (rather than per
		// use) replaces the old hoisting phase: slot order is fixed by the
		// trace's allocaSpecs, not by where the AllocaOperation lives.
		const auto& allocaSpecs = funcOp->getAllocaSpecs();
		functionAllocaSlots_.reserve(allocaSpecs.size());
		for (const auto& spec : allocaSpecs) {
			auto stackMem =
			    cc.newStack(static_cast<uint32_t>(spec.size), static_cast<uint32_t>(std::max<size_t>(spec.align, 1)));
			auto ptrReg = cc.newIntPtr();
			cc.lea(ptrReg, stackMem);
			functionAllocaSlots_.emplace_back(AsmReg(ptrReg));
		}

		// Pre-create labels for all blocks in this function so forward jumps resolve.
		for (auto* block : funcOp->getBasicBlocks()) {
			getOrCreateLabel(block->getIdentifier());
		}

		// Bind function arguments to virtual registers and sign/zero-extend narrow types.
		RegisterFrame rootFrame;
		for (size_t i = 0; i < entryArgs.size(); i++) {
			const Type stamp = entryArgs[i]->getStamp();
			auto reg = allocReg(stamp);
			if (std::holds_alternative<Gp>(reg)) {
				funcNode->setArg(i, toGp(reg));
			} else {
				funcNode->setArg(i, toXmm(reg));
			}
			// The body works on a copy, sign/zero-extended per the stamp. The
			// argument register itself carries the ABI register as a hint, which
			// the allocator honors even when the value is live across a call
			// (forcing a spill around it); the copy is unconstrained, and the
			// allocator coalesces it with the argument register when they do
			// not conflict.
			AsmReg value = allocReg(stamp);
			if (std::holds_alternative<Gp>(reg)) {
				extendFromStamp(toGp(value), toGp(reg), stamp);
			} else {
				emitMove(value, reg);
			}
			rootFrame.setValue(entryArgs[i]->getIdentifier(), value);
		}

		processBlock(&funcBlock, rootFrame);
		lowerExceptionPads(rootFrame);
		cc.endFunc();
	}

	// Publish the lowering counters into the pipeline-wide statistics sink,
	// mirroring the `asmjit.peephole.*` namespace: keys are present exactly
	// when the corresponding optimization is enabled.
	if (statistics_ != nullptr && enableBranchFusion_) {
		statistics_->add("asmjit.lowering.fusedBranches", fusedBranches_);
	}
	if (statistics_ != nullptr && enableConstFolding_) {
		statistics_->add("asmjit.lowering.foldedImmediates", foldedImmediates_);
	}
	if (statistics_ != nullptr && enableAddressFusion_) {
		statistics_->add("asmjit.lowering.fusedAddresses", fusedAddresses_);
	}
	if (statistics_ != nullptr && enableInlining_) {
		statistics_->add("asmjit.lowering.inlinedCalls", inlinedCalls_);
	}
	if (statistics_ != nullptr && enableBranchFusion_ && enableLoopRotation_) {
		statistics_->add("asmjit.lowering.rotatedLoops", rotatedLoops_);
	}

	// Format the builder node list before finalize(): at this point the IR still carries
	// virtual registers, which is the representation users want to inspect as "asmjit IR".
	if (asmjitIRDump != nullptr) {
		::asmjit::String sb;
		::asmjit::FormatOptions formatOptions;
		::asmjit::Formatter::formatNodeList(sb, formatOptions, &cc);
		asmjitIRDump->assign(sb.data(), sb.size());
	}

	cc.finalize();
}

// ── Block processing ──────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::processBlock(const ir::BasicBlock* block, RegisterFrame& frame) {
	const auto id = block->getIdentifier();
	if (processedBlocks.count(id))
		return;
	processedBlocks.insert(id);

	// Bind the pre-created label for this block.
	cc.bind(getOrCreateLabel(id));

	// visitIf consumes a pending fused compare before recursing back here.
	assert(pendingFusedCompare_ == nullptr && "fused compare leaked across blocks");

	const auto& ops = block->getOperations();
	for (size_t i = 0; i < ops.size(); i++) {
		// Compare→branch fusion handshake: when the compare's flags can be
		// consumed directly by the IfOperation that follows it, skip the
		// compare's own lowering and let visitIf emit a fused cmp+jcc.
		// Adjacency guarantees no instruction between cmp and jcc can
		// clobber EFLAGS or retarget an operand register.
		if (enableBranchFusion_ && i + 1 < ops.size()) {
			if (auto* cmp = ir::dyn_cast<ir::CompareOperation>(ops[i])) {
				if (isFusibleCompare(cmp, ops[i + 1], frame)) {
					pendingFusedCompare_ = cmp;
					continue;
				}
			}
		}
		dispatch(ops[i], frame);
	}
}

bool AsmJitLoweringProvider::LoweringContext::isFusibleCompare(const ir::CompareOperation* cmp,
                                                               const ir::Operation* next, RegisterFrame& frame) {
	const auto* ifOp = ir::dyn_cast<ir::IfOperation>(next);
	if (ifOp == nullptr || ifOp->getValue() != cmp) {
		return false;
	}
	// Float EQ/NE needs the parity flag on top of the primary condition
	// (see visitCompare); fusing those requires a two-jump sequence, so the
	// first version keeps float compares on the materialising path.
	if (isFloatType(cmp->getLeftInput()->getStamp())) {
		return false;
	}
	// The if must be the sole consumer; any other use still needs the
	// materialised boolean.
	const auto it = usageCounts_.find(cmp->getIdentifier());
	if (it == usageCounts_.end() || it->second != 1) {
		return false;
	}
	// If the identifier is already bound it doubles as a downstream merge
	// block's parameter register (issue #321); that register must be written,
	// so the compare cannot skip materialisation.
	return !frame.contains(cmp->getIdentifier());
}

// ── Block-argument passing ────────────────────────────────────────────────────
// Copies source values into the destination block's pre-allocated argument
// registers, using temporaries to avoid clobbering when src/dst overlap.

void AsmJitLoweringProvider::LoweringContext::processBlockInvocation(const ir::BasicBlockInvocation& bi,
                                                                     RegisterFrame& frame) {
	const auto& srcArgs = bi.getArguments();
	const auto& dstArgs = bi.getBlock()->getArguments();

	if (srcArgs.empty())
		return;

	// Snapshot the source operands BEFORE binding destination registers: a
	// source identifier can coincide with a destination parameter identifier
	// (issue #321), and allocating the destination first would shadow a
	// deferred-constant source behind the fresh (uninitialised) register.
	//
	// This runs once per CFG edge with one entry per block argument, so the
	// scratch buffers below are members that keep their capacity across calls
	// instead of being allocated per edge (issue #508).
	auto& sources = blockArgSources_;
	sources.clear();
	for (size_t i = 0; i < srcArgs.size(); i++) {
		BlockArgSource source;
		// Constant sources are recovered from the operation itself (see
		// gpOperand for why a frame binding must not shadow a constant).
		const auto value = enableConstFolding_ ? foldableConstValue(srcArgs[i]) : std::optional<int64_t> {};
		if (value.has_value()) {
			source.imm = *value;
		} else {
			source.reg = frame.getValue(srcArgs[i]->getIdentifier());
		}
		sources.push_back(source);
	}

	// Ensure every destination block-argument has a virtual register.
	// If the identifier was already assigned (e.g. it matches a predecessor's
	// SSA value), reuse that register; otherwise allocate a fresh one now.
	auto& dsts = blockArgDsts_;
	dsts.clear();
	for (size_t i = 0; i < dstArgs.size(); i++) {
		const auto& dstId = dstArgs[i]->getIdentifier();
		if (!frame.contains(dstId)) {
			frame.setValue(dstId, allocReg(dstArgs[i]->getStamp()));
		}
		dsts.push_back(frame.getValue(dstId));
	}

	// Parallel-copy semantics: a destination write must not clobber a source
	// that a later copy still reads. Only a register source that is itself
	// one of the destination registers needs to detour through a temp; a
	// self-move (src == dst) needs no code at all, and everything else can
	// be written directly (immediates and temps can never alias a source).
	//
	// Destination registers are marked in a table indexed by virtual register
	// index. Each call uses a fresh epoch value as its mark, so the table
	// never has to be cleared between calls.
	const auto vregIndex = [](const AsmReg& r) {
		const uint32_t id = std::visit([](const auto& reg) { return reg.id(); }, r);
		assert(::asmjit::Operand::isVirtId(id) && "block arguments live in virtual registers");
		return ::asmjit::Operand::virtIdToIndex(id);
	};
	if (dstMarks_.size() < cc.virtRegs().size()) {
		dstMarks_.resize(cc.virtRegs().size(), 0);
	}
	if (++dstMarkEpoch_ == 0) {
		std::fill(dstMarks_.begin(), dstMarks_.end(), 0);
		dstMarkEpoch_ = 1;
	}
	for (const auto& dst : dsts) {
		dstMarks_[vregIndex(dst)] = dstMarkEpoch_;
	}

	// Phase 1: detour hazardous register sources through fresh temps.
	auto& temps = blockArgTemps_;
	temps.assign(srcArgs.size(), std::nullopt);
	for (size_t i = 0; i < srcArgs.size(); i++) {
		if (!sources[i].reg.has_value()) {
			continue; // immediate -- written directly in phase 2
		}
		const auto srcIndex = vregIndex(*sources[i].reg);
		if (srcIndex == vregIndex(dsts[i])) {
			continue; // self-move -- no code needed
		}
		if (dstMarks_[srcIndex] == dstMarkEpoch_) {
			auto temp = allocReg(dstArgs[i]->getStamp());
			emitMove(temp, *sources[i].reg);
			temps[i] = temp;
		}
	}

	// Phase 2: write the destinations.
	for (size_t i = 0; i < srcArgs.size(); i++) {
		const auto& dst = dsts[i];
		if (temps[i].has_value()) {
			emitMove(dst, *temps[i]);
		} else if (sources[i].reg.has_value()) {
			if (vregIndex(*sources[i].reg) != vregIndex(dst)) {
				emitMove(dst, *sources[i].reg);
			}
		} else {
			cc.mov(toGp(dst), sources[i].imm);
		}
	}
}

// ── Constants ─────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitConstBoolean(ir::ConstBooleanOperation* op, RegisterFrame& frame) {
	// Deferred (see the operand helpers): consumers fold or rematerialise.
	// A bound identifier doubles as a merge-block parameter register (issue
	// #321) that must be written, so those keep the materialising path.
	if (enableConstFolding_ && !frame.contains(op->getIdentifier())) {
		return;
	}
	auto reg = allocReg(Type::b);
	cc.mov(toGp(reg), static_cast<int64_t>(op->getValue() ? 1 : 0));
	bindResult(op->getIdentifier(), reg, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitConstInt(ir::ConstIntOperation* op, RegisterFrame& frame) {
	if (enableConstFolding_ && !frame.contains(op->getIdentifier())) {
		return;
	}
	auto reg = allocReg(op->getStamp());
	cc.mov(toGp(reg), op->getValue());
	bindResult(op->getIdentifier(), reg, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitConstFloat(ir::ConstFloatOperation* op, RegisterFrame& frame) {
	// One RIP-relative load from the local constant pool instead of a GP
	// temp plus movd/movq round-trip through the integer register file.
	auto reg = allocReg(op->getStamp());
	auto xmmReg = toXmm(reg);
	if (op->getStamp() == Type::f32) {
		auto mem = cc.newFloatConst(ConstPoolScope::kLocal, static_cast<float>(op->getValue()));
		cc.movss(xmmReg, mem);
	} else {
		auto mem = cc.newDoubleConst(ConstPoolScope::kLocal, op->getValue());
		cc.movsd(xmmReg, mem);
	}
	bindResult(op->getIdentifier(), reg, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitConstPtr(ir::ConstPtrOperation* op, RegisterFrame& frame) {
	if (enableConstFolding_ && !frame.contains(op->getIdentifier())) {
		return;
	}
	auto reg = allocReg(Type::ptr);
	cc.mov(toGp(reg), reinterpret_cast<uint64_t>(op->getValue()));
	bindResult(op->getIdentifier(), reg, frame);
}

// ── Arithmetic ────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitAdd(ir::AddOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto left = frame.getValue(op->getLeftInput()->getIdentifier());
		auto right = frame.getValue(op->getRightInput()->getIdentifier());
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.addss(xDst, toXmm(right));
		else
			cc.addsd(xDst, toXmm(right));
	} else if (const auto parts = matchAddress(op)) {
		// Every consumer folds it into its own memory operand: emit nothing.
		if (deferAddressPart(op, 0x1, frame)) {
			return;
		}
		// Otherwise one three-operand lea computes it, scaling included.
		cc.lea(toGp(result), memFromParts(*parts, frame));
		if (parts->offset == nullptr) {
			foldedImmediates_++;
		}
	} else {
		auto gDst = toGp(result);
		const uint32_t width = stampBits(op->getStamp());
		// Fold a small-constant operand into the add's immediate form
		// (add is commutative, so either side qualifies).
		const auto rightImm = imm32Operand(op->getRightInput());
		std::optional<int32_t> leftImm;
		if (!rightImm.has_value()) {
			leftImm = imm32Operand(op->getLeftInput());
		}
		if (rightImm.has_value()) {
			cc.mov(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame));
			cc.add(gDst, *rightImm);
			foldedImmediates_++;
		} else if (leftImm.has_value()) {
			cc.mov(gDst, gpOperandAtWidth(op->getRightInput(), width, frame));
			cc.add(gDst, *leftImm);
			foldedImmediates_++;
		} else {
			cc.mov(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame));
			cc.add(gDst, gpOperandAtWidth(op->getRightInput(), width, frame));
		}
		// An add that overflows the narrow stamp's width still produces a
		// "correct" 64-bit sum; re-extend per the result type so its
		// sign/zero-extension matches the wrapped-around narrow-width value
		// (see narrowToStamp's doc comment), unless narrowing is lazy.
		narrowResult(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitSub(ir::SubOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto left = frame.getValue(op->getLeftInput()->getIdentifier());
		auto right = frame.getValue(op->getRightInput()->getIdentifier());
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.subss(xDst, toXmm(right));
		else
			cc.subsd(xDst, toXmm(right));
	} else {
		auto gDst = toGp(result);
		const uint32_t width = stampBits(op->getStamp());
		cc.mov(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame));
		if (const auto rightImm = imm32Operand(op->getRightInput())) {
			cc.sub(gDst, *rightImm);
			foldedImmediates_++;
		} else {
			cc.sub(gDst, gpOperandAtWidth(op->getRightInput(), width, frame));
		}
		narrowResult(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMul(ir::MulOperation* op, RegisterFrame& frame) {
	// A scaled address index every consumer folds into its address.
	if (deferAddressPart(op, 0x2, frame)) {
		return;
	}
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto left = frame.getValue(op->getLeftInput()->getIdentifier());
		auto right = frame.getValue(op->getRightInput()->getIdentifier());
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.mulss(xDst, toXmm(right));
		else
			cc.mulsd(xDst, toXmm(right));
	} else {
		auto gDst = toGp(result);
		// Fold a small-constant operand via the three-operand imul form
		// (commutative, so either side qualifies).
		const auto rightImm = imm32Operand(op->getRightInput());
		std::optional<int32_t> leftImm;
		if (!rightImm.has_value()) {
			leftImm = imm32Operand(op->getLeftInput());
		}
		const uint32_t width = stampBits(op->getStamp());
		if (rightImm.has_value() || leftImm.has_value()) {
			const int32_t imm = rightImm.has_value() ? *rightImm : *leftImm;
			auto src = gpOperandAtWidth(rightImm.has_value() ? op->getLeftInput() : op->getRightInput(), width, frame);
			// Strength-reduce the 3-cycle imul: a power of two is a shift, and
			// 3/5/9 are one lea (the low 64 bits agree in every case).
			if (imm > 1 && (imm & (imm - 1)) == 0) {
				cc.mov(gDst, src);
				cc.shl(gDst, static_cast<uint32_t>(std::countr_zero(static_cast<uint32_t>(imm))));
			} else if (imm == 3 || imm == 5 || imm == 9) {
				cc.lea(gDst,
				       x86::ptr(src, src, static_cast<uint32_t>(std::countr_zero(static_cast<uint32_t>(imm - 1)))));
			} else {
				cc.imul(gDst, src, imm);
			}
			foldedImmediates_++;
		} else {
			cc.mov(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame));
			cc.imul(gDst, gpOperandAtWidth(op->getRightInput(), width, frame));
		}
		narrowResult(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitDiv(ir::DivOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto left = frame.getValue(op->getLeftInput()->getIdentifier());
		auto right = frame.getValue(op->getRightInput()->getIdentifier());
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.divss(xDst, toXmm(right));
		else
			cc.divsd(xDst, toXmm(right));
	} else {
		// Integer division: use idiv (signed) or div (unsigned).
		// AsmJit Compiler handles the rax/rdx hardware constraint automatically.
		auto quot = cc.newInt64();
		auto rem = cc.newInt64();
		cc.mov(quot, cleanGpOperand(op->getLeftInput(), frame));
		if (isUnsignedType(op->getStamp())) {
			cc.xor_(rem, rem);
			cc.div(rem, quot, cleanGpOperand(op->getRightInput(), frame));
		} else {
			cc.cqo(rem, quot);
			cc.idiv(rem, quot, cleanGpOperand(op->getRightInput(), frame));
		}
		cc.mov(toGp(result), quot);
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMod(ir::ModOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	auto quot = cc.newInt64();
	auto rem = cc.newInt64();
	cc.mov(quot, cleanGpOperand(op->getLeftInput(), frame));
	if (isUnsignedType(op->getStamp())) {
		cc.xor_(rem, rem);
		cc.div(rem, quot, cleanGpOperand(op->getRightInput(), frame));
	} else {
		cc.cqo(rem, quot);
		cc.idiv(rem, quot, cleanGpOperand(op->getRightInput(), frame));
	}
	cc.mov(toGp(result), rem);
	bindResult(op->getIdentifier(), result, frame);
}

// ── Logical / compare ─────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitCompare(ir::CompareOperation* op, RegisterFrame& frame) {
	auto result = allocReg(Type::b);
	auto resultGp = toGp(result).r8();
	const bool leftIsFloat = isFloatType(op->getLeftInput()->getStamp());
	const bool leftIsUnsigned = isUnsignedType(op->getLeftInput()->getStamp());

	if (leftIsFloat) {
		auto left = frame.getValue(op->getLeftInput()->getIdentifier());
		auto right = frame.getValue(op->getRightInput()->getIdentifier());
		const bool isF32 = op->getLeftInput()->getStamp() == Type::f32;
		auto ucomi = [&](Xmm a, Xmm b) {
			if (isF32)
				cc.ucomiss(a, b);
			else
				cc.ucomisd(a, b);
		};
		// ucomiss/ucomisd sets ZF=1,PF=1,CF=1 for an unordered result (either
		// operand NaN). A single SETcc can't express "ordered and equal" or
		// "unordered or not-equal" -- EQ/NE additionally need the parity flag
		// (PF), which signals "unordered". LT/LE instead compare with the
		// operands swapped and use the "above"/"above-or-equal" condition
		// (CF=0 required), which is already false for an unordered result;
		// GT/GE already get this for free without swapping.
		switch (op->getComparator()) {
		case ir::CompareOperation::EQ: {
			ucomi(toXmm(left), toXmm(right));
			cc.sete(resultGp);
			auto parity = toGp(allocReg(Type::b)).r8();
			cc.setnp(parity);
			cc.and_(resultGp, parity);
			break;
		}
		case ir::CompareOperation::NE: {
			ucomi(toXmm(left), toXmm(right));
			cc.setne(resultGp);
			auto parity = toGp(allocReg(Type::b)).r8();
			cc.setp(parity);
			cc.or_(resultGp, parity);
			break;
		}
		case ir::CompareOperation::LT:
			ucomi(toXmm(right), toXmm(left));
			cc.seta(resultGp);
			break;
		case ir::CompareOperation::LE:
			ucomi(toXmm(right), toXmm(left));
			cc.setae(resultGp);
			break;
		case ir::CompareOperation::GT:
			ucomi(toXmm(left), toXmm(right));
			cc.seta(resultGp);
			break;
		case ir::CompareOperation::GE:
			ucomi(toXmm(left), toXmm(right));
			cc.setae(resultGp);
			break;
		}
	} else if (op->getLeftInput()->getStamp() == Type::ptr && isInteger(op->getRightInput()->getStamp())) {
		// Null-pointer check: compare pointer against zero.
		auto left = gpOperand(op->getLeftInput(), frame);
		cc.test(left, left);
		if (op->getComparator() == ir::CompareOperation::EQ)
			cc.sete(resultGp);
		else
			cc.setne(resultGp);
	} else {
		emitIntegerCompare(op, frame);
		if (leftIsUnsigned) {
			switch (op->getComparator()) {
			case ir::CompareOperation::EQ:
				cc.sete(resultGp);
				break;
			case ir::CompareOperation::NE:
				cc.setne(resultGp);
				break;
			case ir::CompareOperation::LT:
				cc.setb(resultGp);
				break;
			case ir::CompareOperation::LE:
				cc.setbe(resultGp);
				break;
			case ir::CompareOperation::GT:
				cc.seta(resultGp);
				break;
			case ir::CompareOperation::GE:
				cc.setae(resultGp);
				break;
			}
		} else {
			switch (op->getComparator()) {
			case ir::CompareOperation::EQ:
				cc.sete(resultGp);
				break;
			case ir::CompareOperation::NE:
				cc.setne(resultGp);
				break;
			case ir::CompareOperation::LT:
				cc.setl(resultGp);
				break;
			case ir::CompareOperation::LE:
				cc.setle(resultGp);
				break;
			case ir::CompareOperation::GT:
				cc.setg(resultGp);
				break;
			case ir::CompareOperation::GE:
				cc.setge(resultGp);
				break;
			}
		}
	}
	// Zero-extend the 8-bit result into the full 64-bit virtual register.
	cc.movzx(toGp(result).r32(), resultGp);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), gpOperand(op->getLeftInput(), frame));
	if (const auto rightImm = imm32Operand(op->getRightInput())) {
		cc.and_(toGp(result), *rightImm);
		foldedImmediates_++;
	} else {
		cc.and_(toGp(result), gpOperand(op->getRightInput(), frame));
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), gpOperand(op->getLeftInput(), frame));
	if (const auto rightImm = imm32Operand(op->getRightInput())) {
		cc.or_(toGp(result), *rightImm);
		foldedImmediates_++;
	} else {
		cc.or_(toGp(result), gpOperand(op->getRightInput(), frame));
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNot(ir::NotOperation* op, RegisterFrame& frame) {
	// NotOperation is logical NOT on a boolean: result = input XOR 1.
	auto input = gpOperand(op->getInput(), frame);
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), input);
	cc.xor_(toGp(result), 1);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* op, RegisterFrame& frame) {
	auto stamp = op->getStamp();
	if (isFloatType(stamp)) {
		// IEEE-754 sign-flip: XOR the sign bit against a mask constant. This is
		// correct for every input including zero, unlike bitwise NOT (undefined
		// on floats) or `0 - x` (mis-signs zero).
		auto input = frame.getValue(op->getInput()->getIdentifier());
		auto result = allocReg(stamp);
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(input));
		// Load the mask into a register first (matching visitConstFloat's
		// established pattern) rather than using it as a direct memory operand
		// to the XOR itself.
		auto maskReg = allocReg(stamp);
		auto xMask = toXmm(maskReg);
		if (stamp == Type::f32) {
			uint32_t bits = 0x80000000u;
			float mask;
			memcpy(&mask, &bits, sizeof(mask));
			auto mem = cc.newFloatConst(ConstPoolScope::kLocal, mask);
			cc.movss(xMask, mem);
			cc.xorps(xDst, xMask);
		} else {
			uint64_t bits = 0x8000000000000000ULL;
			double mask;
			memcpy(&mask, &bits, sizeof(mask));
			auto mem = cc.newDoubleConst(ConstPoolScope::kLocal, mask);
			cc.movsd(xMask, mem);
			cc.xorpd(xDst, xMask);
		}
		bindResult(op->getIdentifier(), result, frame);
		return;
	}
	// NegateOperation is bitwise NOT (~x): result = input XOR all-ones.
	auto result = allocReg(stamp);
	auto gDst = toGp(result);
	cc.mov(gDst, gpOperandAtWidth(op->getInput(), stampBits(stamp), frame));
	cc.not_(gDst);
	// not_ flips the full 64-bit register, including the extension padding.
	// That happens to stay correct for signed stamps (flipping a sign bit
	// flips its replicated extension consistently) but is wrong for unsigned
	// stamps, whose invariant is a zero-extended (not flipped) upper half.
	narrowResult(gDst, stamp);
	bindResult(op->getIdentifier(), result, frame);
}

// ── Binary bit operations ─────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitShift(ir::ShiftOperation* op, RegisterFrame& frame) {
	// A scaled address index every consumer folds into its address.
	if (deferAddressPart(op, 0x2, frame)) {
		return;
	}
	auto result = allocReg(op->getStamp());
	auto gDst = toGp(result);
	// A left shift's low bits depend only on the input's low bits; a right
	// shift pulls the upper bits down, so it needs the canonical pattern.
	const bool isLeftShift = op->getType() == ir::ShiftOperation::LS;
	cc.mov(gDst, isLeftShift ? gpOperandAtWidth(op->getLeftInput(), stampBits(op->getStamp()), frame)
	                         : cleanGpOperand(op->getLeftInput(), frame));
	// A constant count uses the immediate shift form. The hardware masks the
	// count mod 64 for 64-bit shifts, exactly like the CL-register form, so
	// masking here preserves the register-form semantics.
	if (const auto countImm = imm32Operand(op->getRightInput())) {
		const uint32_t count = static_cast<uint32_t>(*countImm) & 63u;
		if (op->getType() == ir::ShiftOperation::LS) {
			cc.shl(gDst, count);
		} else if (isUnsignedType(op->getStamp())) {
			cc.shr(gDst, count);
		} else {
			cc.sar(gDst, count);
		}
		foldedImmediates_++;
	} else {
		auto right = gpOperand(op->getRightInput(), frame);
		// The shift count operand must be the CL register; AsmJit's Compiler
		// handles that constraint when a GP register is given as the count.
		if (op->getType() == ir::ShiftOperation::LS) {
			cc.shl(gDst, right.r8());
		} else if (isUnsignedType(op->getStamp())) {
			cc.shr(gDst, right.r8());
		} else {
			cc.sar(gDst, right.r8());
		}
	}
	// Shifting the full 64-bit register (rather than just the narrow stamp's
	// width) can leave the extension padding inconsistent with the
	// narrow-width result -- e.g. a left shift that overflows the stamp's
	// width still computes a "correct" 64-bit shift, whose sign-extension no
	// longer matches the wrapped-around narrow-width value. sar already
	// shifts in the sign bit, but a shift can still move that bit into
	// positions that change the narrow-width result's own sign, so this is
	// needed for all three shift forms. (Under lazy narrowing a left shift's
	// result is recorded as dirty instead.)
	if (isLeftShift) {
		narrowResult(gDst, op->getStamp());
	} else {
		narrowToStamp(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitBinaryComp(ir::BinaryCompOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	const uint32_t width = stampBits(op->getStamp());
	cc.mov(toGp(result), gpOperandAtWidth(op->getLeftInput(), width, frame));
	if (const auto rightImm = imm32Operand(op->getRightInput())) {
		switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			cc.and_(toGp(result), *rightImm);
			break;
		case ir::BinaryCompOperation::BOR:
			cc.or_(toGp(result), *rightImm);
			break;
		case ir::BinaryCompOperation::XOR:
			cc.xor_(toGp(result), *rightImm);
			break;
		}
		foldedImmediates_++;
	} else {
		auto right = gpOperandAtWidth(op->getRightInput(), width, frame);
		switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			cc.and_(toGp(result), right);
			break;
		case ir::BinaryCompOperation::BOR:
			cc.or_(toGp(result), right);
			break;
		case ir::BinaryCompOperation::XOR:
			cc.xor_(toGp(result), right);
			break;
		}
	}
	bindResult(op->getIdentifier(), result, frame);
}

// ── Control flow ──────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitIf(ir::IfOperation* op, RegisterFrame& frame) {
	auto trueLabel = getOrCreateLabel(op->getTrueBlockInvocation().getBlock()->getIdentifier());
	auto falseLabel = getOrCreateLabel(op->getFalseBlockInvocation().getBlock()->getIdentifier());

	auto elsePath = cc.newLabel();
	if (pendingFusedCompare_ != nullptr) {
		// processBlock proved this compare's only consumer is this if and
		// skipped its lowering; emit the compare and the negated conditional
		// jump in one fused step. The block-argument copies below sit after
		// the jcc, so they cannot disturb the flags.
		assert(op->getValue() == pendingFusedCompare_ && "pending fused compare does not feed this if");
		const auto* cmp = pendingFusedCompare_;
		pendingFusedCompare_ = nullptr;
		emitFusedCompareBranch(cmp, elsePath, frame);
		fusedBranches_++;
	} else {
		auto condGp = gpOperand(op->getValue(), frame);
		cc.test(condGp, condGp);
		cc.jz(elsePath);
	}

	// True branch: copy arguments then jump.
	processBlockInvocation(op->getTrueBlockInvocation(), frame);
	cc.jmp(trueLabel);

	// Emit the true-branch body *between* the two invocations' arg-passing
	// steps. The body's value-producing visitors (visitAdd, etc.) bind
	// their SSA identifiers in the frame; when those identifiers
	// coincide with a downstream block arg (typical at an SSA merge
	// point that both arms target), the false invocation below must see
	// the body's binding so it emits a MOV into the body-chosen
	// register. Processing the false invocation first would bind the
	// merge identifier to a fresh temp register, and the body's later
	// `frame.setValue` — an `emplace`, no overwrite — would silently
	// fail to retarget it.
	processBlock(op->getTrueBlockInvocation().getBlock(), frame);

	// False branch: copy arguments then jump.
	cc.bind(elsePath);
	processBlockInvocation(op->getFalseBlockInvocation(), frame);
	cc.jmp(falseLabel);

	processBlock(op->getFalseBlockInvocation().getBlock(), frame);
}

// Emits the flag-setting half of an integer compare (everything but the
// null-pointer check). When both sides are 32 bits wide the compare runs on
// the low halves, which are exact even for dirty operands (see the lazy
// narrowing helpers); other widths compare the canonical 64-bit patterns.
void AsmJitLoweringProvider::LoweringContext::emitIntegerCompare(const ir::CompareOperation* cmp,
                                                                 RegisterFrame& frame) {
	const auto* leftIn = cmp->getLeftInput();
	const auto* rightIn = cmp->getRightInput();
	const bool compare32 = stampBits(leftIn->getStamp()) == 32 && stampBits(rightIn->getStamp()) == 32;
	auto left = compare32 ? gpOperand(leftIn, frame).r32() : cleanGpOperand(leftIn, frame);
	// Peephole: `cmp x, 0` → `test x, x` when the right operand is the
	// integer constant zero. Same flag output for ZF/SF/CF/OF and all
	// consumers here read only via setcc/jcc — two bytes shorter and
	// breaks no dependency.
	const auto* rightConst = ir::dyn_cast<ir::ConstIntOperation>(rightIn);
	if (rightConst != nullptr && rightConst->getValue() == 0) {
		cc.test(left, left);
	} else if (const auto rightImm = imm32Operand(rightIn)) {
		// The canonical pattern fits a sign-extended imm32, so its low 32 bits
		// are also the 32-bit compare's immediate.
		cc.cmp(left, *rightImm);
		foldedImmediates_++;
	} else {
		cc.cmp(left, compare32 ? gpOperand(rightIn, frame).r32() : cleanGpOperand(rightIn, frame));
	}
}

// Fused replacement for visitCompare + the test/jz in visitIf: emits the
// compare and jumps to @p falseTarget when the condition is false. Mirrors
// visitCompare's integer paths (the float path is excluded by
// isFusibleCompare); the condition codes are the negation of the setcc the
// unfused lowering would have used.
void AsmJitLoweringProvider::LoweringContext::emitFusedCompareBranch(const ir::CompareOperation* cmp, Label target,
                                                                     RegisterFrame& frame, bool jumpIfTrue) {
	// Jump to `target` on `cond` (the condition holding) when jumpIfTrue,
	// otherwise on its negation.
	const auto jumpOn = [&](CondCode cond) {
		cc.j(jumpIfTrue ? cond : x86::negateCond(cond), target);
	};
	if (cmp->getLeftInput()->getStamp() == Type::ptr && isInteger(cmp->getRightInput()->getStamp())) {
		// Null-pointer check (see visitCompare): only EQ/NE are meaningful.
		auto left = gpOperand(cmp->getLeftInput(), frame);
		cc.test(left, left);
		if (cmp->getComparator() == ir::CompareOperation::EQ) {
			jumpOn(CondCode::kZero);
		} else {
			jumpOn(CondCode::kNotZero);
		}
		return;
	}

	emitIntegerCompare(cmp, frame);

	if (isUnsignedType(cmp->getLeftInput()->getStamp())) {
		switch (cmp->getComparator()) {
		case ir::CompareOperation::EQ:
			jumpOn(CondCode::kEqual);
			break;
		case ir::CompareOperation::NE:
			jumpOn(CondCode::kNotEqual);
			break;
		case ir::CompareOperation::LT:
			jumpOn(CondCode::kUnsignedLT);
			break;
		case ir::CompareOperation::LE:
			jumpOn(CondCode::kUnsignedLE);
			break;
		case ir::CompareOperation::GT:
			jumpOn(CondCode::kUnsignedGT);
			break;
		case ir::CompareOperation::GE:
			jumpOn(CondCode::kUnsignedGE);
			break;
		}
	} else {
		switch (cmp->getComparator()) {
		case ir::CompareOperation::EQ:
			jumpOn(CondCode::kEqual);
			break;
		case ir::CompareOperation::NE:
			jumpOn(CondCode::kNotEqual);
			break;
		case ir::CompareOperation::LT:
			jumpOn(CondCode::kSignedLT);
			break;
		case ir::CompareOperation::LE:
			jumpOn(CondCode::kSignedLE);
			break;
		case ir::CompareOperation::GT:
			jumpOn(CondCode::kSignedGT);
			break;
		case ir::CompareOperation::GE:
			jumpOn(CondCode::kSignedGE);
			break;
		}
	}
}

void AsmJitLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* op, RegisterFrame& frame) {
	const auto& bi = op->getNextBlockInvocation();
	processBlockInvocation(bi, frame);
	if (!tryRotateLoopBranch(bi, frame)) {
		cc.jmp(getOrCreateLabel(bi.getBlock()->getIdentifier()));
	}
	processBlock(bi.getBlock(), frame);
}

bool AsmJitLoweringProvider::LoweringContext::isNoOpInvocation(const ir::BasicBlockInvocation& bi,
                                                               RegisterFrame& frame) {
	const auto& srcArgs = bi.getArguments();
	const auto& dstArgs = bi.getBlock()->getArguments();
	for (size_t i = 0; i < srcArgs.size(); i++) {
		const auto& dstId = dstArgs[i]->getIdentifier();
		if ((enableConstFolding_ && foldableConstValue(srcArgs[i]).has_value()) || !frame.contains(dstId) ||
		    !frame.contains(srcArgs[i]->getIdentifier())) {
			return false;
		}
		const auto regId = [](const AsmReg& r) {
			return std::visit([](const auto& reg) { return reg.id(); }, r);
		};
		if (regId(frame.getValue(srcArgs[i]->getIdentifier())) != regId(frame.getValue(dstId))) {
			return false;
		}
	}
	return true;
}

// A loop's back edge normally jumps to the header, which tests the exit
// condition and branches into the body: two jumps per iteration, one of them
// taken. When the header holds nothing but that test (a fused compare feeding
// its if) and the if's own block-argument copies are register self-moves, the
// test can be repeated at the end of the back edge instead, jumping straight
// into the body (taken) or out of the loop. The header's own copy still
// handles the first entry. The compare reads the header's parameter registers,
// which the back edge's copies have just written.
bool AsmJitLoweringProvider::LoweringContext::tryRotateLoopBranch(const ir::BasicBlockInvocation& bi,
                                                                  RegisterFrame& frame) {
	const auto* header = bi.getBlock();
	if (!enableBranchFusion_ || !enableLoopRotation_ || !processedBlocks.contains(header->getIdentifier())) {
		return false;
	}
	const auto& ops = header->getOperations();
	if (ops.size() != 2) {
		return false;
	}
	const auto* cmp = ir::dyn_cast<ir::CompareOperation>(ops[0]);
	const auto* ifOp = ir::dyn_cast<ir::IfOperation>(ops[1]);
	if (cmp == nullptr || ifOp == nullptr || !isFusibleCompare(cmp, ifOp, frame) ||
	    !isNoOpInvocation(ifOp->getTrueBlockInvocation(), frame) ||
	    !isNoOpInvocation(ifOp->getFalseBlockInvocation(), frame)) {
		return false;
	}
	emitFusedCompareBranch(cmp, getOrCreateLabel(ifOp->getTrueBlockInvocation().getBlock()->getIdentifier()), frame,
	                       /*jumpIfTrue=*/true);
	cc.jmp(getOrCreateLabel(ifOp->getFalseBlockInvocation().getBlock()->getIdentifier()));
	rotatedLoops_++;
	return true;
}

void AsmJitLoweringProvider::LoweringContext::visitReturn(ir::ReturnOperation* op, RegisterFrame& frame) {
	if (op->hasReturnValue()) {
		auto retReg = regOperand(op->getReturnValue(), frame);
		if (std::holds_alternative<Xmm>(retReg)) {
			cc.ret(toXmm(retReg));
		} else {
			auto gp = toGp(retReg);
			// Narrow the value to its declared stamp so the caller sees a clean
			// register matching the ABI contract (mirrors A64's visitReturn).
			narrowToStamp(gp, op->getReturnValue()->getStamp());
			cc.ret(gp);
		}
	} else {
		cc.ret();
	}
}

void AsmJitLoweringProvider::LoweringContext::visitSelect(ir::SelectOperation* op, RegisterFrame& frame) {
	auto condGp = gpOperand(op->getCondition(), frame);
	auto result = allocReg(op->getStamp());

	if (enableSelectCmov_ && !isFloatType(op->getStamp())) {
		// Branch-free data mux: both values are already unconditionally
		// computed (select is not lazy), so a cmov replaces the two-way
		// branch and its misprediction risk. The operand movs do not touch
		// EFLAGS, so the test's flags survive until the cmov. A64 lowers
		// select the same way via csel.
		const uint32_t width = stampBits(op->getStamp());
		auto falseGp = gpOperandAtWidth(op->getFalseValue(), width, frame);
		cc.mov(toGp(result), gpOperandAtWidth(op->getTrueValue(), width, frame));
		cc.test(condGp, condGp);
		cc.cmovz(toGp(result), falseGp);
	} else {
		auto falsePath = cc.newLabel();
		auto donePath = cc.newLabel();

		cc.test(condGp, condGp);
		cc.jz(falsePath);
		emitMoveFromOperand(result, op->getTrueValue(), frame);
		cc.jmp(donePath);
		cc.bind(falsePath);
		emitMoveFromOperand(result, op->getFalseValue(), frame);
		cc.bind(donePath);
	}

	bindResult(op->getIdentifier(), result, frame);
}

// ── Memory ────────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitLoad(ir::LoadOperation* op, RegisterFrame& frame) {
	const auto* addr = op->getAddress();
	auto result = allocReg(op->getStamp());
	const auto at = [&](uint32_t size) {
		return memOperand(addr, size, frame);
	};

	if (op->getStamp() == Type::f32) {
		cc.movss(toXmm(result), at(4));
	} else if (op->getStamp() == Type::f64) {
		cc.movsd(toXmm(result), at(8));
	} else {
		auto gDst = toGp(result);
		switch (op->getStamp()) {
		case Type::b:
		case Type::ui8:
			cc.movzx(gDst.r32(), at(1));
			break; // zero-extends to 64
		case Type::i8:
			cc.movsx(gDst.r64(), at(1));
			break;
		case Type::ui16:
			cc.movzx(gDst.r32(), at(2));
			break; // zero-extends to 64
		case Type::i16:
			cc.movsx(gDst.r64(), at(2));
			break;
		case Type::ui32:
			cc.mov(gDst.r32(), at(4));
			break; // zero-extends
		case Type::i32:
			cc.movsxd(gDst.r64(), at(4));
			break;
		case Type::i64:
		case Type::ui64:
		case Type::ptr:
			cc.mov(gDst.r64(), at(8));
			break;
		default:
			cc.mov(gDst.r64(), at(8));
			break;
		}
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitStore(ir::StoreOperation* op, RegisterFrame& frame) {
	auto valReg = regOperand(op->getValue(), frame);
	const auto at = [&](uint32_t size) {
		return memOperand(op->getAddress(), size, frame);
	};

	if (op->getValue()->getStamp() == Type::f32) {
		cc.movss(at(4), toXmm(valReg));
		return;
	}
	if (op->getValue()->getStamp() == Type::f64) {
		cc.movsd(at(8), toXmm(valReg));
		return;
	}

	auto valGp = toGp(valReg);
	switch (op->getValue()->getStamp()) {
	case Type::b:
	case Type::i8:
	case Type::ui8:
		cc.mov(at(1), valGp.r8());
		break;
	case Type::i16:
	case Type::ui16:
		cc.mov(at(2), valGp.r16());
		break;
	case Type::i32:
	case Type::ui32:
		cc.mov(at(4), valGp.r32());
		break;
	default:
		cc.mov(at(8), valGp.r64());
		break;
	}
}

void AsmJitLoweringProvider::LoweringContext::visitAlloca(ir::AllocaOperation* op, RegisterFrame& frame) {
	// Stack slots were created in the function prologue from the alloca
	// table; this op just rebinds its identifier to the corresponding
	// pointer register.
	auto index = op->getIndex();
	assert(index < functionAllocaSlots_.size() && "AllocaOperation index out of range for function");
	bindResult(op->getIdentifier(), functionAllocaSlots_[index], frame);
}

// ── External function calls ───────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitCall(ir::CallOperation* op, RegisterFrame& frame) {
	// Check the intrinsic manager first. A registered handler can fully
	// replace the scalar function-call lowering with native instructions
	// (e.g. emit `paddd xmm0, xmm1` for vector_add_i32x4_impl). The handler
	// is expected to bind the result identifier into the frame itself.
	// The linkage was decided when the callee was interned, so this is an id
	// lookup rather than a match against the raw address. Finding no handler
	// is not an error: the target keeps its address, and falling through
	// emits the ordinary call to it.
	if (auto intrinsic = intrinsicManager_.getIntrinsic(ir->getFunctionTable().get(op->getCalleeId()).getIntrinsic())) {
		// Intrinsic handlers read argument registers straight from the frame,
		// so materialise-and-bind any deferred constant argument first.
		for (auto* arg : op->getInputArguments()) {
			if (!frame.contains(arg->getIdentifier())) {
				frame.setValue(arg->getIdentifier(), regOperand(arg, frame));
			} else if (isDirty(arg) && !(enableConstFolding_ && foldableConstValue(arg))) {
				// Handlers expect canonical registers. Re-extending in place is
				// safe: the canonical pattern is a valid encoding of a dirty
				// value for every other reader too.
				narrowToStamp(toGp(frame.getValue(arg->getIdentifier())), arg->getStamp());
			}
		}
		IntrinsicCallContext ctx {cc, op, frame};
		if ((*intrinsic)(ctx)) {
			return;
		}
	}

	if (tryInlineCall(op, frame)) {
		return;
	}

	// Build the callee's signature dynamically from the IR's type information.
	// Internal Nautilus functions (resolved via a JIT label, whose function
	// pointer is not a callable address) are never capture sites; only external
	// calls need the capture thunk (mirrors TBC's visitCall).
	auto it = funcNodes_.find(op->getCalleeId());
	const bool isInternal = it != funcNodes_.end();
	const bool needsCapture = !isInternal && transport_.callNeedsCaptureThunk(op);
	const bool needsCheck = callNeedsCapture(op);
	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	if (needsCapture) {
		sig.addArg(TypeId::kUIntPtr); // raw target as the thunk's first argument
	}
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

	// Resolve argument registers BEFORE emitting the InvokeNode: a deferred
	// constant rematerialises with a `mov reg, imm`, which must precede the
	// call in the instruction stream.
	// External callees get canonical narrow arguments (the SysV ABI leaves
	// bits above 32 undefined, but compilers expect 8/16-bit arguments
	// extended to 32 bits); internal callees re-extend in their prologue.
	std::vector<AsmReg> argRegs;
	argRegs.reserve(op->getInputArguments().size());
	for (auto* arg : op->getInputArguments()) {
		argRegs.push_back(isInternal || isFloatType(arg->getStamp()) ? regOperand(arg, frame)
		                                                             : AsmReg(cleanGpOperand(arg, frame)));
	}

	InvokeNode* invokeNode = nullptr;
	if (it != funcNodes_.end()) {
		// Forward reference via AsmJit label — resolved at finalize() time.
		cc.invoke(&invokeNode, it->second->label(), sig);
	} else if (needsCapture) {
		// Captured-exception call site: invoke the captureThrowingCall<R,
		// Args...> thunk — a real C++ frame — instead of the raw target, so
		// an exception is caught before it crosses the unwind-table-less JIT
		// frame. The raw target becomes the thunk's first argument.
		void* thunk = resolveCaptureThunk(op);
		// Emit the raw-target `mov` BEFORE invoking the thunk. AsmJit cannot
		// hoist a def that appears after the invoke into the invoke's argument
		// register; the argument value must already be live when the call is
		// emitted (mirrors how the regular argument registers are resolved).
		auto targetReg = cc.newIntPtr();
		cc.mov(targetReg, reinterpret_cast<uint64_t>(op->getFunctionPtr()));
		cc.invoke(&invokeNode, reinterpret_cast<uint64_t>(thunk), sig);
		// Argument 0: the raw target pointer (compiled as a constant).
		invokeNode->setArg(0, targetReg);
	} else {
		// External function (not a Nautilus IR function): call by raw address.
		cc.invoke(&invokeNode, reinterpret_cast<uint64_t>(op->getFunctionPtr()), sig);
	}
	for (size_t i = 0; i < argRegs.size(); i++) {
		if (std::holds_alternative<Xmm>(argRegs[i]))
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toXmm(argRegs[i]));
		else
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toGp(argRegs[i]));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Xmm>(result)) {
			invokeNode->setRet(0, toXmm(result));
		} else {
			invokeNode->setRet(0, toGp(result));
			// The ABI leaves the upper bits of a narrow integer return value
			// unspecified (the callee only guarantees the stamp's own width),
			// so re-establish the extension invariant before anything reads
			// the full 64-bit register.
			narrowToStamp(toGp(result), op->getStamp());
		}
		bindResult(op->getIdentifier(), result, frame);
	}

	if (needsCheck) {
		emitCheckPendingException(op);
	}
}

void AsmJitLoweringProvider::LoweringContext::visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame) {
	// Build callee signature from IR type information.
	const bool needsCapture = transport_.callNeedsCaptureThunk(op);
	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	if (needsCapture) {
		sig.addArg(TypeId::kUIntPtr); // raw target as the thunk's first argument
	}
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

	// The function pointer is a runtime GP register value.
	auto fnPtrGp = gpOperand(op->getFunctionPtrOperand(), frame);

	// Resolve argument registers BEFORE emitting the InvokeNode (see
	// visitCall): rematerialisation movs must precede the call.
	const auto inputArgs = op->getInputArguments();
	std::vector<AsmReg> argRegs;
	argRegs.reserve(inputArgs.size());
	for (auto* arg : inputArgs) {
		argRegs.push_back(isFloatType(arg->getStamp()) ? regOperand(arg, frame) : AsmReg(cleanGpOperand(arg, frame)));
	}

	InvokeNode* invokeNode = nullptr;
	if (needsCapture) {
		// Captured-exception call site: route through the capture thunk, with
		// the runtime function pointer as its first argument (see visitCall).
		void* thunk = resolveCaptureThunk(op);
		cc.invoke(&invokeNode, reinterpret_cast<uint64_t>(thunk), sig);
	} else {
		cc.invoke(&invokeNode, fnPtrGp, sig);
	}

	if (needsCapture) {
		// Argument 0: the raw target (the runtime function-pointer register).
		invokeNode->setArg(0, fnPtrGp);
	}
	for (size_t i = 0; i < argRegs.size(); i++) {
		if (std::holds_alternative<Xmm>(argRegs[i]))
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toXmm(argRegs[i]));
		else
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toGp(argRegs[i]));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Xmm>(result)) {
			invokeNode->setRet(0, toXmm(result));
		} else {
			invokeNode->setRet(0, toGp(result));
			// See visitCall: narrow integer returns arrive with
			// unspecified upper bits and must be re-extended.
			narrowToStamp(toGp(result), op->getStamp());
		}
		bindResult(op->getIdentifier(), result, frame);
	}

	if (needsCapture) {
		emitCheckPendingException(op);
	}
}

void AsmJitLoweringProvider::LoweringContext::visitFunctionAddressOf(ir::FunctionAddressOfOperation* op,
                                                                     RegisterFrame& frame) {
	auto reg = allocReg(Type::ptr);
	auto it = funcNodes_.find(op->getCalleeId());
	if (it != funcNodes_.end()) {
		// Load the JIT function's address via RIP-relative LEA — resolved at finalize().
		cc.lea(toGp(reg), x86::ptr(it->second->label()));
	} else {
		// External function: embed the raw pointer as a compile-time constant.
		cc.mov(toGp(reg), reinterpret_cast<uint64_t>(op->getFunctionPtr()));
	}
	bindResult(op->getIdentifier(), reg, frame);
}

// ── Type conversion ───────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitCast(ir::CastOperation* op, RegisterFrame& frame) {
	// An integer cast of a constant chain is itself a deferred constant (see
	// foldableConstValue); emit nothing and let consumers fold or
	// rematerialise the pre-computed value. Bound identifiers double as
	// merge-block parameters and must still be written (issue #321).
	if (enableConstFolding_ && !frame.contains(op->getIdentifier()) && foldableConstValue(op).has_value()) {
		return;
	}
	const Type srcType = op->getInput()->getStamp();
	const Type dstType = op->getStamp();
	const bool srcIsFloat = isFloatType(srcType);
	const bool dstIsFloat = isFloatType(dstType);
	// Integer sources are read at their own width below (extension, test),
	// except by the int->float conversions, which read all 64 bits.
	auto src =
	    !srcIsFloat && dstIsFloat ? AsmReg(cleanGpOperand(op->getInput(), frame)) : regOperand(op->getInput(), frame);
	auto result = allocReg(dstType);

	if (dstType == Type::b && srcType != Type::b) {
		// A cast to bool is `value != 0` (C++ semantics), not a truncation to
		// the low byte: 256 and 0.5 are true. Only the source width is tested.
		auto gDst = toGp(result);
		if (srcIsFloat) {
			auto xSrc = toXmm(src);
			auto xZero = srcType == Type::f32 ? cc.newXmmSs() : cc.newXmmSd();
			auto unordered = cc.newInt64();
			cc.xorps(xZero, xZero);
			if (srcType == Type::f32) {
				cc.ucomiss(xSrc, xZero);
			} else {
				cc.ucomisd(xSrc, xZero);
			}
			// ZF is also set for NaN, which is true in C++; PF flags it.
			cc.setne(gDst.r8());
			cc.setp(unordered.r8());
			cc.or_(gDst.r8(), unordered.r8());
		} else {
			auto gSrc = toGp(src);
			switch (srcType) {
			case Type::i8:
			case Type::ui8:
				cc.test(gSrc.r8(), gSrc.r8());
				break;
			case Type::i16:
			case Type::ui16:
				cc.test(gSrc.r16(), gSrc.r16());
				break;
			case Type::i32:
			case Type::ui32:
				cc.test(gSrc.r32(), gSrc.r32());
				break;
			default:
				cc.test(gSrc.r64(), gSrc.r64());
				break;
			}
			cc.setne(gDst.r8());
		}
		cc.movzx(gDst.r32(), gDst.r8());
		bindResult(op->getIdentifier(), result, frame);
		return;
	}

	if (!srcIsFloat && !dstIsFloat) {
		// Integer → integer: first extend from source width to get a clean
		// 64-bit representation, then narrow to destination width. The second
		// step matters whenever dstWidth <= srcWidth (truncating, or a
		// same-width signedness change like i16->ui16): the value is then
		// defined by its low dstWidth bits, and those must be re-extended per
		// the *destination*'s signedness, not the source's -- e.g. casting
		// i16(-1) to ui16 must zero-extend the resulting 0xFFFF to produce
		// 65535, not sign-extend it to 0xFFFFFFFFFFFFFFFF. Mirrors
		// A64LoweringProvider::visitCast's two-step extend-then-narrow.
		auto gSrc = toGp(src);
		auto gDst = toGp(result);
		// Step 1: extend from source width.
		switch (srcType) {
		case Type::i8:
			cc.movsx(gDst.r64(), gSrc.r8());
			break;
		case Type::b:
		case Type::ui8:
			cc.movzx(gDst.r32(), gSrc.r8());
			break; // movzx r32 zero-extends to 64
		case Type::i16:
			cc.movsx(gDst.r64(), gSrc.r16());
			break;
		case Type::ui16:
			cc.movzx(gDst.r32(), gSrc.r16());
			break; // movzx r32 zero-extends to 64
		case Type::i32:
			cc.movsxd(gDst.r64(), gSrc.r32());
			break;
		case Type::ui32:
			cc.mov(gDst.r32(), gSrc.r32());
			break; // zero-extends to 64
		default:
			cc.mov(gDst, gSrc);
			break;
		}
		narrowToStamp(gDst, dstType);
	} else if (srcIsFloat && !dstIsFloat) {
		// Float → integer: truncate toward zero, then narrow to the destination
		// width so the value is defined by its low dstWidth bits (see the
		// integer→integer path above). Mirrors A64LoweringProvider::visitCast
		// (which uses fcvtzu/fcvtzs and needs no ui64 special case).
		auto gDst = toGp(result);
		auto xSrc = toXmm(src);
		if (dstType == Type::ui64) {
			// cvttss2si/cvttsd2si is signed; inputs in [2^63, 2^64) would
			// overflow to the indefinite value 2^63 (issue #328). Standard
			// unsigned sequence: below 2^63 convert directly; otherwise
			// subtract 2^63 in the float domain (exact, 2^63 is representable
			// in f32 and f64), convert, and set bit 63 back. Narrower unsigned
			// destinations fit the signed conversion and need no special case.
			Label big = cc.newLabel();
			Label done = cc.newLabel();
			if (srcType == Type::f32) {
				auto threshold = cc.newFloatConst(ConstPoolScope::kLocal, 9223372036854775808.0f);
				cc.ucomiss(xSrc, threshold);
				cc.jae(big);
				cc.cvttss2si(gDst.r64(), xSrc);
				cc.jmp(done);
				cc.bind(big);
				auto xAdj = cc.newXmmSs();
				cc.movss(xAdj, xSrc);
				cc.subss(xAdj, threshold);
				cc.cvttss2si(gDst.r64(), xAdj);
			} else {
				auto threshold = cc.newDoubleConst(ConstPoolScope::kLocal, 9223372036854775808.0);
				cc.ucomisd(xSrc, threshold);
				cc.jae(big);
				cc.cvttsd2si(gDst.r64(), xSrc);
				cc.jmp(done);
				cc.bind(big);
				auto xAdj = cc.newXmmSd();
				cc.movsd(xAdj, xSrc);
				cc.subsd(xAdj, threshold);
				cc.cvttsd2si(gDst.r64(), xAdj);
			}
			cc.btc(gDst.r64(), 63);
			cc.bind(done);
		} else {
			if (srcType == Type::f32)
				cc.cvttss2si(gDst.r64(), xSrc);
			else
				cc.cvttsd2si(gDst.r64(), xSrc);
			narrowToStamp(gDst, dstType);
		}
	} else if (!srcIsFloat && dstIsFloat) {
		// Integer → float.
		auto gSrc = toGp(src);
		auto xDst = toXmm(result);
		if (srcType == Type::ui64) {
			// cvtsi2ss/cvtsi2sd treats input as signed; values >= 2^63 need special handling.
			Label done = cc.newLabel();
			Label big = cc.newLabel();
			auto tmp = cc.newInt64();
			auto lowBit = cc.newInt64();
			cc.test(gSrc, gSrc);
			cc.js(big);
			// High bit not set: fits in signed int64, convert directly.
			if (dstType == Type::f32)
				cc.cvtsi2ss(xDst, gSrc.r64());
			else
				cc.cvtsi2sd(xDst, gSrc.r64());
			cc.jmp(done);
			cc.bind(big);
			// Shift right by 1, preserving the low bit, convert, then double.
			cc.mov(tmp, gSrc);
			cc.mov(lowBit, gSrc);
			cc.and_(lowBit, 1);
			cc.shr(tmp, 1);
			cc.or_(tmp, lowBit);
			if (dstType == Type::f32) {
				cc.cvtsi2ss(xDst, tmp.r64());
				cc.addss(xDst, xDst);
			} else {
				cc.cvtsi2sd(xDst, tmp.r64());
				cc.addsd(xDst, xDst);
			}
			cc.bind(done);
		} else {
			if (dstType == Type::f32)
				cc.cvtsi2ss(xDst, gSrc.r64());
			else
				cc.cvtsi2sd(xDst, gSrc.r64());
		}
	} else {
		// Float → float.
		auto xSrc = toXmm(src);
		auto xDst = toXmm(result);
		if (srcType == Type::f32 && dstType == Type::f64)
			cc.cvtss2sd(xDst, xSrc);
		else
			cc.cvtsd2ss(xDst, xSrc);
	}
	bindResult(op->getIdentifier(), result, frame);
}

// ── Captured-exception transport ─────────────────────────────────────────────
// Potentially-throwing calls are routed through captureThrowingCall<R, Args...>
// (a real C++ frame) and followed by a pending-exception check that branches to
// the call's landing pad. Pads and the exceptional exit are emitted after the
// main CFG, mirroring the BC/TBC/CPP captured backends.

bool AsmJitLoweringProvider::LoweringContext::callNeedsCapture(const ir::Operation* call) const {
	if (currentFunction_ == nullptr) {
		return false;
	}
	return transport_.callNeedsCapture(call);
}

const ir::LandingPadBlock* AsmJitLoweringProvider::LoweringContext::getPadForCall(const ir::Operation* call) const {
	if (currentFunction_ == nullptr) {
		return nullptr;
	}
	return transport_.getPadForCall(call);
}

void* AsmJitLoweringProvider::LoweringContext::resolveCaptureThunk(const ir::Operation* call) const {
	void* thunk = CapturedExceptionTransport::captureThunkFor(call);
	if (thunk == nullptr) {
		throw NotImplementedException("asmjit: captured-exception call has no recorded capture wrapper");
	}
	return thunk;
}

void AsmJitLoweringProvider::LoweringContext::emitCheckPendingException(const ir::Operation* call) {
	const auto padIndex = transport_.getPadIndexForCall(call);
	const Label target = padIndex != ir::noLandingPad ? getPadLabel(padIndex) : getExceptionalExitLabel();

	// frame = currentExceptionFrame()
	FuncSignature frameSig;
	frameSig.setRet(TypeId::kUIntPtr);
	InvokeNode* frameInvoke = nullptr;
	cc.invoke(&frameInvoke, reinterpret_cast<uint64_t>(&currentExceptionFrame), frameSig);
	auto frameReg = cc.newIntPtr();
	frameInvoke->setRet(0, frameReg);

	// Load the pending field and branch to the pad when it is non-null. The
	// TLS frame is always non-null inside a JIT-invoked function (the
	// Invocable wrapper pushes it), so no null check is needed here.
	auto pendingReg = cc.newIntPtr();
	cc.mov(pendingReg, x86::qword_ptr(frameReg, offsetof(ExceptionFrame, pending)));
	cc.test(pendingReg, pendingReg);
	cc.jne(target);
}

::asmjit::Label AsmJitLoweringProvider::LoweringContext::getPadLabel(size_t padIndex) {
	auto it = padLabels_.find(padIndex);
	if (it != padLabels_.end()) {
		return it->second;
	}
	auto label = cc.newLabel();
	padLabels_[padIndex] = label;
	return label;
}

::asmjit::Label AsmJitLoweringProvider::LoweringContext::getExceptionalExitLabel() {
	if (!exceptionalExitLabel_.isValid()) {
		exceptionalExitLabel_ = cc.newLabel();
	}
	return exceptionalExitLabel_;
}

void AsmJitLoweringProvider::LoweringContext::lowerExceptionPads(RegisterFrame& frame) {
	if (currentFunction_ == nullptr || !transport_.hasExceptionalCallSites()) {
		return;
	}

	const auto& pads = currentFunction_->exceptionRegion->pads;
	for (size_t padIndex = 0; padIndex < pads.size(); ++padIndex) {
		const auto& pad = pads[padIndex];
		cc.bind(getPadLabel(padIndex));
		// Pads contain only destructor CallOperations (all noUnwind), so
		// lowering them through the normal dispatch emits plain external calls
		// with no re-entrant capture.
		for (auto* op : pad.block->getOperations()) {
			dispatch(op, frame);
		}
		cc.jmp(getExceptionalExitLabel());
	}

	// Exceptional exit: return the ABI default for the function's return type.
	cc.bind(getExceptionalExitLabel());
	emitDefaultReturn(currentFunction_->getOutputArg());
}

void AsmJitLoweringProvider::LoweringContext::emitDefaultReturn(Type retType) {
	if (retType == Type::v) {
		cc.ret();
	} else if (retType == Type::f32 || retType == Type::f64) {
		auto xmm = retType == Type::f32 ? cc.newXmmSs() : cc.newXmmSd();
		if (retType == Type::f32) {
			cc.xorps(xmm, xmm);
		} else {
			cc.xorpd(xmm, xmm);
		}
		cc.ret(xmm);
	} else {
		// Integers, bools, and pointers: return 0 (false / nullptr).
		auto reg = cc.newInt64();
		cc.xor_(reg, reg);
		cc.ret(reg);
	}
}

} // namespace nautilus::compiler::asmjit
