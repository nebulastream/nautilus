
#include "nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/common/ExceptionTransport.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/backends/CapturedExceptionTransport.hpp"
#include "nautilus/compiler/ir/Usages.hpp"
#include "nautilus/compiler/ir/operations/DestructorOperands.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <algorithm>
#include <asmjit/arm/armutils.h>
#include <bit>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>

namespace nautilus::compiler::asmjit {

using namespace ::asmjit;
using namespace ::asmjit::a64;

// ── AsmJitLoweringProvider ────────────────────────────────────────────────────

namespace {
class ThrowOnError : public ::asmjit::ErrorHandler {
public:
	void handleError(::asmjit::Error err, const char* message, ::asmjit::BaseEmitter* /*origin*/) override {
		fprintf(stderr, "[AsmJit/A64] Error %u: %s\n", err, message ? message : "(null)");
		throw std::runtime_error(std::string("AsmJit/A64 error: ") + (message ? message : "unknown"));
	}
};

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

// add/sub/cmp/cmn immediates: 12 bits, optionally shifted left by 12.
bool isAddSubImm(uint64_t value) {
	return arm::Utils::isAddSubImm(value);
}

// and/orr/eor/tst immediates on a 64-bit register (a repeating bit pattern;
// neither all-zeros nor all-ones is encodable).
bool isLogicalImm64(int64_t value) {
	return arm::Utils::isLogicalImm(static_cast<uint64_t>(value), 64);
}

// The immediate operands encoding an add/sub-class @p value: `#value` or
// `#(value >> 12), lsl #12`. Callers check isAddSubImm first.
template <typename Emit>
void withAddSubImm(uint64_t value, Emit&& emit) {
	if (value <= 0xFFFu) {
		emit(Imm(value), std::optional<Imm> {});
	} else {
		emit(Imm(value >> 12), std::optional<Imm> {Imm(arm::lsl(12))});
	}
}
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
	if (options.getOptionOrDefault<bool>("asmjit.enablePostRAPeephole", true)) {
		cc.addPassT<A64PostRAPeepholePass>(statistics);
	}
	enableBranchFusion_ = options.getOptionOrDefault<bool>("asmjit.enableBranchFusion", true);
	enableConstFolding_ = options.getOptionOrDefault<bool>("asmjit.enableConstFolding", true);
	enableSelectCmov_ = options.getOptionOrDefault<bool>("asmjit.enableSelectCmov", true);
	enableLazyNarrowing_ = options.getOptionOrDefault<bool>("asmjit.enableLazyNarrowing", true);
	enableAddressFusion_ = options.getOptionOrDefault<bool>("asmjit.enableAddressFusion", true);
	enableInlining_ = options.getOptionOrDefault<bool>("asmjit.enableInlining", true);
	enableLoopRotation_ = options.getOptionOrDefault<bool>("asmjit.enableLoopRotation", true);
	enableTailDuplication_ = options.getOptionOrDefault<bool>("asmjit.enableTailDuplication", true);
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
		cc.sxtb(reg.x(), reg.w());
		break;
	case Type::b:
	case Type::ui8:
		cc.uxtb(reg.w(), reg.w());
		break;
	case Type::i16:
		cc.sxth(reg.x(), reg.w());
		break;
	case Type::ui16:
		cc.uxth(reg.w(), reg.w());
		break;
	case Type::i32:
		cc.sxtw(reg.x(), reg.w());
		break;
	case Type::ui32:
		cc.mov(reg.w(), reg.w()); // zero-extends upper 32 bits
		break;
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
// Mirrors X64LoweringProvider.

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
		cc.sxtb(dst.x(), src.w());
		break;
	case Type::b:
	case Type::ui8:
		cc.uxtb(dst.w(), src.w());
		break;
	case Type::i16:
		cc.sxth(dst.x(), src.w());
		break;
	case Type::ui16:
		cc.uxth(dst.w(), src.w());
		break;
	case Type::i32:
		cc.sxtw(dst.x(), src.w());
		break;
	case Type::ui32:
		cc.mov(dst.w(), src.w()); // zero-extends upper 32 bits
		break;
	default:
		cc.mov(dst.x(), src.x());
		break;
	}
}

