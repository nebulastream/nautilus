
#include "nautilus/compiler/backends/amsjit/X64LoweringProvider.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
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
    : cc(&code), ir(std::move(ir)), intrinsicManager_(intrinsicManager) {
	// Register the optional post-RA peephole pass. It appends to the
	// Compiler's pass list *after* X86RAPass (which was installed during
	// `cc(&code)` via x86::Compiler::onAttach), so it sees physical-register
	// operands. Gated by an option so it can be toggled off for benchmarks
	// and regression investigations.
	if (options.getOptionOrDefault<bool>("asmjit.enablePostRAPeephole", true)) {
		cc.addPassT<X64PostRAPeepholePass>(statistics);
	}
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

	for (auto* op : block->getOperations()) {
		dispatch(op, frame);
	}
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

	// Ensure every destination block-argument has a virtual register.
	// If the identifier was already assigned (e.g. it matches a predecessor's
	// SSA value), reuse that register; otherwise allocate a fresh one now.
	for (size_t i = 0; i < dstArgs.size(); i++) {
		const auto& dstId = dstArgs[i]->getIdentifier();
		if (!frame.contains(dstId)) {
			frame.setValue(dstId, allocReg(dstArgs[i]->getStamp()));
		}
	}

	// Copy each source to a fresh temp register first (parallel-copy semantics
	// avoids clobbering when src and dst registers overlap).
	std::vector<AsmReg> temps;
	temps.reserve(srcArgs.size());
	for (size_t i = 0; i < srcArgs.size(); i++) {
		auto src = frame.getValue(srcArgs[i]->getIdentifier());
		auto temp = allocReg(dstArgs[i]->getStamp());
		emitMove(temp, src);
		temps.push_back(temp);
	}

	// Then write temps into the destination registers.
	for (size_t i = 0; i < srcArgs.size(); i++) {
		auto dst = frame.getValue(dstArgs[i]->getIdentifier());
		emitMove(dst, temps[i]);
	}
}

