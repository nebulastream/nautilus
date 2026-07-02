
#include "nautilus/compiler/backends/amsjit/X64LoweringProvider.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/compiler/ir/Usages.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <cassert>
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
	// When disabled, the manager stays empty and every ProxyCall falls through
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
	default:
		return TypeId::kInt64;
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
		funcNodes_[funcOp->getName()] = cc.newFunc(sig);
	}

	// Pass 2: emit each function body.
	for (const auto& funcOp : functionOperations) {
		const auto& funcBlock = funcOp->getFunctionBasicBlock();
		const auto& entryArgs = funcBlock.getArguments();
		auto* funcNode = funcNodes_.at(funcOp->getName());

		cc.addFunc(funcNode);

		// Reset per-function block tracking so each function starts clean.
		blockLabels.clear();
		processedBlocks.clear();
		functionAllocaSlots_.clear();

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
			rootFrame.setValue(entryArgs[i]->getIdentifier(), reg);

			if (std::holds_alternative<Gp>(reg)) {
				auto gReg = toGp(reg);
				if (stamp == Type::i8) {
					cc.movsx(gReg.r64(), gReg.r8());
				} else if (stamp == Type::i16) {
					cc.movsx(gReg.r64(), gReg.r16());
				} else if (stamp == Type::ui8 || stamp == Type::b) {
					cc.movzx(gReg.r32(), gReg.r8());
				} else if (stamp == Type::ui16) {
					cc.movzx(gReg.r32(), gReg.r16());
				} else if (stamp == Type::i32) {
					cc.movsxd(gReg.r64(), gReg.r32());
				} else if (stamp == Type::ui32) {
					cc.mov(gReg.r32(), gReg.r32()); // zero-extends upper 32 bits
				}
			}
		}

		processBlock(&funcBlock, rootFrame);
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
	struct SourceValue {
		std::optional<AsmReg> reg; // bound register, or
		int64_t imm = 0;           // deferred-constant pattern
	};
	std::vector<SourceValue> sources;
	sources.reserve(srcArgs.size());
	for (size_t i = 0; i < srcArgs.size(); i++) {
		SourceValue source;
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
	for (size_t i = 0; i < dstArgs.size(); i++) {
		const auto& dstId = dstArgs[i]->getIdentifier();
		if (!frame.contains(dstId)) {
			frame.setValue(dstId, allocReg(dstArgs[i]->getStamp()));
		}
	}

	// Parallel-copy semantics: a destination write must not clobber a source
	// that a later copy still reads. Only a register source that is itself
	// one of the destination registers needs to detour through a temp; a
	// self-move (src == dst) needs no code at all, and everything else can
	// be written directly (immediates and temps can never alias a source).
	const auto vregId = [](const AsmReg& r) {
		return std::visit([](const auto& reg) { return reg.id(); }, r);
	};
	std::unordered_set<uint32_t> dstIds;
	for (size_t i = 0; i < dstArgs.size(); i++) {
		dstIds.insert(vregId(frame.getValue(dstArgs[i]->getIdentifier())));
	}

	// Phase 1: detour hazardous register sources through fresh temps.
	std::vector<std::optional<AsmReg>> temps(srcArgs.size());
	for (size_t i = 0; i < srcArgs.size(); i++) {
		if (!sources[i].reg.has_value()) {
			continue; // immediate -- written directly in phase 2
		}
		const auto srcId = vregId(*sources[i].reg);
		if (srcId == vregId(frame.getValue(dstArgs[i]->getIdentifier()))) {
			continue; // self-move -- no code needed
		}
		if (dstIds.count(srcId) != 0) {
			auto temp = allocReg(dstArgs[i]->getStamp());
			emitMove(temp, *sources[i].reg);
			temps[i] = temp;
		}
	}

	// Phase 2: write the destinations.
	for (size_t i = 0; i < srcArgs.size(); i++) {
		auto dst = frame.getValue(dstArgs[i]->getIdentifier());
		if (temps[i].has_value()) {
			emitMove(dst, *temps[i]);
		} else if (sources[i].reg.has_value()) {
			if (vregId(*sources[i].reg) != vregId(dst)) {
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
	} else {
		auto gDst = toGp(result);
		// Fold a small-constant operand into the add's immediate form
		// (add is commutative, so either side qualifies).
		const auto rightImm = imm32Operand(op->getRightInput());
		std::optional<int32_t> leftImm;
		if (!rightImm.has_value()) {
			leftImm = imm32Operand(op->getLeftInput());
		}
		if (rightImm.has_value()) {
			cc.mov(gDst, gpOperand(op->getLeftInput(), frame));
			cc.add(gDst, *rightImm);
			foldedImmediates_++;
		} else if (leftImm.has_value()) {
			cc.mov(gDst, gpOperand(op->getRightInput(), frame));
			cc.add(gDst, *leftImm);
			foldedImmediates_++;
		} else {
			cc.mov(gDst, gpOperand(op->getLeftInput(), frame));
			cc.add(gDst, gpOperand(op->getRightInput(), frame));
		}
		// An add that overflows the narrow stamp's width still produces a
		// "correct" 64-bit sum; re-extend per the result type so its
		// sign/zero-extension matches the wrapped-around narrow-width value
		// (see narrowToStamp's doc comment).
		narrowToStamp(gDst, op->getStamp());
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
		cc.mov(gDst, gpOperand(op->getLeftInput(), frame));
		if (const auto rightImm = imm32Operand(op->getRightInput())) {
			cc.sub(gDst, *rightImm);
			foldedImmediates_++;
		} else {
			cc.sub(gDst, gpOperand(op->getRightInput(), frame));
		}
		narrowToStamp(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMul(ir::MulOperation* op, RegisterFrame& frame) {
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
		if (rightImm.has_value()) {
			cc.imul(gDst, gpOperand(op->getLeftInput(), frame), *rightImm);
			foldedImmediates_++;
		} else if (leftImm.has_value()) {
			cc.imul(gDst, gpOperand(op->getRightInput(), frame), *leftImm);
			foldedImmediates_++;
		} else {
			cc.mov(gDst, gpOperand(op->getLeftInput(), frame));
			cc.imul(gDst, gpOperand(op->getRightInput(), frame));
		}
		narrowToStamp(gDst, op->getStamp());
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
		cc.mov(quot, gpOperand(op->getLeftInput(), frame));
		if (isUnsignedType(op->getStamp())) {
			cc.xor_(rem, rem);
			cc.div(rem, quot, gpOperand(op->getRightInput(), frame));
		} else {
			cc.cqo(rem, quot);
			cc.idiv(rem, quot, gpOperand(op->getRightInput(), frame));
		}
		cc.mov(toGp(result), quot);
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMod(ir::ModOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	auto quot = cc.newInt64();
	auto rem = cc.newInt64();
	cc.mov(quot, gpOperand(op->getLeftInput(), frame));
	if (isUnsignedType(op->getStamp())) {
		cc.xor_(rem, rem);
		cc.div(rem, quot, gpOperand(op->getRightInput(), frame));
	} else {
		cc.cqo(rem, quot);
		cc.idiv(rem, quot, gpOperand(op->getRightInput(), frame));
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
		auto left = gpOperand(op->getLeftInput(), frame);
		// Peephole: `cmp x, 0` → `test x, x` when the right operand is the
		// integer constant zero. Same flag output for ZF/SF/CF/OF and all
		// consumers here read only via setcc/jcc — two bytes shorter and
		// breaks no dependency.
		const auto* rightConst = ir::dyn_cast<ir::ConstIntOperation>(op->getRightInput());
		if (rightConst != nullptr && rightConst->getValue() == 0) {
			cc.test(left, left);
		} else if (const auto rightImm = imm32Operand(op->getRightInput())) {
			cc.cmp(left, *rightImm);
			foldedImmediates_++;
		} else {
			cc.cmp(left, gpOperand(op->getRightInput(), frame));
		}
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
	// NegateOperation is bitwise NOT (~x): result = input XOR all-ones.
	auto result = allocReg(op->getStamp());
	auto gDst = toGp(result);
	cc.mov(gDst, gpOperand(op->getInput(), frame));
	cc.not_(gDst);
	// not_ flips the full 64-bit register, including the extension padding.
	// That happens to stay correct for signed stamps (flipping a sign bit
	// flips its replicated extension consistently) but is wrong for unsigned
	// stamps, whose invariant is a zero-extended (not flipped) upper half.
	narrowToStamp(gDst, op->getStamp());
	bindResult(op->getIdentifier(), result, frame);
}

// ── Binary bit operations ─────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitShift(ir::ShiftOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	auto gDst = toGp(result);
	cc.mov(gDst, gpOperand(op->getLeftInput(), frame));
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
	// needed for all three shift forms.
	narrowToStamp(gDst, op->getStamp());
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitBinaryComp(ir::BinaryCompOperation* op, RegisterFrame& frame) {
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), gpOperand(op->getLeftInput(), frame));
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
		auto right = gpOperand(op->getRightInput(), frame);
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

// Fused replacement for visitCompare + the test/jz in visitIf: emits the
// compare and jumps to @p falseTarget when the condition is false. Mirrors
// visitCompare's integer paths (the float path is excluded by
// isFusibleCompare); the condition codes are the negation of the setcc the
// unfused lowering would have used.
void AsmJitLoweringProvider::LoweringContext::emitFusedCompareBranch(const ir::CompareOperation* cmp, Label falseTarget,
                                                                     RegisterFrame& frame) {
	auto left = gpOperand(cmp->getLeftInput(), frame);

	if (cmp->getLeftInput()->getStamp() == Type::ptr && isInteger(cmp->getRightInput()->getStamp())) {
		// Null-pointer check (see visitCompare): only EQ/NE are meaningful.
		cc.test(left, left);
		if (cmp->getComparator() == ir::CompareOperation::EQ) {
			cc.jnz(falseTarget);
		} else {
			cc.jz(falseTarget);
		}
		return;
	}

	// Keep visitCompare's `cmp x, 0` → `test x, x` peephole.
	const auto* rightConst = ir::dyn_cast<ir::ConstIntOperation>(cmp->getRightInput());
	if (rightConst != nullptr && rightConst->getValue() == 0) {
		cc.test(left, left);
	} else if (const auto rightImm = imm32Operand(cmp->getRightInput())) {
		cc.cmp(left, *rightImm);
		foldedImmediates_++;
	} else {
		cc.cmp(left, gpOperand(cmp->getRightInput(), frame));
	}

	if (isUnsignedType(cmp->getLeftInput()->getStamp())) {
		switch (cmp->getComparator()) {
		case ir::CompareOperation::EQ:
			cc.jne(falseTarget);
			break;
		case ir::CompareOperation::NE:
			cc.je(falseTarget);
			break;
		case ir::CompareOperation::LT:
			cc.jae(falseTarget);
			break;
		case ir::CompareOperation::LE:
			cc.ja(falseTarget);
			break;
		case ir::CompareOperation::GT:
			cc.jbe(falseTarget);
			break;
		case ir::CompareOperation::GE:
			cc.jb(falseTarget);
			break;
		}
	} else {
		switch (cmp->getComparator()) {
		case ir::CompareOperation::EQ:
			cc.jne(falseTarget);
			break;
		case ir::CompareOperation::NE:
			cc.je(falseTarget);
			break;
		case ir::CompareOperation::LT:
			cc.jge(falseTarget);
			break;
		case ir::CompareOperation::LE:
			cc.jg(falseTarget);
			break;
		case ir::CompareOperation::GT:
			cc.jle(falseTarget);
			break;
		case ir::CompareOperation::GE:
			cc.jl(falseTarget);
			break;
		}
	}
}

void AsmJitLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* op, RegisterFrame& frame) {
	const auto& bi = op->getNextBlockInvocation();
	processBlockInvocation(bi, frame);
	cc.jmp(getOrCreateLabel(bi.getBlock()->getIdentifier()));
	processBlock(bi.getBlock(), frame);
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
		auto falseGp = gpOperand(op->getFalseValue(), frame);
		emitMoveFromOperand(result, op->getTrueValue(), frame);
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
	auto addrGp = gpOperand(op->getAddress(), frame);
	auto result = allocReg(op->getStamp());

	if (op->getStamp() == Type::f32) {
		cc.movss(toXmm(result), x86::dword_ptr(addrGp));
	} else if (op->getStamp() == Type::f64) {
		cc.movsd(toXmm(result), x86::qword_ptr(addrGp));
	} else {
		auto gDst = toGp(result);
		switch (op->getStamp()) {
		case Type::b:
		case Type::ui8:
			cc.movzx(gDst.r32(), x86::byte_ptr(addrGp));
			break; // zero-extends to 64
		case Type::i8:
			cc.movsx(gDst.r64(), x86::byte_ptr(addrGp));
			break;
		case Type::ui16:
			cc.movzx(gDst.r32(), x86::word_ptr(addrGp));
			break; // zero-extends to 64
		case Type::i16:
			cc.movsx(gDst.r64(), x86::word_ptr(addrGp));
			break;
		case Type::ui32:
			cc.mov(gDst.r32(), x86::dword_ptr(addrGp));
			break; // zero-extends
		case Type::i32:
			cc.movsxd(gDst.r64(), x86::dword_ptr(addrGp));
			break;
		case Type::i64:
		case Type::ui64:
		case Type::ptr:
			cc.mov(gDst.r64(), x86::qword_ptr(addrGp));
			break;
		default:
			cc.mov(gDst.r64(), x86::qword_ptr(addrGp));
			break;
		}
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitStore(ir::StoreOperation* op, RegisterFrame& frame) {
	auto addrGp = gpOperand(op->getAddress(), frame);
	auto valReg = regOperand(op->getValue(), frame);

	if (op->getValue()->getStamp() == Type::f32) {
		cc.movss(x86::dword_ptr(addrGp), toXmm(valReg));
		return;
	}
	if (op->getValue()->getStamp() == Type::f64) {
		cc.movsd(x86::qword_ptr(addrGp), toXmm(valReg));
		return;
	}

	auto valGp = toGp(valReg);
	switch (op->getValue()->getStamp()) {
	case Type::b:
	case Type::i8:
	case Type::ui8:
		cc.mov(x86::byte_ptr(addrGp), valGp.r8());
		break;
	case Type::i16:
	case Type::ui16:
		cc.mov(x86::word_ptr(addrGp), valGp.r16());
		break;
	case Type::i32:
	case Type::ui32:
		cc.mov(x86::dword_ptr(addrGp), valGp.r32());
		break;
	default:
		cc.mov(x86::qword_ptr(addrGp), valGp.r64());
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

void AsmJitLoweringProvider::LoweringContext::visitProxyCall(ir::ProxyCallOperation* op, RegisterFrame& frame) {
	// Check the intrinsic manager first. A registered handler can fully
	// replace the scalar function-call lowering with native instructions
	// (e.g. emit `paddd xmm0, xmm1` for vector_add_i32x4_impl). The handler
	// is expected to bind the result identifier into the frame itself.
	if (auto intrinsic = intrinsicManager_.getIntrinsic(op->getFunctionPtr())) {
		// Intrinsic handlers read argument registers straight from the frame,
		// so materialise-and-bind any deferred constant argument first.
		for (auto* arg : op->getInputArguments()) {
			if (!frame.contains(arg->getIdentifier())) {
				frame.setValue(arg->getIdentifier(), regOperand(arg, frame));
			}
		}
		IntrinsicCallContext ctx {cc, op, frame};
		if ((*intrinsic)(ctx)) {
			return;
		}
	}

	// Build the callee's signature dynamically from the IR's type information.
	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

	// Resolve argument registers BEFORE emitting the InvokeNode: a deferred
	// constant rematerialises with a `mov reg, imm`, which must precede the
	// call in the instruction stream.
	std::vector<AsmReg> argRegs;
	argRegs.reserve(op->getInputArguments().size());
	for (auto* arg : op->getInputArguments()) {
		argRegs.push_back(regOperand(arg, frame));
	}

	InvokeNode* invokeNode = nullptr;
	auto it = funcNodes_.find(op->getFunctionName());
	if (it != funcNodes_.end()) {
		// Forward reference via AsmJit label — resolved at finalize() time.
		cc.invoke(&invokeNode, it->second->label(), sig);
	} else {
		// External function (not a Nautilus IR function): call by raw address.
		cc.invoke(&invokeNode, reinterpret_cast<uint64_t>(op->getFunctionPtr()), sig);
	}

	for (size_t i = 0; i < argRegs.size(); i++) {
		if (std::holds_alternative<Xmm>(argRegs[i]))
			invokeNode->setArg(i, toXmm(argRegs[i]));
		else
			invokeNode->setArg(i, toGp(argRegs[i]));
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
}

void AsmJitLoweringProvider::LoweringContext::visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame) {
	// Build callee signature from IR type information.
	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

	// The function pointer is a runtime GP register value.
	auto fnPtrGp = gpOperand(op->getFunctionPtrOperand(), frame);

	// Resolve argument registers BEFORE emitting the InvokeNode (see
	// visitProxyCall): rematerialisation movs must precede the call.
	const auto inputArgs = op->getInputArguments();
	std::vector<AsmReg> argRegs;
	argRegs.reserve(inputArgs.size());
	for (auto* arg : inputArgs) {
		argRegs.push_back(regOperand(arg, frame));
	}

	InvokeNode* invokeNode = nullptr;
	cc.invoke(&invokeNode, fnPtrGp, sig);

	for (size_t i = 0; i < argRegs.size(); i++) {
		if (std::holds_alternative<Xmm>(argRegs[i]))
			invokeNode->setArg(i, toXmm(argRegs[i]));
		else
			invokeNode->setArg(i, toGp(argRegs[i]));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Xmm>(result)) {
			invokeNode->setRet(0, toXmm(result));
		} else {
			invokeNode->setRet(0, toGp(result));
			// See visitProxyCall: narrow integer returns arrive with
			// unspecified upper bits and must be re-extended.
			narrowToStamp(toGp(result), op->getStamp());
		}
		bindResult(op->getIdentifier(), result, frame);
	}
}

void AsmJitLoweringProvider::LoweringContext::visitFunctionAddressOf(ir::FunctionAddressOfOperation* op,
                                                                     RegisterFrame& frame) {
	auto reg = allocReg(Type::ptr);
	auto it = funcNodes_.find(op->getFunctionName());
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
	auto src = regOperand(op->getInput(), frame);
	const Type srcType = op->getInput()->getStamp();
	const Type dstType = op->getStamp();
	auto result = allocReg(dstType);

	const bool srcIsFloat = isFloatType(srcType);
	const bool dstIsFloat = isFloatType(dstType);

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

} // namespace nautilus::compiler::asmjit