Gp AsmJitLoweringProvider::LoweringContext::gpOperandAtWidth(const ir::Operation* in, uint32_t width,
                                                             RegisterFrame& frame) {
	auto reg = gpOperand(in, frame);
	// A rematerialised constant is always canonical.
	if (stampBits(in->getStamp()) < width && isDirty(in) && !constOperand(in).has_value()) {
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
//
// AArch64 addresses are `[base, #disp]` (a signed 9-bit displacement, or an
// unsigned 12-bit one scaled by the access size) or `[base, index{, lsl #k}]`
// with k either 0 or log2 of the access size. A materialised scaled add is one
// shifted-register `add`, for any shift.

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

	AddressParts parts;
	if (const auto disp = constOperand(offset)) {
		if (constOperand(base).has_value() || *disp < INT32_MIN || *disp > INT32_MAX) {
			return std::nullopt;
		}
		parts.base = base;
		parts.disp = static_cast<int32_t>(*disp);
		return parts;
	}
	// For an integer add either side may be the scaled one.
	if (stamp != Type::ptr && !matchScaled(offset).has_value() && matchScaled(base).has_value()) {
		std::swap(base, offset);
	}
	parts.base = base;
	parts.offset = offset;
	if (const auto scaled = matchScaled(offset)) {
		parts.scaledInput = scaled->first;
		parts.shift = scaled->second;
	}
	return parts;
}

std::optional<std::pair<const ir::Operation*, uint32_t>>
AsmJitLoweringProvider::LoweringContext::matchScaled(const ir::Operation* op) {
	// The scaled input must be a full-width register value (a constant would
	// have been folded by the IR passes).
	const auto isScalable = [&](const ir::Operation* x) {
		return stampBits(x->getStamp()) == 64 && !isFloatType(x->getStamp()) && !constOperand(x).has_value();
	};
	if (const auto* mul = ir::dyn_cast<ir::MulOperation>(op)) {
		for (int side = 0; side < 2; side++) {
			const auto* x = side == 0 ? mul->getLeftInput() : mul->getRightInput();
			const auto* c = side == 0 ? mul->getRightInput() : mul->getLeftInput();
			const auto value = constOperand(c);
			if (value.has_value() && isScalable(x) && *value > 0 &&
			    std::has_single_bit(static_cast<uint64_t>(*value))) {
				return std::make_pair(x, static_cast<uint32_t>(std::countr_zero(static_cast<uint64_t>(*value))));
			}
		}
	} else if (const auto* shl = ir::dyn_cast<ir::ShiftOperation>(op)) {
		const auto count = constOperand(shl->getRightInput());
		const auto* x = shl->getLeftInput();
		if (shl->getType() == ir::ShiftOperation::LS && count.has_value() && *count >= 0 && *count <= 63 &&
		    isScalable(x)) {
			return std::make_pair(x, static_cast<uint32_t>(*count));
		}
	}
	return std::nullopt;
}

const ir::CastOperation* AsmJitLoweringProvider::LoweringContext::asWidenedIndex(const ir::Operation* op) {
	const auto* cast = ir::dyn_cast<ir::CastOperation>(op);
	if (cast == nullptr) {
		return nullptr;
	}
	const Type src = cast->getInput()->getStamp();
	const Type dst = cast->getStamp();
	return (src == Type::i32 || src == Type::ui32) && (dst == Type::i64 || dst == Type::ui64) ? cast : nullptr;
}

AsmJitLoweringProvider::LoweringContext::IndexOperand
AsmJitLoweringProvider::LoweringContext::indexOperand(const ir::Operation* index, RegisterFrame& frame) {
	if (const auto* cast = asWidenedIndex(index); cast != nullptr && isDeferredAddressPart(index)) {
		// Only the low 32 bits are read, so a dirty source is fine.
		return {gpOperand(cast->getInput(), frame).w(), true, cast->getInput()->getStamp() == Type::i32};
	}
	return {gpOperand(index, frame), false, false};
}

std::pair<const ir::Operation*, uint32_t>
AsmJitLoweringProvider::LoweringContext::addressIndex(const AddressParts& parts) const {
	if (parts.scaledInput != nullptr && isDeferredAddressPart(parts.offset)) {
		return {parts.scaledInput, parts.shift};
	}
	return {parts.offset, 0};
}

bool AsmJitLoweringProvider::LoweringContext::fitsAccess(const AddressParts& parts, uint32_t size) {
	if (parts.offset == nullptr) {
		const int64_t disp = parts.disp;
		return (disp >= -256 && disp <= 255) || (disp >= 0 && disp % size == 0 && disp / size < 4096);
	}
	// The index of a scaled form is shifted by the access size or not at all.
	// (When the scaling op ends up materialised the index is unshifted, which
	// always fits; requiring the scaled form to fit is merely conservative.)
	return parts.scaledInput == nullptr || parts.shift == 0 || (parts.shift <= 3 && (1u << parts.shift) == size);
}

namespace {
// Bytes accessed by a load or store of a @p stamp value.
uint32_t accessSize(Type stamp) {
	switch (stamp) {
	case Type::b:
	case Type::i8:
	case Type::ui8:
		return 1;
	case Type::i16:
	case Type::ui16:
		return 2;
	case Type::i32:
	case Type::ui32:
	case Type::f32:
		return 4;
	default:
		return 8;
	}
}
} // anonymous namespace

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
			fusionUses_.resize(id + 1, 0x7);
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
	// Bit 0 for an address operand of an access of @p size bytes: only when the
	// address, if it is a matched add, fits that access's memory operand.
	const auto addressBit = [&](const ir::Operation* address, uint32_t size) -> uint8_t {
		const auto parts = matchAddress(address);
		return parts.has_value() && fitsAccess(*parts, size) ? 0x1 : 0x0;
	};
	for (const auto* block : blocks) {
		for (const auto* op : block->getOperations()) {
			const auto* load = ir::dyn_cast<ir::LoadOperation>(op);
			const auto* store = ir::dyn_cast<ir::StoreOperation>(op);
			const auto parts = ir::dyn_cast<ir::AddOperation>(op) != nullptr ? matchAddress(op) : std::nullopt;
			const auto scaled = matchScaled(op);
			for (const auto* in : op->getInputs()) {
				uint8_t allowed = 0;
				if (load != nullptr && in == load->getAddress()) {
					allowed |= addressBit(in, accessSize(load->getStamp()));
				} else if (store != nullptr && in == store->getAddress() && in != store->getValue()) {
					allowed |= addressBit(in, accessSize(store->getValue()->getStamp()));
				}
				if (parts.has_value() && parts->scaledInput != nullptr && in == parts->offset) {
					allowed |= 0x2;
				}
				if ((scaled.has_value() && in == scaled->first) ||
				    (parts.has_value() && parts->scaledInput == nullptr && in == parts->offset)) {
					allowed |= 0x4;
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

Mem AsmJitLoweringProvider::LoweringContext::memOperand(const ir::Operation* addr, RegisterFrame& frame) {
	if (!isDeferredAddressPart(addr)) {
		return a64::ptr(gpOperand(addr, frame));
	}
	const auto parts = *matchAddress(addr);
	fusedAddresses_++;
	auto base = gpOperand(parts.base, frame);
	if (parts.offset == nullptr) {
		return a64::ptr(base, parts.disp);
	}
	const auto [indexOp, shift] = addressIndex(parts);
	const auto index = indexOperand(indexOp, frame);
	if (index.extended) {
		return a64::ptr(base, index.reg, index.isSigned ? a64::sxtw(shift) : a64::uxtw(shift));
	}
	return shift == 0 ? a64::ptr(base, index.reg) : a64::ptr(base, index.reg, arm::lsl(shift));
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
	// A constant argument stays a constant: the body folds or rematerialises
	// it like any other deferred constant, and never reads it from the frame.
	RegisterFrame calleeFrame;
	for (size_t i = 0; i < params.size(); i++) {
		if (enableConstFolding_ && !isFloatType(args[i]->getStamp())) {
			if (const auto value = foldableConstValue(args[i])) {
				inlinedConstParams_[params[i]] = canonicalizeToStamp(*value, params[i]->getStamp());
				continue;
			}
		}
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

	inlinedConstParams_.clear();
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

AsmJitLoweringProvider::AsmReg AsmJitLoweringProvider::LoweringContext::allocReg(Type t) {
	if (t == Type::f32)
		return cc.newVecS();
	if (t == Type::f64)
		return cc.newVecD();
	return cc.newInt64();
}

Gp AsmJitLoweringProvider::LoweringContext::toGp(const AsmReg& r) {
	return std::get<Gp>(r);
}

Vec AsmJitLoweringProvider::LoweringContext::toVec(const AsmReg& r) {
	return std::get<Vec>(r);
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
	if (std::holds_alternative<Vec>(dst)) {
		cc.fmov(toVec(dst), toVec(src));
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

std::optional<int64_t> AsmJitLoweringProvider::LoweringContext::foldableConstValue(const ir::Operation* in) {
	if (!inlinedConstParams_.empty()) {
		if (const auto it = inlinedConstParams_.find(in); it != inlinedConstParams_.end()) {
			return it->second;
		}
	}
	if (!constBlockParams_.empty()) {
		if (const auto it = constBlockParams_.find(in); it != constBlockParams_.end()) {
			return it->second;
		}
	}
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

std::optional<int64_t> AsmJitLoweringProvider::LoweringContext::constOperand(const ir::Operation* in) {
	if (!enableConstFolding_) {
		return std::nullopt;
	}
	return foldableConstValue(in);
}

Gp AsmJitLoweringProvider::LoweringContext::gpOperand(const ir::Operation* in, RegisterFrame& frame) {
	// A constant operand is recovered from the operation itself, IGNORING any
	// frame binding: the constant's identifier can be shadowed by a
	// merge-block parameter register that carries a different value on other
	// paths (issue #321), while an SSA input edge to a constant operation
	// always means that constant.
	if (const auto value = constOperand(in)) {
		auto reg = cc.newInt64();
		cc.mov(reg, *value);
		return reg;
	}
	// A deferred widening cast read as a full register after all (its consumer
	// could not fold it): rematerialise the extension from its source, whose
	// register is unchanged within the cast's block (see address fusion).
	if (const auto* cast = asWidenedIndex(in); cast != nullptr && isDeferredAddressPart(in)) {
		auto reg = cc.newInt64();
		extendFromStamp(reg, gpOperand(cast->getInput(), frame), cast->getInput()->getStamp());
		return reg;
	}
	return toGp(frame.getValue(in->getIdentifier()));
}

AsmJitLoweringProvider::AsmReg AsmJitLoweringProvider::LoweringContext::regOperand(const ir::Operation* in,
                                                                                   RegisterFrame& frame) {
	if (constOperand(in).has_value()) {
		return AsmReg(gpOperand(in, frame));
	}
	return frame.getValue(in->getIdentifier());
}

void AsmJitLoweringProvider::LoweringContext::emitMoveFromOperand(const AsmReg& dst, const ir::Operation* src,
                                                                  RegisterFrame& frame) {
	if (const auto value = constOperand(src)) {
		cc.mov(toGp(dst), *value);
		return;
	}
	emitMove(dst, frame.getValue(src->getIdentifier()));
}

void AsmJitLoweringProvider::LoweringContext::emitAddImmediate(Gp dst, Gp src, int64_t value) {
	const auto magnitude = static_cast<uint64_t>(value);
	const auto negated = uint64_t {0} - magnitude;
	if (isAddSubImm(magnitude)) {
		withAddSubImm(magnitude, [&](const Imm& imm, const std::optional<Imm>& shift) {
			shift ? cc.add(dst, src, imm, *shift) : cc.add(dst, src, imm);
		});
		foldedImmediates_++;
	} else if (isAddSubImm(negated)) {
		withAddSubImm(negated, [&](const Imm& imm, const std::optional<Imm>& shift) {
			shift ? cc.sub(dst, src, imm, *shift) : cc.sub(dst, src, imm);
		});
		foldedImmediates_++;
	} else {
		auto tmp = cc.newInt64();
		cc.mov(tmp, value);
		cc.add(dst, src, tmp);
	}
}

// A block parameter that every incoming edge feeds the same constant (e.g. a
// loop increment both arms of a diamond pass to their merge block) is that
// constant. Edges that feed the parameter back to itself (a loop-invariant
// constant carried around a loop) do not count against it.
void AsmJitLoweringProvider::LoweringContext::computeConstBlockParams(const ir::FunctionOperation* funcOp) {
	constBlockParams_.clear();
	if (!enableConstFolding_) {
		return;
	}
	std::unordered_map<const ir::Operation*, std::vector<const ir::Operation*>> incoming;
	const auto addEdge = [&](const ir::BasicBlockInvocation& bi) {
		const auto& srcArgs = bi.getArguments();
		const auto& dstArgs = bi.getBlock()->getArguments();
		for (size_t i = 0; i < srcArgs.size() && i < dstArgs.size(); i++) {
			incoming[dstArgs[i]].push_back(srcArgs[i]);
		}
	};
	for (const auto* block : funcOp->getBasicBlocks()) {
		const auto& ops = block->getOperations();
		if (ops.empty()) {
			continue;
		}
		if (const auto* ifOp = ir::dyn_cast<ir::IfOperation>(ops.back())) {
			addEdge(ifOp->getTrueBlockInvocation());
			addEdge(ifOp->getFalseBlockInvocation());
		} else if (const auto* br = ir::dyn_cast<ir::BranchOperation>(ops.back())) {
			addEdge(br->getNextBlockInvocation());
		}
	}
	for (bool changed = true; changed;) {
		changed = false;
		for (const auto& [param, args] : incoming) {
			if (constBlockParams_.contains(param) || isFloatType(param->getStamp())) {
				continue;
			}
			std::optional<int64_t> value;
			bool constant = true;
			for (const auto* arg : args) {
				if (arg == param) {
					continue;
				}
				const auto argValue = foldableConstValue(arg);
				if (!argValue.has_value() ||
				    (value.has_value() && *value != canonicalizeToStamp(*argValue, param->getStamp()))) {
					constant = false;
					break;
				}
				value = canonicalizeToStamp(*argValue, param->getStamp());
			}
			if (constant && value.has_value()) {
				constBlockParams_[param] = *value;
				changed = true;
			}
		}
	}
}

// An accumulator updated as `acc = (acc + a) - b` serialises two operations
// per iteration on acc; `acc + (a - b)` leaves one, with `a - b` computed off
// the chain. Two's-complement add/sub are associative, so the low 64 bits
// agree in every case. The parameter is the operand recognised as the chain:
// a parameter of the block, whose register only changes on its edges.
void AsmJitLoweringProvider::LoweringContext::computeReassociations(const ir::FunctionOperation* funcOp) {
	std::unordered_map<ir::OperationIdentifier, uint32_t> uses;
	const auto countBlock = [&](const ir::BasicBlock* block) {
		for (const auto* op : block->getOperations()) {
			for (const auto* in : op->getInputs()) {
				uses[in->getIdentifier()]++;
			}
			for (size_t i = 0; i < ir::getDestructorOperandCount(*op); i++) {
				uses[ir::getDestructorOperand(*op, i)->getIdentifier()]++;
			}
			if (const auto* ifOp = ir::dyn_cast<ir::IfOperation>(op)) {
				for (const auto* in : ifOp->getTrueBlockInvocation().getArguments()) {
					uses[in->getIdentifier()]++;
				}
				for (const auto* in : ifOp->getFalseBlockInvocation().getArguments()) {
					uses[in->getIdentifier()]++;
				}
			} else if (const auto* br = ir::dyn_cast<ir::BranchOperation>(op)) {
				for (const auto* in : br->getNextBlockInvocation().getArguments()) {
					uses[in->getIdentifier()]++;
				}
			}
		}
	};
	for (const auto* block : funcOp->getBasicBlocks()) {
		countBlock(block);
	}
	if (funcOp->exceptionRegion.has_value()) {
		for (const auto& pad : funcOp->exceptionRegion->pads) {
			countBlock(pad.block);
		}
	}
	const auto isWideAddSub = [](const ir::Operation* op) {
		return (ir::dyn_cast<ir::AddOperation>(op) != nullptr || ir::dyn_cast<ir::SubOperation>(op) != nullptr) &&
		       (op->getStamp() == Type::i64 || op->getStamp() == Type::ui64);
	};
	for (const auto* block : funcOp->getBasicBlocks()) {
		const auto& params = block->getArguments();
		const auto isParam = [&](const ir::Operation* op) {
			return std::find(params.begin(), params.end(), op) != params.end();
		};
		const auto& ops = block->getOperations();
		const auto definedHere = [&](const ir::Operation* op) {
			return std::find(ops.begin(), ops.end(), op) != ops.end();
		};
		for (const auto* outer : ops) {
			if (!isWideAddSub(outer)) {
				continue;
			}
			const bool outerSubtracts = ir::dyn_cast<ir::SubOperation>(outer) != nullptr;
			const auto& outerInputs = outer->getInputs();
			// A subtraction reassociates only through its left operand.
			for (size_t side = 0; side < (outerSubtracts ? 1u : 2u); side++) {
				const auto* inner = outerInputs[side];
				const auto* b = outerInputs[1 - side];
				if (!isWideAddSub(inner) || inner->getStamp() != outer->getStamp() || !definedHere(inner) ||
				    uses[inner->getIdentifier()] != 1 || isParam(b)) {
					continue;
				}
				const bool innerSubtracts = ir::dyn_cast<ir::SubOperation>(inner) != nullptr;
				const auto& innerInputs = inner->getInputs();
				for (size_t pSide = 0; pSide < (innerSubtracts ? 1u : 2u); pSide++) {
					const auto* param = innerInputs[pSide];
					const auto* a = innerInputs[1 - pSide];
					if (!isParam(param) || isParam(a) || param->getStamp() != outer->getStamp()) {
						continue;
					}
					reassociations_[outer] = {inner, param, a, b, innerSubtracts, outerSubtracts};
					reassociatedInners_.insert(inner);
					break;
				}
				if (reassociations_.contains(outer)) {
					break;
				}
			}
		}
	}
}

bool AsmJitLoweringProvider::LoweringContext::emitReassociated(const ir::Operation* op, Gp dst, RegisterFrame& frame) {
	if (reassociations_.empty()) {
		return false;
	}
	const auto it = reassociations_.find(op);
	if (it == reassociations_.end()) {
		return false;
	}
	const auto& r = it->second;
	// (p + a) + b = p + (a + b)    (p + a) - b = p + (a - b)
	// (p - a) + b = p + (b - a)    (p - a) - b = p - (a + b)
	const bool tAdds = r.innerSubtracts == r.outerSubtracts;
	const auto* tLeft = r.innerSubtracts && !r.outerSubtracts ? r.b : r.a;
	const auto* tRight = tLeft == r.a ? r.b : r.a;
	auto t = cc.newInt64();
	if (const auto rightImm = constOperand(tRight)) {
		const auto value = static_cast<uint64_t>(*rightImm);
		emitAddImmediate(t, gpOperand(tLeft, frame), static_cast<int64_t>(tAdds ? value : uint64_t {0} - value));
	} else if (tAdds) {
		cc.add(t, gpOperand(tLeft, frame), gpOperand(tRight, frame));
	} else {
		cc.sub(t, gpOperand(tLeft, frame), gpOperand(tRight, frame));
	}
	auto param = gpOperand(r.param, frame);
	if (r.innerSubtracts && r.outerSubtracts) {
		cc.sub(dst, param, t);
	} else {
		cc.add(dst, param, t);
	}
	return true;
}

// ── Two-pass compilation ──────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::processAll(std::string* asmjitIRDump) {
	const auto& functionOperations = ir->getFunctionOperations();
	if (functionOperations.empty()) {
		throw std::runtime_error("AsmJit/A64: no functions found in IR graph");
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

	// Pointer-keyed analyses that inlining reads across functions.
	for (const auto& funcOp : functionOperations) {
		computeReassociations(funcOp);
	}

	// Pass 2: emit each function body.
	for (const auto& funcOp : functionOperations) {
		const auto& funcBlock = funcOp->getFunctionBasicBlock();
		const auto& entryArgs = funcBlock.getArguments();
		auto* funcNode = funcNodes_.at(ir->getFunctionTable().findByDefinition(funcOp));

		cc.addFunc(funcNode);

		blockLabels.clear();
		processedBlocks.clear();
		functionAllocaSlots_.clear();
		currentFunction_ = funcOp;
		transport_ = CapturedExceptionTransport(*funcOp);
		padLabels_.clear();
		exceptionalExitLabel_.reset();
		computeConstBlockParams(funcOp);
		computeDirtyValues(funcOp);
		computeFusionUses(funcOp);
		deferredAddressParts_.clear();

		// Static usage counts feed the compare→branch fusion decision; only
		// pay for the walk when the fusion is enabled.
		usageCounts_ = enableBranchFusion_ ? ir::countUsages(&funcBlock)
		                                   : std::unordered_map<ir::OperationIdentifier, uint32_t> {};

		// Materialise the function's alloca table into one stack slot per
		// entry in the prologue.  visitAlloca() then just looks the slot up
		// by index.  Replaces the old hoisting phase: slot order is fixed
		// by trace's allocaSpecs, not by where the AllocaOperation lives.
		const auto& allocaSpecs = funcOp->getAllocaSpecs();
		functionAllocaSlots_.reserve(allocaSpecs.size());
		for (const auto& spec : allocaSpecs) {
			auto stackMem =
			    cc.newStack(static_cast<uint32_t>(spec.size), static_cast<uint32_t>(std::max<size_t>(spec.align, 1)));
			auto ptrReg = cc.newIntPtr();
			cc.loadAddressOf(ptrReg, stackMem);
			functionAllocaSlots_.emplace_back(AsmReg(ptrReg));
		}

		for (auto* block : funcOp->getBasicBlocks()) {
			getOrCreateLabel(block->getIdentifier());
		}

		RegisterFrame rootFrame;
		for (size_t i = 0; i < entryArgs.size(); i++) {
			const Type stamp = entryArgs[i]->getStamp();
			auto reg = allocReg(stamp);
			if (std::holds_alternative<Gp>(reg)) {
				funcNode->setArg(i, toGp(reg));
			} else {
				funcNode->setArg(i, toVec(reg));
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
	if (statistics_ != nullptr && enableTailDuplication_) {
		statistics_->add("asmjit.lowering.duplicatedTails", duplicatedTails_);
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

	cc.bind(getOrCreateLabel(id));

	// visitIf consumes a pending fused compare before recursing back here.
	assert(pendingFusedCompare_ == nullptr && "fused compare leaked across blocks");

	const auto& ops = block->getOperations();
	for (size_t i = 0; i < ops.size(); i++) {
		// Compare→branch fusion handshake: when the compare's flags can be
		// consumed directly by the IfOperation that follows it, skip the
		// compare's own lowering and let visitIf emit a fused cmp+b.cond.
		// Adjacency guarantees no instruction between cmp and b.cond can
		// clobber NZCV or retarget an operand register.
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
	// Unlike x86, float compares fuse too: every float condition visitCompare
	// uses is a single NZCV condition (unordered results set C and V), and a
	// negated condition is exactly its complement.
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
		// A constant parameter is never read from a register.
		if (constBlockParams_.contains(dstArgs[i])) {
			source.skip = true;
			sources.push_back(source);
			continue;
		}
		// Constant sources are recovered from the operation itself (see
		// gpOperand for why a frame binding must not shadow a constant).
		if (const auto value = constOperand(srcArgs[i])) {
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
		if (sources[i].skip) {
			dsts.emplace_back(AsmReg {});
			continue;
		}
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
	for (size_t i = 0; i < dsts.size(); i++) {
		if (!sources[i].skip) {
			dstMarks_[vregIndex(dsts[i])] = dstMarkEpoch_;
		}
	}

	// Phase 1: detour hazardous register sources through fresh temps.
	auto& temps = blockArgTemps_;
	temps.assign(srcArgs.size(), std::nullopt);
	for (size_t i = 0; i < srcArgs.size(); i++) {
		if (sources[i].skip || !sources[i].reg.has_value()) {
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
		if (sources[i].skip) {
			continue;
		}
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
	auto reg = allocReg(op->getStamp());
	auto vecReg = toVec(reg);
	if (op->getStamp() == Type::f32) {
		float val = static_cast<float>(op->getValue());
		uint32_t bits = 0;
		memcpy(&bits, &val, sizeof(bits));
		auto tempGp = cc.newUInt32();
		cc.mov(tempGp, bits);
		cc.fmov(vecReg, tempGp);
	} else {
		double val = op->getValue();
		uint64_t bits = 0;
		memcpy(&bits, &val, sizeof(bits));
		auto tempGp = cc.newUInt64();
		cc.mov(tempGp, bits);
		cc.fmov(vecReg, tempGp);
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
		cc.fadd(toVec(result), toVec(left), toVec(right));
		bindResult(op->getIdentifier(), result, frame);
		return;
	}
	auto gDst = toGp(result);
	// The single consumer recomputes a reassociated inner add from its inputs.
	if (reassociatedInners_.contains(op) && !frame.contains(op->getIdentifier())) {
		return;
	}
	if (emitReassociated(op, gDst, frame)) {
		bindResult(op->getIdentifier(), result, frame);
		return;
	}
	const auto parts = matchAddress(op);
	if (parts.has_value()) {
		// Every consumer folds it into its own memory operand: emit nothing.
		if (deferAddressPart(op, 0x1, frame)) {
			return;
		}
		// A scaled index whose scaling was skipped, or a widened 32-bit index
		// whose cast was skipped: one shifted- or extended-register add.
		if (parts->offset != nullptr) {
			const auto [indexOp, shift] = addressIndex(*parts);
			if (shift != 0 || isDeferredAddressPart(indexOp)) {
				auto base = gpOperand(parts->base, frame);
				auto index = indexOperand(indexOp, frame);
				if (index.extended && shift <= 4) {
					// AsmJit insists on equal source widths; the extend option
					// alone selects the W view, so the X name encodes the same.
					cc.add(gDst, base, index.reg.x(), index.isSigned ? a64::sxtw(shift) : a64::uxtw(shift));
				} else {
					cc.add(gDst, base, index.extended ? gpOperand(indexOp, frame) : index.reg, arm::lsl(shift));
				}
				bindResult(op->getIdentifier(), result, frame);
				return;
			}
		}
	}
	const uint32_t width = stampBits(op->getStamp());
	// Fold a constant operand into the add/sub immediate form (add is
	// commutative, so either side qualifies).
	if (const auto rightImm = constOperand(op->getRightInput())) {
		emitAddImmediate(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame), *rightImm);
	} else if (const auto leftImm = constOperand(op->getLeftInput())) {
		emitAddImmediate(gDst, gpOperandAtWidth(op->getRightInput(), width, frame), *leftImm);
	} else {
		cc.add(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame),
		       gpOperandAtWidth(op->getRightInput(), width, frame));
	}
	// An add that overflows the narrow stamp's width still produces a
	// "correct" 64-bit sum; re-extend per the result type so its
	// sign/zero-extension matches the wrapped-around narrow-width value
	// (see narrowToStamp's doc comment), unless narrowing is lazy.
	narrowResult(gDst, op->getStamp());
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitSub(ir::SubOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto left = frame.getValue(op->getLeftInput()->getIdentifier());
		auto right = frame.getValue(op->getRightInput()->getIdentifier());
		cc.fsub(toVec(result), toVec(left), toVec(right));
	} else if (reassociatedInners_.contains(op) && !frame.contains(op->getIdentifier())) {
		// The single consumer recomputes a reassociated inner sub from its inputs.
		return;
	} else if (!emitReassociated(op, toGp(result), frame)) {
		auto gDst = toGp(result);
		const uint32_t width = stampBits(op->getStamp());
		auto left = gpOperandAtWidth(op->getLeftInput(), width, frame);
		if (const auto rightImm = constOperand(op->getRightInput())) {
			emitAddImmediate(gDst, left, static_cast<int64_t>(uint64_t {0} - static_cast<uint64_t>(*rightImm)));
		} else {
			cc.sub(gDst, left, gpOperandAtWidth(op->getRightInput(), width, frame));
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
		cc.fmul(toVec(result), toVec(left), toVec(right));
	} else {
		auto gDst = toGp(result);
		const uint32_t width = stampBits(op->getStamp());
		const auto rightImm = constOperand(op->getRightInput());
		const auto leftImm = rightImm.has_value() ? std::nullopt : constOperand(op->getLeftInput());
		const auto imm = rightImm.has_value() ? rightImm : leftImm;
		// Strength-reduce the multiply (no immediate form on AArch64): a power
		// of two is a shift, and 2^k + 1 is one shifted-register add (the low
		// 64 bits agree in every case).
		const auto value = imm.has_value() ? static_cast<uint64_t>(*imm) : 0;
		if (imm.has_value() && value > 1 && value <= (uint64_t {1} << 62) &&
		    (std::has_single_bit(value) || std::has_single_bit(value - 1))) {
			auto src = gpOperandAtWidth(rightImm.has_value() ? op->getLeftInput() : op->getRightInput(), width, frame);
			if (std::has_single_bit(value)) {
				cc.lsl(gDst, src, Imm(std::countr_zero(value)));
			} else {
				cc.add(gDst, src, src, arm::lsl(static_cast<uint32_t>(std::countr_zero(value - 1))));
			}
			foldedImmediates_++;
		} else {
			cc.mul(gDst, gpOperandAtWidth(op->getLeftInput(), width, frame),
			       gpOperandAtWidth(op->getRightInput(), width, frame));
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
		cc.fdiv(toVec(result), toVec(left), toVec(right));
	} else if (isUnsignedType(op->getStamp())) {
		cc.udiv(toGp(result), cleanGpOperand(op->getLeftInput(), frame), cleanGpOperand(op->getRightInput(), frame));
	} else {
		cc.sdiv(toGp(result), cleanGpOperand(op->getLeftInput(), frame), cleanGpOperand(op->getRightInput(), frame));
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMod(ir::ModOperation* op, RegisterFrame& frame) {
	auto left = cleanGpOperand(op->getLeftInput(), frame);
	auto right = cleanGpOperand(op->getRightInput(), frame);
	auto result = allocReg(op->getStamp());
	// ARM64 has no remainder instruction. Compute: result = left - (left / right) * right
	auto quotient = cc.newInt64();
	if (isUnsignedType(op->getStamp())) {
		cc.udiv(quotient, left, right);
	} else {
		cc.sdiv(quotient, left, right);
	}
	// msub dst, quotient, right, left → dst = left - quotient * right
	cc.msub(toGp(result), quotient, right, left);
	bindResult(op->getIdentifier(), result, frame);
}

// ── Logical / compare ─────────────────────────────────────────────────────────

arm::CondCode AsmJitLoweringProvider::LoweringContext::emitCompareFlags(const ir::CompareOperation* cmp,
                                                                        RegisterFrame& frame) {
	// arm::CondCode lives in asmjit/core/archcommons.h.
	using CC = arm::CondCode;
	const auto* leftIn = cmp->getLeftInput();
	const auto* rightIn = cmp->getRightInput();

	if (isFloatType(leftIn->getStamp())) {
		cc.fcmp(toVec(frame.getValue(leftIn->getIdentifier())), toVec(frame.getValue(rightIn->getIdentifier())));
		// fcmp sets NZCV; for unordered (NaN), C=1, V=1.
		// These condition codes give correct IEEE 754 semantics.
		switch (cmp->getComparator()) {
		case ir::CompareOperation::EQ:
			return CC::kEQ;
		case ir::CompareOperation::NE:
			return CC::kNE;
		case ir::CompareOperation::LT:
			return CC::kMI;
		case ir::CompareOperation::LE:
			return CC::kLS;
		case ir::CompareOperation::GT:
			return CC::kGT;
		case ir::CompareOperation::GE:
			return CC::kGE;
		}
	}

	if (leftIn->getStamp() == Type::ptr && isInteger(rightIn->getStamp())) {
		// Null-pointer check: compare pointer against zero.
		cc.cmp(gpOperand(leftIn, frame), Imm(0));
		return cmp->getComparator() == ir::CompareOperation::EQ ? CC::kEQ : CC::kNE;
	}

	// When both sides are 32 bits wide the compare runs on the W registers,
	// which are exact even for dirty operands (see the lazy narrowing
	// helpers); other widths compare the canonical 64-bit patterns.
	const bool compare32 = stampBits(leftIn->getStamp()) == 32 && stampBits(rightIn->getStamp()) == 32;
	const auto operand = [&](const ir::Operation* in) {
		return compare32 ? gpOperand(in, frame).w() : cleanGpOperand(in, frame);
	};
	auto left = operand(leftIn);
	const auto rightImm = constOperand(rightIn);
	// The immediate's bits at the compare's width; `cmp x, #k` and
	// `cmn x, #-k` set identical flags for every k != 0.
	const uint64_t widthMask = compare32 ? 0xFFFFFFFFu : ~uint64_t {0};
	const uint64_t k = rightImm.has_value() ? static_cast<uint64_t>(*rightImm) & widthMask : 0;
	const uint64_t negK = (uint64_t {0} - k) & widthMask;
	if (rightImm.has_value() && isAddSubImm(k)) {
		withAddSubImm(k, [&](const Imm& imm, const std::optional<Imm>& shift) {
			shift ? cc.cmp(left, imm, *shift) : cc.cmp(left, imm);
		});
		foldedImmediates_++;
	} else if (rightImm.has_value() && k != 0 && isAddSubImm(negK)) {
		withAddSubImm(negK, [&](const Imm& imm, const std::optional<Imm>& shift) {
			shift ? cc.cmn(left, imm, *shift) : cc.cmn(left, imm);
		});
		foldedImmediates_++;
	} else {
		cc.cmp(left, operand(rightIn));
	}

	const bool isUnsigned = isUnsignedType(leftIn->getStamp());
	switch (cmp->getComparator()) {
	case ir::CompareOperation::EQ:
		return CC::kEQ;
	case ir::CompareOperation::NE:
		return CC::kNE;
	case ir::CompareOperation::LT:
		return isUnsigned ? CC::kLO : CC::kLT;
	case ir::CompareOperation::LE:
		return isUnsigned ? CC::kLS : CC::kLE;
	case ir::CompareOperation::GT:
		return isUnsigned ? CC::kHI : CC::kGT;
	case ir::CompareOperation::GE:
		return isUnsigned ? CC::kHS : CC::kGE;
	}
	return CC::kAL;
}

void AsmJitLoweringProvider::LoweringContext::visitCompare(ir::CompareOperation* op, RegisterFrame& frame) {
	auto result = allocReg(Type::b);
	const auto condCode = emitCompareFlags(op, frame);
	cc.cset(toGp(result), Imm(static_cast<uint32_t>(condCode)));
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	auto left = gpOperand(op->getLeftInput(), frame);
	if (const auto rightImm = constOperand(op->getRightInput()); rightImm.has_value() && isLogicalImm64(*rightImm)) {
		cc.and_(toGp(result), left, Imm(static_cast<uint64_t>(*rightImm)));
		foldedImmediates_++;
	} else {
		cc.and_(toGp(result), left, gpOperand(op->getRightInput(), frame));
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	auto left = gpOperand(op->getLeftInput(), frame);
	if (const auto rightImm = constOperand(op->getRightInput()); rightImm.has_value() && isLogicalImm64(*rightImm)) {
		cc.orr(toGp(result), left, Imm(static_cast<uint64_t>(*rightImm)));
		foldedImmediates_++;
	} else {
		cc.orr(toGp(result), left, gpOperand(op->getRightInput(), frame));
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNot(ir::NotOperation* op, RegisterFrame& frame) {
	// NotOperation is logical NOT on a boolean: result = input XOR 1.
	auto input = gpOperand(op->getInput(), frame);
	auto result = allocReg(op->getStamp());
	cc.eor(toGp(result), input, Imm(1));
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		// ARM64 has a native FNEG vector instruction: a real IEEE-754
		// sign-flip, correct for every input including zero.
		cc.fneg(toVec(result), toVec(frame.getValue(op->getInput()->getIdentifier())));
		bindResult(op->getIdentifier(), result, frame);
		return;
	}
	cc.mvn(toGp(result), gpOperandAtWidth(op->getInput(), stampBits(op->getStamp()), frame));
	// mvn flips the full 64-bit register, including the extension padding.
	// That happens to stay correct for signed stamps (flipping a sign bit
	// flips its replicated extension consistently) but is wrong for unsigned
	// stamps, whose invariant is a zero-extended (not flipped) upper half.
	narrowResult(toGp(result), op->getStamp());
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
	auto left = isLeftShift ? gpOperandAtWidth(op->getLeftInput(), stampBits(op->getStamp()), frame)
	                        : cleanGpOperand(op->getLeftInput(), frame);
	// A constant count uses the immediate shift form. The register forms
	// (lslv/lsrv/asrv) take the count mod 64, so masking here preserves the
	// register-form semantics.
	if (const auto countImm = constOperand(op->getRightInput())) {
		const auto count = Imm(static_cast<uint32_t>(*countImm) & 63u);
		if (isLeftShift) {
			cc.lsl(gDst, left, count);
		} else if (isUnsignedType(op->getStamp())) {
			cc.lsr(gDst, left, count);
		} else {
			cc.asr(gDst, left, count);
		}
		foldedImmediates_++;
	} else {
		auto right = gpOperand(op->getRightInput(), frame);
		if (isLeftShift) {
			cc.lsl(gDst, left, right);
		} else if (isUnsignedType(op->getStamp())) {
			cc.lsr(gDst, left, right);
		} else {
			cc.asr(gDst, left, right);
		}
	}
	// Shifting the full 64-bit register (rather than just the narrow stamp's
	// width) can leave the extension padding inconsistent with the
	// narrow-width result -- e.g. a left shift that overflows the stamp's
	// width still computes a "correct" 64-bit shift, whose sign-extension no
	// longer matches the wrapped-around narrow-width value. asr already
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
	auto gDst = toGp(result);
	const uint32_t width = stampBits(op->getStamp());
	auto left = gpOperandAtWidth(op->getLeftInput(), width, frame);
	if (const auto rightImm = constOperand(op->getRightInput()); rightImm.has_value() && isLogicalImm64(*rightImm)) {
		const auto imm = Imm(static_cast<uint64_t>(*rightImm));
		switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			cc.and_(gDst, left, imm);
			break;
		case ir::BinaryCompOperation::BOR:
			cc.orr(gDst, left, imm);
			break;
		case ir::BinaryCompOperation::XOR:
			cc.eor(gDst, left, imm);
			break;
		}
		foldedImmediates_++;
	} else {
		auto right = gpOperandAtWidth(op->getRightInput(), width, frame);
		switch (op->getType()) {
		case ir::BinaryCompOperation::BAND:
			cc.and_(gDst, left, right);
			break;
		case ir::BinaryCompOperation::BOR:
			cc.orr(gDst, left, right);
			break;
		case ir::BinaryCompOperation::XOR:
			cc.eor(gDst, left, right);
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
		// branch in one fused step. The block-argument copies below sit after
		// the branch, so they cannot disturb the flags.
		assert(op->getValue() == pendingFusedCompare_ && "pending fused compare does not feed this if");
		const auto* cmp = pendingFusedCompare_;
		pendingFusedCompare_ = nullptr;
		emitFusedCompareBranch(cmp, elsePath, frame);
		fusedBranches_++;
	} else {
		cc.cbz(gpOperand(op->getValue(), frame), elsePath);
	}

	// True branch: copy arguments then jump.
	processBlockInvocation(op->getTrueBlockInvocation(), frame);
	cc.b(trueLabel);

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
	cc.b(falseLabel);

	processBlock(op->getFalseBlockInvocation().getBlock(), frame);
}

std::optional<Gp> AsmJitLoweringProvider::LoweringContext::zeroTestOperand(const ir::CompareOperation* cmp,
                                                                           RegisterFrame& frame) {
	const auto comparator = cmp->getComparator();
	const auto* leftIn = cmp->getLeftInput();
	const auto* rightIn = cmp->getRightInput();
	if ((comparator != ir::CompareOperation::EQ && comparator != ir::CompareOperation::NE) ||
	    isFloatType(leftIn->getStamp())) {
		return std::nullopt;
	}
	if (leftIn->getStamp() == Type::ptr && isInteger(rightIn->getStamp())) {
		return gpOperand(leftIn, frame); // null-pointer check
	}
	const auto rightImm = constOperand(rightIn);
	if (!rightImm.has_value() || *rightImm != 0) {
		return std::nullopt;
	}
	if (stampBits(leftIn->getStamp()) == 32 && stampBits(rightIn->getStamp()) == 32) {
		return gpOperand(leftIn, frame).w();
	}
	return cleanGpOperand(leftIn, frame);
}

// Fused replacement for visitCompare + the cbz in visitIf: emits the compare
// and branches to @p target when the condition is false (or true, with
// @p jumpIfTrue). An EQ/NE test against zero needs no flags at all.
void AsmJitLoweringProvider::LoweringContext::emitFusedCompareBranch(const ir::CompareOperation* cmp, Label target,
                                                                     RegisterFrame& frame, bool jumpIfTrue) {
	if (const auto reg = zeroTestOperand(cmp, frame)) {
		const bool branchIfZero = (cmp->getComparator() == ir::CompareOperation::EQ) == jumpIfTrue;
		if (branchIfZero) {
			cc.cbz(*reg, target);
		} else {
			cc.cbnz(*reg, target);
		}
		return;
	}
	const auto cond = emitCompareFlags(cmp, frame);
	cc.b(jumpIfTrue ? cond : arm::negateCond(cond), target);
}

void AsmJitLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* op, RegisterFrame& frame) {
	const auto& bi = op->getNextBlockInvocation();
	processBlockInvocation(bi, frame);
	if (tryDuplicateTail(bi, frame)) {
		return;
	}
	if (!tryRotateLoopBranch(bi, frame)) {
		cc.b(getOrCreateLabel(bi.getBlock()->getIdentifier()));
	}
	processBlock(bi.getBlock(), frame);
}

// A diamond's arms both branch to the merge block, and only one of them can
// fall into it; the other pays a taken branch, on top of the one that entered
// it. When the merge block is small and ends in an unconditional branch (which
// loop rotation usually turns into the loop's latch test), that arm lowers a
// copy of it instead. The copy runs in the same frame: every value the block
// defines that its first emission materialised is still bound, so the copy
// writes the same registers (bindResult moves into them), and its successors
// see exactly the registers they expect. Deferred values stay deferred, since
// they were never bound. Nautilus IR passes every cross-block value as a block
// argument, so nothing outside the block reads its values except through its
// terminator.
bool AsmJitLoweringProvider::LoweringContext::tryDuplicateTail(const ir::BasicBlockInvocation& bi,
                                                               RegisterFrame& frame) {
	const auto* block = bi.getBlock();
	if (!enableTailDuplication_ || duplicatingTail_ || !processedBlocks.contains(block->getIdentifier())) {
		return false;
	}
	constexpr size_t maxTailOperations = 6;
	const auto& ops = block->getOperations();
	if (ops.empty() || ops.size() > maxTailOperations + 1 || ir::dyn_cast<ir::BranchOperation>(ops.back()) == nullptr) {
		return false;
	}
	for (size_t i = 0; i + 1 < ops.size(); i++) {
		const auto* op = ops[i];
		const bool plainValueOp =
		    ir::dyn_cast<ir::ConstIntOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::ConstBooleanOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::ConstPtrOperation>(op) != nullptr || ir::dyn_cast<ir::AddOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::SubOperation>(op) != nullptr || ir::dyn_cast<ir::MulOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::CompareOperation>(op) != nullptr || ir::dyn_cast<ir::AndOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::OrOperation>(op) != nullptr || ir::dyn_cast<ir::NotOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::NegateOperation>(op) != nullptr || ir::dyn_cast<ir::ShiftOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::BinaryCompOperation>(op) != nullptr || ir::dyn_cast<ir::SelectOperation>(op) != nullptr ||
		    ir::dyn_cast<ir::LoadOperation>(op) != nullptr || ir::dyn_cast<ir::CastOperation>(op) != nullptr;
		if (!plainValueOp) {
			return false;
		}
	}
	duplicatingTail_ = true;
	for (auto* op : ops) {
		dispatch(op, frame);
	}
	duplicatingTail_ = false;
	duplicatedTails_++;
	return true;
}

bool AsmJitLoweringProvider::LoweringContext::isNoOpInvocation(const ir::BasicBlockInvocation& bi,
                                                               RegisterFrame& frame) {
	const auto& srcArgs = bi.getArguments();
	const auto& dstArgs = bi.getBlock()->getArguments();
	for (size_t i = 0; i < srcArgs.size(); i++) {
		if (constBlockParams_.contains(dstArgs[i])) {
			continue;
		}
		const auto& dstId = dstArgs[i]->getIdentifier();
		if (constOperand(srcArgs[i]).has_value() || !frame.contains(dstId) ||
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
// condition and branches into the body: two branches per iteration, one of
// them taken. When the header holds nothing but that test (a fused compare
// feeding its if) and the if's own block-argument copies are register
// self-moves, the test can be repeated at the end of the back edge instead,
// branching straight into the body (taken) or out of the loop. The header's
// own copy still handles the first entry. The compare reads the header's
// parameter registers, which the back edge's copies have just written.
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
	cc.b(getOrCreateLabel(ifOp->getFalseBlockInvocation().getBlock()->getIdentifier()));
	rotatedLoops_++;
	return true;
}

void AsmJitLoweringProvider::LoweringContext::visitReturn(ir::ReturnOperation* op, RegisterFrame& frame) {
	if (op->hasReturnValue()) {
		auto retReg = regOperand(op->getReturnValue(), frame);
		if (std::holds_alternative<Vec>(retReg)) {
			cc.ret(toVec(retReg));
		} else {
			auto gp = toGp(retReg);
			// Ensure narrow return values are properly extended so the caller
			// sees a clean W0/X0 value that matches the ABI contract.
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

	if (enableSelectCmov_) {
		// Branch-free data mux: both values are already unconditionally
		// computed (select is not lazy), so a csel replaces the two-way
		// branch and its misprediction risk.
		cc.cmp(condGp, Imm(0));
		const auto ne = Imm(static_cast<uint32_t>(arm::CondCode::kNE));
		if (std::holds_alternative<Vec>(result)) {
			cc.fcsel(toVec(result), toVec(frame.getValue(op->getTrueValue()->getIdentifier())),
			         toVec(frame.getValue(op->getFalseValue()->getIdentifier())), ne);
		} else {
			// A constant zero arm reads the zero register.
			const uint32_t width = stampBits(op->getStamp());
			const auto operand = [&](const ir::Operation* in) {
				const auto value = constOperand(in);
				return value.has_value() && *value == 0 ? Gp(xzr) : gpOperandAtWidth(in, width, frame);
			};
			cc.csel(toGp(result), operand(op->getTrueValue()), operand(op->getFalseValue()), ne);
		}
	} else {
		auto falsePath = cc.newLabel();
		auto donePath = cc.newLabel();
		cc.cbz(condGp, falsePath);
		emitMoveFromOperand(result, op->getTrueValue(), frame);
		cc.b(donePath);
		cc.bind(falsePath);
		emitMoveFromOperand(result, op->getFalseValue(), frame);
		cc.bind(donePath);
	}

	bindResult(op->getIdentifier(), result, frame);
}

// ── Memory ────────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitLoad(ir::LoadOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	auto mem = memOperand(op->getAddress(), frame);

	if (op->getStamp() == Type::f32 || op->getStamp() == Type::f64) {
		cc.ldr(toVec(result), mem);
	} else {
		auto gDst = toGp(result);
		switch (op->getStamp()) {
		case Type::b:
		case Type::ui8:
			cc.ldrb(gDst.w(), mem);
			break;
		case Type::i8:
			cc.ldrsb(gDst.x(), mem);
			break;
		case Type::ui16:
			cc.ldrh(gDst.w(), mem);
			break;
		case Type::i16:
			cc.ldrsh(gDst.x(), mem);
			break;
		case Type::ui32:
			cc.ldr(gDst.w(), mem);
			break;
		case Type::i32:
			cc.ldrsw(gDst.x(), mem);
			break;
		default:
			cc.ldr(gDst.x(), mem);
			break;
		}
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitStore(ir::StoreOperation* op, RegisterFrame& frame) {
	const Type stamp = op->getValue()->getStamp();
	if (stamp == Type::f32 || stamp == Type::f64) {
		auto valReg = frame.getValue(op->getValue()->getIdentifier());
		cc.str(toVec(valReg), memOperand(op->getAddress(), frame));
		return;
	}

	// A constant zero is stored straight from the zero register.
	const auto value = constOperand(op->getValue());
	auto valGp = value.has_value() && *value == 0 ? Gp(xzr) : gpOperand(op->getValue(), frame);
	auto mem = memOperand(op->getAddress(), frame);
	switch (stamp) {
	case Type::b:
	case Type::i8:
	case Type::ui8:
		cc.strb(valGp.w(), mem);
		break;
	case Type::i16:
	case Type::ui16:
		cc.strh(valGp.w(), mem);
		break;
	case Type::i32:
	case Type::ui32:
		cc.str(valGp.w(), mem);
		break;
	default:
		cc.str(valGp.x(), mem);
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
	// replace the scalar function-call lowering with native NEON instructions
	// (e.g. emit `add v0.4s, v1.4s, v2.4s` for vector_add_i32x4_impl). The
	// handler is expected to bind the result identifier into the frame itself.
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
			} else if (isDirty(arg) && !constOperand(arg).has_value()) {
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

	// Internal Nautilus functions (resolved via a JIT label, whose function
	// pointer is not a callable address) are never capture sites; only external
	// calls need the capture thunk (mirrors X64's visitCall).
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
	// call in the instruction stream. External callees get canonical narrow
	// arguments (AAPCS64 on Darwin has the caller extend sub-32-bit
	// arguments); internal callees re-extend in their prologue.
	std::vector<AsmReg> argRegs;
	argRegs.reserve(op->getInputArguments().size());
	for (auto* arg : op->getInputArguments()) {
		argRegs.push_back(isInternal || isFloatType(arg->getStamp()) ? regOperand(arg, frame)
		                                                             : AsmReg(cleanGpOperand(arg, frame)));
	}

	// ARM64 blr requires a register operand — labels and immediates are not
	// valid operands.  Materialize the target address into a GP register.
	InvokeNode* invokeNode = nullptr;
	if (it != funcNodes_.end()) {
		// Internal JIT function: load label address via adr, then indirect call.
		auto tmp = cc.newGpx();
		cc.adr(tmp, it->second->label());
		cc.invoke(&invokeNode, tmp, sig);
	} else if (needsCapture) {
		// Captured-exception call site: invoke the captureThrowingCall<R,
		// Args...> thunk — a real C++ frame — instead of the raw target, so
		// an exception is caught before it crosses the unwind-table-less JIT
		// frame. The raw target becomes the thunk's first argument.
		void* thunk = resolveCaptureThunk(op);
		// Emit the raw-target `mov` BEFORE invoking the thunk: AsmJit cannot
		// hoist a def that appears after the invoke into its argument register.
		auto targetReg = cc.newIntPtr();
		cc.mov(targetReg, reinterpret_cast<uint64_t>(op->getFunctionPtr()));
		auto tmp = cc.newIntPtr();
		cc.mov(tmp, reinterpret_cast<uint64_t>(thunk));
		cc.invoke(&invokeNode, tmp, sig);
		// Argument 0: the raw target pointer (compiled as a constant).
		invokeNode->setArg(0, targetReg);
	} else {
		// External function: load raw pointer into register, then indirect call.
		auto tmp = cc.newIntPtr();
		cc.mov(tmp, reinterpret_cast<uint64_t>(op->getFunctionPtr()));
		cc.invoke(&invokeNode, tmp, sig);
	}

	for (size_t i = 0; i < argRegs.size(); i++) {
		if (std::holds_alternative<Vec>(argRegs[i]))
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toVec(argRegs[i]));
		else
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toGp(argRegs[i]));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Vec>(result)) {
			invokeNode->setRet(0, toVec(result));
		} else {
			invokeNode->setRet(0, toGp(result));
			// The ABI leaves the upper bits of a narrow integer return value
			// unspecified (the callee only guarantees the stamp's own width),
			// so re-establish the extension invariant before anything reads
			// the full X register.
			narrowToStamp(toGp(result), op->getStamp());
		}
		bindResult(op->getIdentifier(), result, frame);
	}

	if (needsCheck) {
		emitCheckPendingException(op);
	}
}

void AsmJitLoweringProvider::LoweringContext::visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame) {
	const bool needsCapture = transport_.callNeedsCaptureThunk(op);

	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	if (needsCapture) {
		sig.addArg(TypeId::kUIntPtr); // raw target as the thunk's first argument
	}
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

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
		auto tmp = cc.newIntPtr();
		cc.mov(tmp, reinterpret_cast<uint64_t>(thunk));
		cc.invoke(&invokeNode, tmp, sig);
		// Argument 0: the raw target (the runtime function-pointer register).
		invokeNode->setArg(0, fnPtrGp.x());
	} else {
		cc.invoke(&invokeNode, fnPtrGp.x(), sig);
	}

	for (size_t i = 0; i < argRegs.size(); i++) {
		if (std::holds_alternative<Vec>(argRegs[i]))
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toVec(argRegs[i]));
		else
			invokeNode->setArg(i + (needsCapture ? 1 : 0), toGp(argRegs[i]));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Vec>(result)) {
			invokeNode->setRet(0, toVec(result));
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
		cc.adr(toGp(reg), it->second->label());
	} else {
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
	// A widening index every consumer reads as an extended W register.
	if (asWidenedIndex(op) != nullptr && deferAddressPart(op, 0x4, frame)) {
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
		// For floats, NE also holds for NaN (fcmp clears Z when unordered).
		auto gDst = toGp(result);
		if (srcIsFloat) {
			cc.fcmp(toVec(src), Imm(0));
		} else {
			auto gSrc = toGp(src);
			switch (srcType) {
			case Type::i8:
			case Type::ui8:
				cc.tst(gSrc.w(), Imm(0xFF));
				break;
			case Type::i16:
			case Type::ui16:
				cc.tst(gSrc.w(), Imm(0xFFFF));
				break;
			case Type::i32:
			case Type::ui32:
				cc.cmp(gSrc.w(), Imm(0));
				break;
			default:
				cc.cmp(gSrc.x(), Imm(0));
				break;
			}
		}
		cc.cset(gDst.w(), Imm(static_cast<uint32_t>(arm::CondCode::kNE)));
		bindResult(op->getIdentifier(), result, frame);
		return;
	}

	if (!srcIsFloat && !dstIsFloat) {
		// Integer → integer: first extend from source width, then narrow to
		// destination width. The second step matters whenever dstWidth <=
		// srcWidth (truncating, or a same-width signedness change like
		// i16->ui16): the value is then defined by its low dstWidth bits, and
		// those must be re-extended per the *destination*'s signedness.
		auto gDst = toGp(result);
		// Step 1: extend from source width to get a clean 64-bit representation.
		extendFromStamp(gDst, toGp(src), srcType);
		// Step 2: narrow to destination width. ARM64 has no sub-32-bit register
		// aliases, so the upper bits of W/X must be explicitly cleared to ensure
		// the caller sees the correct value (especially for return values).
		narrowToStamp(gDst, dstType);
	} else if (srcIsFloat && !dstIsFloat) {
		// Float → integer: truncate toward zero.
		auto gDst = toGp(result);
		auto vSrc = toVec(src);
		if (isUnsignedType(dstType))
			cc.fcvtzu(gDst, vSrc);
		else
			cc.fcvtzs(gDst, vSrc);
		// Narrow to sub-32-bit destination if needed.
		switch (dstType) {
		case Type::i8:
		case Type::b:
		case Type::ui8:
		case Type::i16:
		case Type::ui16:
			narrowToStamp(gDst, dstType);
			break;
		default:
			break;
		}
	} else if (!srcIsFloat && dstIsFloat) {
		// Integer → float.
		auto gSrc = toGp(src);
		auto vDst = toVec(result);
		if (isUnsignedType(srcType))
			cc.ucvtf(vDst, gSrc);
		else
			cc.scvtf(vDst, gSrc);
	} else {
		// Float → float: f32↔f64.
		cc.fcvt(toVec(result), toVec(src));
	}
	bindResult(op->getIdentifier(), result, frame);
}

// ── Captured-exception transport ─────────────────────────────────────────────
// Potentially-throwing calls are routed through captureThrowingCall<R, Args...>
// (a real C++ frame) and followed by a pending-exception check that branches to
// the call's landing pad. Pads and the exceptional exit are emitted after the
// main CFG, mirroring the X64/BC/TBC/CPP captured backends.

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
	if (const auto* proxy = ir::dyn_cast<ir::CallOperation>(call)) {
		return proxy->getCaptureFunc();
	}
	if (const auto* indirect = ir::dyn_cast<ir::IndirectCallOperation>(call)) {
		return indirect->getCaptureFunc();
	}
	throw NotImplementedException("asmjit/a64: captured-exception call has no recorded capture wrapper");
}

void AsmJitLoweringProvider::LoweringContext::emitCheckPendingException(const ir::Operation* call) {
	const auto padIndex = transport_.getPadIndexForCall(call);
	const Label target = padIndex != ir::noLandingPad ? getPadLabel(padIndex) : getExceptionalExitLabel();

	// frame = currentExceptionFrame()
	FuncSignature frameSig;
	frameSig.setRet(TypeId::kUIntPtr);
	InvokeNode* frameInvoke = nullptr;
	auto frameFn = cc.newIntPtr();
	cc.mov(frameFn, reinterpret_cast<uint64_t>(&currentExceptionFrame));
	cc.invoke(&frameInvoke, frameFn, frameSig);
	auto frameReg = cc.newIntPtr();
	frameInvoke->setRet(0, frameReg);

	// Load the pending field and branch to the pad when it is non-null. The
	// TLS frame is always non-null inside a JIT-invoked function (the
	// Invocable wrapper pushes it), so no null check is needed here.
	auto pendingReg = cc.newIntPtr();
	cc.ldr(pendingReg, a64::ptr(frameReg, static_cast<int32_t>(offsetof(ExceptionFrame, pending))));
	cc.cbnz(pendingReg, target);
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
		cc.b(getExceptionalExitLabel());
	}

	// Exceptional exit: return the ABI default for the function's return type.
	cc.bind(getExceptionalExitLabel());
	emitDefaultReturn(currentFunction_->getOutputArg());
}

void AsmJitLoweringProvider::LoweringContext::emitDefaultReturn(Type retType) {
	if (retType == Type::v) {
		cc.ret();
	} else if (retType == Type::f32 || retType == Type::f64) {
		// Zero the float return register. A64 only encodes `fmov Sd, Wn` /
		// `fmov Dd, Xn`, so the GP source width must match the FP width.
		auto vec = retType == Type::f32 ? cc.newVecS() : cc.newVecD();
		auto tmp = retType == Type::f32 ? cc.newUInt32() : cc.newUInt64();
		cc.mov(tmp, 0);
		cc.fmov(vec, tmp);
		cc.ret(vec);
	} else {
		// Integers, bools, and pointers: return 0 (false / nullptr).
		auto reg = cc.newInt64();
		cc.mov(reg, 0);
		cc.ret(reg);
	}
}

} // namespace nautilus::compiler::asmjit