// ── Constants ─────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitConstBoolean(ir::ConstBooleanOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(Type::b);
	cc.mov(toGp(reg), static_cast<int64_t>(op->getValue() ? 1 : 0));
	bindResult(op->getIdentifier(), reg, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitConstInt(ir::ConstIntOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(op->getStamp());
	cc.mov(toGp(reg), op->getValue());
	bindResult(op->getIdentifier(), reg, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitConstFloat(ir::ConstFloatOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(op->getStamp());
	auto xmmReg = toXmm(reg);
	if (op->getStamp() == Type::f32) {
		float val = static_cast<float>(op->getValue());
		uint32_t bits = 0;
		memcpy(&bits, &val, sizeof(bits));
		auto tempGp = cc.newUInt32();
		cc.mov(tempGp, bits);
		cc.movd(xmmReg, tempGp);
	} else {
		double val = op->getValue();
		uint64_t bits = 0;
		memcpy(&bits, &val, sizeof(bits));
		auto tempGp = cc.newUInt64();
		cc.mov(tempGp, bits);
		cc.movq(xmmReg, tempGp);
	}
	bindResult(op->getIdentifier(), reg, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitConstPtr(ir::ConstPtrOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(Type::ptr);
	cc.mov(toGp(reg), reinterpret_cast<uint64_t>(op->getValue()));
	bindResult(op->getIdentifier(), reg, frame);
}

// ── Arithmetic ────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitAdd(ir::AddOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.addss(xDst, toXmm(right));
		else
			cc.addsd(xDst, toXmm(right));
	} else {
		auto gDst = toGp(result);
		cc.mov(gDst, toGp(left));
		cc.add(gDst, toGp(right));
		// An add that overflows the narrow stamp's width still produces a
		// "correct" 64-bit sum; re-extend per the result type so its
		// sign/zero-extension matches the wrapped-around narrow-width value
		// (see narrowToStamp's doc comment).
		narrowToStamp(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitSub(ir::SubOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.subss(xDst, toXmm(right));
		else
			cc.subsd(xDst, toXmm(right));
	} else {
		auto gDst = toGp(result);
		cc.mov(gDst, toGp(left));
		cc.sub(gDst, toGp(right));
		narrowToStamp(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMul(ir::MulOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		auto xDst = toXmm(result);
		cc.movaps(xDst, toXmm(left));
		if (op->getStamp() == Type::f32)
			cc.mulss(xDst, toXmm(right));
		else
			cc.mulsd(xDst, toXmm(right));
	} else {
		auto gDst = toGp(result);
		cc.mov(gDst, toGp(left));
		cc.imul(gDst, toGp(right));
		narrowToStamp(gDst, op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitDiv(ir::DivOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
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
		cc.mov(quot, toGp(left));
		if (isUnsignedType(op->getStamp())) {
			cc.xor_(rem, rem);
			cc.div(rem, quot, toGp(right));
		} else {
			cc.cqo(rem, quot);
			cc.idiv(rem, quot, toGp(right));
		}
		cc.mov(toGp(result), quot);
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMod(ir::ModOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	auto quot = cc.newInt64();
	auto rem = cc.newInt64();
	cc.mov(quot, toGp(left));
	if (isUnsignedType(op->getStamp())) {
		cc.xor_(rem, rem);
		cc.div(rem, quot, toGp(right));
	} else {
		cc.cqo(rem, quot);
		cc.idiv(rem, quot, toGp(right));
	}
	cc.mov(toGp(result), rem);
	bindResult(op->getIdentifier(), result, frame);
}

// ── Logical / compare ─────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitCompare(ir::CompareOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(Type::b);
	auto resultGp = toGp(result).r8();
	const bool leftIsFloat = isFloatType(op->getLeftInput()->getStamp());
	const bool leftIsUnsigned = isUnsignedType(op->getLeftInput()->getStamp());

	if (leftIsFloat) {
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
		cc.test(toGp(left), toGp(left));
		if (op->getComparator() == ir::CompareOperation::EQ)
			cc.sete(resultGp);
		else
			cc.setne(resultGp);
	} else {
		// Peephole: `cmp x, 0` → `test x, x` when the right operand is the
		// integer constant zero. Same flag output for ZF/SF/CF/OF and all
		// consumers here read only via setcc/jcc — two bytes shorter and
		// breaks no dependency.
		const auto* rightConst = ir::dyn_cast<ir::ConstIntOperation>(op->getRightInput());
		if (rightConst != nullptr && rightConst->getValue() == 0) {
			cc.test(toGp(left), toGp(left));
		} else {
			cc.cmp(toGp(left), toGp(right));
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
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), left);
	cc.and_(toGp(result), right);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), left);
	cc.or_(toGp(result), right);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNot(ir::NotOperation* op, RegisterFrame& frame) {
	// NotOperation is logical NOT on a boolean: result = input XOR 1.
	auto input = toGp(frame.getValue(op->getInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), input);
	cc.xor_(toGp(result), 1);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* op, RegisterFrame& frame) {
	// NegateOperation is bitwise NOT (~x): result = input XOR all-ones.
	auto src = frame.getValue(op->getInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	auto gDst = toGp(result);
	cc.mov(gDst, toGp(src));
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
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	auto gDst = toGp(result);
	cc.mov(gDst, left);
	// The shift count operand must be the CL register; AsmJit's Compiler
	// handles that constraint when a GP register is given as the count.
	if (op->getType() == ir::ShiftOperation::LS) {
		cc.shl(gDst, right.r8());
	} else if (isUnsignedType(op->getStamp())) {
		cc.shr(gDst, right.r8());
	} else {
		cc.sar(gDst, right.r8());
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
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), left);
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
	bindResult(op->getIdentifier(), result, frame);
}

// ── Control flow ──────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitIf(ir::IfOperation* op, RegisterFrame& frame) {
	auto condGp = toGp(frame.getValue(op->getValue()->getIdentifier()));
	auto trueLabel = getOrCreateLabel(op->getTrueBlockInvocation().getBlock()->getIdentifier());
	auto falseLabel = getOrCreateLabel(op->getFalseBlockInvocation().getBlock()->getIdentifier());

	auto elsePath = cc.newLabel();
	cc.test(condGp, condGp);
	cc.jz(elsePath);

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

void AsmJitLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* op, RegisterFrame& frame) {
	const auto& bi = op->getNextBlockInvocation();
	processBlockInvocation(bi, frame);
	cc.jmp(getOrCreateLabel(bi.getBlock()->getIdentifier()));
	processBlock(bi.getBlock(), frame);
}

void AsmJitLoweringProvider::LoweringContext::visitReturn(ir::ReturnOperation* op, RegisterFrame& frame) {
	if (op->hasReturnValue()) {
		auto retReg = frame.getValue(op->getReturnValue()->getIdentifier());
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
	auto condGp = toGp(frame.getValue(op->getCondition()->getIdentifier()));
	auto trueVal = frame.getValue(op->getTrueValue()->getIdentifier());
	auto falseVal = frame.getValue(op->getFalseValue()->getIdentifier());
	auto result = allocReg(op->getStamp());

	auto falsePath = cc.newLabel();
	auto donePath = cc.newLabel();

	cc.test(condGp, condGp);
	cc.jz(falsePath);
	emitMove(result, trueVal);
	cc.jmp(donePath);
	cc.bind(falsePath);
	emitMove(result, falseVal);
	cc.bind(donePath);

	bindResult(op->getIdentifier(), result, frame);
}

// ── Memory ────────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitLoad(ir::LoadOperation* op, RegisterFrame& frame) {
	auto addrGp = toGp(frame.getValue(op->getAddress()->getIdentifier()));
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
	auto addrGp = toGp(frame.getValue(op->getAddress()->getIdentifier()));
	auto valReg = frame.getValue(op->getValue()->getIdentifier());

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

	InvokeNode* invokeNode = nullptr;
	auto it = funcNodes_.find(op->getFunctionName());
	if (it != funcNodes_.end()) {
		// Forward reference via AsmJit label — resolved at finalize() time.
		cc.invoke(&invokeNode, it->second->label(), sig);
	} else {
		// External function (not a Nautilus IR function): call by raw address.
		cc.invoke(&invokeNode, reinterpret_cast<uint64_t>(op->getFunctionPtr()), sig);
	}

	for (size_t i = 0; i < op->getInputArguments().size(); i++) {
		auto argReg = frame.getValue(op->getInputArguments()[i]->getIdentifier());
		if (std::holds_alternative<Xmm>(argReg))
			invokeNode->setArg(i, toXmm(argReg));
		else
			invokeNode->setArg(i, toGp(argReg));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Xmm>(result))
			invokeNode->setRet(0, toXmm(result));
		else
			invokeNode->setRet(0, toGp(result));
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
	auto fnPtrGp = toGp(frame.getValue(op->getFunctionPtrOperand()->getIdentifier()));

	InvokeNode* invokeNode = nullptr;
	cc.invoke(&invokeNode, fnPtrGp, sig);

	const auto inputArgs = op->getInputArguments();
	for (size_t i = 0; i < inputArgs.size(); i++) {
		auto argReg = frame.getValue(inputArgs[i]->getIdentifier());
		if (std::holds_alternative<Xmm>(argReg))
			invokeNode->setArg(i, toXmm(argReg));
		else
			invokeNode->setArg(i, toGp(argReg));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Xmm>(result))
			invokeNode->setRet(0, toXmm(result));
		else
			invokeNode->setRet(0, toGp(result));
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
	auto src = frame.getValue(op->getInput()->getIdentifier());
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
		// integer→integer path above). Mirrors A64LoweringProvider::visitCast.
		auto gDst = toGp(result);
		auto xSrc = toXmm(src);
		if (srcType == Type::f32)
			cc.cvttss2si(gDst.r64(), xSrc);
		else
			cc.cvttsd2si(gDst.r64(), xSrc);
		narrowToStamp(gDst, dstType);
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
