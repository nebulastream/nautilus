
#include "nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp"
#include "nautilus/CompilationStatistics.hpp"
#include "nautilus/compiler/DumpHandler.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include <cassert>
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
	if (options.getOptionOrDefault<bool>("asmjit.enablePostRAPeephole", true)) {
		cc.addPassT<A64PostRAPeepholePass>(statistics);
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
		funcNodes_[funcOp->getName()] = cc.newFunc(sig);
	}

	// Pass 2: emit each function body.
	for (const auto& funcOp : functionOperations) {
		const auto& funcBlock = funcOp->getFunctionBasicBlock();
		const auto& entryArgs = funcBlock.getArguments();
		auto* funcNode = funcNodes_.at(funcOp->getName());

		cc.addFunc(funcNode);

		blockLabels.clear();
		processedBlocks.clear();
		functionAllocaSlots_.clear();

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
			rootFrame.setValue(entryArgs[i]->getIdentifier(), reg);

			// Sign/zero-extend narrow integer arguments to full 64-bit.
			if (std::holds_alternative<Gp>(reg)) {
				auto gReg = toGp(reg);
				if (stamp == Type::i8) {
					cc.sxtb(gReg.x(), gReg.w());
				} else if (stamp == Type::i16) {
					cc.sxth(gReg.x(), gReg.w());
				} else if (stamp == Type::ui8 || stamp == Type::b) {
					cc.uxtb(gReg.w(), gReg.w());
				} else if (stamp == Type::ui16) {
					cc.uxth(gReg.w(), gReg.w());
				} else if (stamp == Type::i32) {
					cc.sxtw(gReg.x(), gReg.w());
				} else if (stamp == Type::ui32) {
					cc.mov(gReg.w(), gReg.w()); // zero-extends upper 32 bits
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

	cc.bind(getOrCreateLabel(id));

	for (auto* op : block->getOperations()) {
		dispatch(op, frame);
	}
}

// ── Block-argument passing ────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::processBlockInvocation(const ir::BasicBlockInvocation& bi,
                                                                     RegisterFrame& frame) {
	const auto& srcArgs = bi.getArguments();
	const auto& dstArgs = bi.getBlock()->getArguments();

	if (srcArgs.empty())
		return;

	for (size_t i = 0; i < dstArgs.size(); i++) {
		const auto& dstId = dstArgs[i]->getIdentifier();
		if (!frame.contains(dstId)) {
			frame.setValue(dstId, allocReg(dstArgs[i]->getStamp()));
		}
	}

	std::vector<AsmReg> temps;
	temps.reserve(srcArgs.size());
	for (size_t i = 0; i < srcArgs.size(); i++) {
		auto src = frame.getValue(srcArgs[i]->getIdentifier());
		auto temp = allocReg(dstArgs[i]->getStamp());
		emitMove(temp, src);
		temps.push_back(temp);
	}

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
		cc.fadd(toVec(result), toVec(left), toVec(right));
	} else {
		cc.add(toGp(result), toGp(left), toGp(right));
		// An add that overflows the narrow stamp's width still produces a
		// "correct" 64-bit sum; re-extend per the result type so its
		// sign/zero-extension matches the wrapped-around narrow-width value
		// (see narrowToStamp's doc comment).
		narrowToStamp(toGp(result), op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitSub(ir::SubOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		cc.fsub(toVec(result), toVec(left), toVec(right));
	} else {
		cc.sub(toGp(result), toGp(left), toGp(right));
		narrowToStamp(toGp(result), op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMul(ir::MulOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		cc.fmul(toVec(result), toVec(left), toVec(right));
	} else {
		cc.mul(toGp(result), toGp(left), toGp(right));
		narrowToStamp(toGp(result), op->getStamp());
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitDiv(ir::DivOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		cc.fdiv(toVec(result), toVec(left), toVec(right));
	} else if (isUnsignedType(op->getStamp())) {
		cc.udiv(toGp(result), toGp(left), toGp(right));
	} else {
		cc.sdiv(toGp(result), toGp(left), toGp(right));
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitMod(ir::ModOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	// ARM64 has no remainder instruction. Compute: result = left - (left / right) * right
	auto quotient = cc.newInt64();
	if (isUnsignedType(op->getStamp())) {
		cc.udiv(quotient, toGp(left), toGp(right));
	} else {
		cc.sdiv(quotient, toGp(left), toGp(right));
	}
	// msub dst, quotient, right, left → dst = left - quotient * right
	cc.msub(toGp(result), quotient, toGp(right), toGp(left));
	bindResult(op->getIdentifier(), result, frame);
}

// ── Logical / compare ─────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitCompare(ir::CompareOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(Type::b);
	auto resultGp = toGp(result);
	const bool leftIsFloat = isFloatType(op->getLeftInput()->getStamp());
	const bool leftIsUnsigned = isUnsignedType(op->getLeftInput()->getStamp());

	// Map Nautilus comparator to ARM condition code.
	// arm::CondCode lives in asmjit/core/archcommons.h.
	using CC = arm::CondCode;
	uint32_t condCode;

	if (leftIsFloat) {
		cc.fcmp(toVec(left), toVec(right));
		// fcmp sets NZCV; for unordered (NaN), C=1, V=1.
		// Use unsigned condition codes which give correct IEEE 754 semantics.
		switch (op->getComparator()) {
		case ir::CompareOperation::EQ:
			condCode = static_cast<uint32_t>(CC::kEQ);
			break;
		case ir::CompareOperation::NE:
			condCode = static_cast<uint32_t>(CC::kNE);
			break;
		case ir::CompareOperation::LT:
			condCode = static_cast<uint32_t>(CC::kMI);
			break;
		case ir::CompareOperation::LE:
			condCode = static_cast<uint32_t>(CC::kLS);
			break;
		case ir::CompareOperation::GT:
			condCode = static_cast<uint32_t>(CC::kGT);
			break;
		case ir::CompareOperation::GE:
			condCode = static_cast<uint32_t>(CC::kGE);
			break;
		}
	} else if (op->getLeftInput()->getStamp() == Type::ptr && isInteger(op->getRightInput()->getStamp())) {
		// Null-pointer check.
		cc.tst(toGp(left), toGp(left));
		if (op->getComparator() == ir::CompareOperation::EQ)
			condCode = static_cast<uint32_t>(CC::kEQ);
		else
			condCode = static_cast<uint32_t>(CC::kNE);
	} else {
		// Peephole: `cmp x, <const 0>` → `cmp x, #0` (immediate form). Same
		// NZCV output, and it avoids depending on a separately-materialized
		// zero register, which lets that constant be dead-code-eliminated.
		const auto* rightConst = ir::dyn_cast<ir::ConstIntOperation>(op->getRightInput());
		if (rightConst != nullptr && rightConst->getValue() == 0) {
			cc.cmp(toGp(left), Imm(0));
		} else {
			cc.cmp(toGp(left), toGp(right));
		}
		if (leftIsUnsigned) {
			switch (op->getComparator()) {
			case ir::CompareOperation::EQ:
				condCode = static_cast<uint32_t>(CC::kEQ);
				break;
			case ir::CompareOperation::NE:
				condCode = static_cast<uint32_t>(CC::kNE);
				break;
			case ir::CompareOperation::LT:
				condCode = static_cast<uint32_t>(CC::kLO);
				break;
			case ir::CompareOperation::LE:
				condCode = static_cast<uint32_t>(CC::kLS);
				break;
			case ir::CompareOperation::GT:
				condCode = static_cast<uint32_t>(CC::kHI);
				break;
			case ir::CompareOperation::GE:
				condCode = static_cast<uint32_t>(CC::kHS);
				break;
			}
		} else {
			switch (op->getComparator()) {
			case ir::CompareOperation::EQ:
				condCode = static_cast<uint32_t>(CC::kEQ);
				break;
			case ir::CompareOperation::NE:
				condCode = static_cast<uint32_t>(CC::kNE);
				break;
			case ir::CompareOperation::LT:
				condCode = static_cast<uint32_t>(CC::kLT);
				break;
			case ir::CompareOperation::LE:
				condCode = static_cast<uint32_t>(CC::kLE);
				break;
			case ir::CompareOperation::GT:
				condCode = static_cast<uint32_t>(CC::kGT);
				break;
			case ir::CompareOperation::GE:
				condCode = static_cast<uint32_t>(CC::kGE);
				break;
			}
		}
	}
	cc.cset(resultGp, Imm(condCode));
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.and_(toGp(result), left, right);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.orr(toGp(result), left, right);
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNot(ir::NotOperation* op, RegisterFrame& frame) {
	auto input = toGp(frame.getValue(op->getInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.eor(toGp(result), input, Imm(1));
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* op, RegisterFrame& frame) {
	auto src = frame.getValue(op->getInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	cc.mvn(toGp(result), toGp(src));
	// mvn flips the full 64-bit register, including the extension padding.
	// That happens to stay correct for signed stamps (flipping a sign bit
	// flips its replicated extension consistently) but is wrong for unsigned
	// stamps, whose invariant is a zero-extended (not flipped) upper half.
	narrowToStamp(toGp(result), op->getStamp());
	bindResult(op->getIdentifier(), result, frame);
}

// ── Binary bit operations ─────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitShift(ir::ShiftOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	if (op->getType() == ir::ShiftOperation::LS) {
		cc.lsl(toGp(result), left, right);
	} else if (isUnsignedType(op->getStamp())) {
		cc.lsr(toGp(result), left, right);
	} else {
		cc.asr(toGp(result), left, right);
	}
	// Shifting the full 64-bit register (rather than just the narrow stamp's
	// width) can leave the extension padding inconsistent with the
	// narrow-width result -- e.g. a left shift that overflows the stamp's
	// width still computes a "correct" 64-bit shift, whose sign-extension no
	// longer matches the wrapped-around narrow-width value. asr already
	// shifts in the sign bit, but a shift can still move that bit into
	// positions that change the narrow-width result's own sign, so this is
	// needed for all three shift forms.
	narrowToStamp(toGp(result), op->getStamp());
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitBinaryComp(ir::BinaryCompOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	switch (op->getType()) {
	case ir::BinaryCompOperation::BAND:
		cc.and_(toGp(result), left, right);
		break;
	case ir::BinaryCompOperation::BOR:
		cc.orr(toGp(result), left, right);
		break;
	case ir::BinaryCompOperation::XOR:
		cc.eor(toGp(result), left, right);
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
	cc.cbz(condGp, elsePath);

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

void AsmJitLoweringProvider::LoweringContext::visitBranch(ir::BranchOperation* op, RegisterFrame& frame) {
	const auto& bi = op->getNextBlockInvocation();
	processBlockInvocation(bi, frame);
	cc.b(getOrCreateLabel(bi.getBlock()->getIdentifier()));
	processBlock(bi.getBlock(), frame);
}

void AsmJitLoweringProvider::LoweringContext::visitReturn(ir::ReturnOperation* op, RegisterFrame& frame) {
	if (op->hasReturnValue()) {
		auto retReg = frame.getValue(op->getReturnValue()->getIdentifier());
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
	auto condGp = toGp(frame.getValue(op->getCondition()->getIdentifier()));
	auto trueVal = frame.getValue(op->getTrueValue()->getIdentifier());
	auto falseVal = frame.getValue(op->getFalseValue()->getIdentifier());
	auto result = allocReg(op->getStamp());

	// Compare condition against zero, then use conditional select.
	cc.cmp(condGp, Imm(0));
	using CC = arm::CondCode;
	if (std::holds_alternative<Vec>(result)) {
		// fcsel dst, trueVec, falseVec, NE
		cc.fcsel(toVec(result), toVec(trueVal), toVec(falseVal), Imm(static_cast<uint32_t>(CC::kNE)));
	} else {
		// csel dst, trueGp, falseGp, NE
		cc.csel(toGp(result), toGp(trueVal), toGp(falseVal), Imm(static_cast<uint32_t>(CC::kNE)));
	}

	bindResult(op->getIdentifier(), result, frame);
}

// ── Memory ────────────────────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitLoad(ir::LoadOperation* op, RegisterFrame& frame) {
	auto addrGp = toGp(frame.getValue(op->getAddress()->getIdentifier()));
	auto result = allocReg(op->getStamp());

	if (op->getStamp() == Type::f32) {
		cc.ldr(toVec(result), a64::ptr(addrGp));
	} else if (op->getStamp() == Type::f64) {
		cc.ldr(toVec(result), a64::ptr(addrGp));
	} else {
		auto gDst = toGp(result);
		switch (op->getStamp()) {
		case Type::b:
		case Type::ui8:
			cc.ldrb(gDst.w(), a64::ptr(addrGp));
			break;
		case Type::i8:
			cc.ldrsb(gDst.x(), a64::ptr(addrGp));
			break;
		case Type::ui16:
			cc.ldrh(gDst.w(), a64::ptr(addrGp));
			break;
		case Type::i16:
			cc.ldrsh(gDst.x(), a64::ptr(addrGp));
			break;
		case Type::ui32:
			cc.ldr(gDst.w(), a64::ptr(addrGp));
			break;
		case Type::i32:
			cc.ldrsw(gDst.x(), a64::ptr(addrGp));
			break;
		case Type::i64:
		case Type::ui64:
		case Type::ptr:
			cc.ldr(gDst.x(), a64::ptr(addrGp));
			break;
		default:
			cc.ldr(gDst.x(), a64::ptr(addrGp));
			break;
		}
	}
	bindResult(op->getIdentifier(), result, frame);
}

void AsmJitLoweringProvider::LoweringContext::visitStore(ir::StoreOperation* op, RegisterFrame& frame) {
	auto addrGp = toGp(frame.getValue(op->getAddress()->getIdentifier()));
	auto valReg = frame.getValue(op->getValue()->getIdentifier());

	if (op->getValue()->getStamp() == Type::f32 || op->getValue()->getStamp() == Type::f64) {
		cc.str(toVec(valReg), a64::ptr(addrGp));
		return;
	}

	auto valGp = toGp(valReg);
	switch (op->getValue()->getStamp()) {
	case Type::b:
	case Type::i8:
	case Type::ui8:
		cc.strb(valGp.w(), a64::ptr(addrGp));
		break;
	case Type::i16:
	case Type::ui16:
		cc.strh(valGp.w(), a64::ptr(addrGp));
		break;
	case Type::i32:
	case Type::ui32:
		cc.str(valGp.w(), a64::ptr(addrGp));
		break;
	default:
		cc.str(valGp.x(), a64::ptr(addrGp));
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
	// replace the scalar function-call lowering with native NEON instructions
	// (e.g. emit `add v0.4s, v1.4s, v2.4s` for vector_add_i32x4_impl). The
	// handler is expected to bind the result identifier into the frame itself.
	if (auto intrinsic = intrinsicManager_.getIntrinsic(op->getFunctionPtr())) {
		IntrinsicCallContext ctx {cc, op, frame};
		if ((*intrinsic)(ctx)) {
			return;
		}
	}

	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

	// ARM64 blr requires a register operand — labels and immediates are not
	// valid operands.  Materialize the target address into a GP register.
	InvokeNode* invokeNode = nullptr;
	auto it = funcNodes_.find(op->getFunctionName());
	if (it != funcNodes_.end()) {
		// Internal JIT function: load label address via adr, then indirect call.
		auto tmp = cc.newGpx();
		cc.adr(tmp, it->second->label());
		cc.invoke(&invokeNode, tmp, sig);
	} else {
		// External function: load raw pointer into register, then indirect call.
		auto tmp = cc.newGpx();
		cc.mov(tmp, reinterpret_cast<uint64_t>(op->getFunctionPtr()));
		cc.invoke(&invokeNode, tmp, sig);
	}

	for (size_t i = 0; i < op->getInputArguments().size(); i++) {
		auto argReg = frame.getValue(op->getInputArguments()[i]->getIdentifier());
		if (std::holds_alternative<Vec>(argReg))
			invokeNode->setArg(i, toVec(argReg));
		else
			invokeNode->setArg(i, toGp(argReg));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Vec>(result))
			invokeNode->setRet(0, toVec(result));
		else
			invokeNode->setRet(0, toGp(result));
		bindResult(op->getIdentifier(), result, frame);
	}
}

void AsmJitLoweringProvider::LoweringContext::visitIndirectCall(ir::IndirectCallOperation* op, RegisterFrame& frame) {
	FuncSignature sig;
	sig.setRet(getTypeId(op->getStamp()));
	for (auto* arg : op->getInputArguments()) {
		sig.addArg(getTypeId(arg->getStamp()));
	}

	auto fnPtrGp = toGp(frame.getValue(op->getFunctionPtrOperand()->getIdentifier()));

	InvokeNode* invokeNode = nullptr;
	cc.invoke(&invokeNode, fnPtrGp.x(), sig);

	const auto inputArgs = op->getInputArguments();
	for (size_t i = 0; i < inputArgs.size(); i++) {
		auto argReg = frame.getValue(inputArgs[i]->getIdentifier());
		if (std::holds_alternative<Vec>(argReg))
			invokeNode->setArg(i, toVec(argReg));
		else
			invokeNode->setArg(i, toGp(argReg));
	}

	if (op->getStamp() != Type::v) {
		auto result = allocReg(op->getStamp());
		if (std::holds_alternative<Vec>(result))
			invokeNode->setRet(0, toVec(result));
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
		cc.adr(toGp(reg), it->second->label());
	} else {
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
		// Integer → integer: first extend from source width, then narrow to destination width.
		auto gSrc = toGp(src);
		auto gDst = toGp(result);
		// Step 1: extend from source width to get a clean 64-bit representation.
		switch (srcType) {
		case Type::i8:
			cc.sxtb(gDst.x(), gSrc.w());
			break;
		case Type::b:
		case Type::ui8:
			cc.uxtb(gDst.w(), gSrc.w());
			break;
		case Type::i16:
			cc.sxth(gDst.x(), gSrc.w());
			break;
		case Type::ui16:
			cc.uxth(gDst.w(), gSrc.w());
			break;
		case Type::i32:
			cc.sxtw(gDst.x(), gSrc.w());
			break;
		case Type::ui32:
			cc.mov(gDst.w(), gSrc.w()); // zero-extends to 64
			break;
		default:
			cc.mov(gDst, gSrc);
			break;
		}
		// Step 2: narrow to destination width. ARM64 has no sub-32-bit register
		// aliases, so the upper bits of W/X must be explicitly cleared to ensure
		// the caller sees the correct value (especially for return values).
		switch (dstType) {
		case Type::i8:
			cc.sxtb(gDst.x(), gDst.w());
			break;
		case Type::b:
		case Type::ui8:
			cc.uxtb(gDst.w(), gDst.w());
			break;
		case Type::i16:
			cc.sxth(gDst.x(), gDst.w());
			break;
		case Type::ui16:
			cc.uxth(gDst.w(), gDst.w());
			break;
		case Type::i32:
			cc.sxtw(gDst.x(), gDst.w());
			break;
		case Type::ui32:
			cc.mov(gDst.w(), gDst.w());
			break;
		default:
			break; // i64, ui64 — already 64-bit, no narrowing needed.
		}
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
			cc.sxtb(gDst.x(), gDst.w());
			break;
		case Type::b:
		case Type::ui8:
			cc.uxtb(gDst.w(), gDst.w());
			break;
		case Type::i16:
			cc.sxth(gDst.x(), gDst.w());
			break;
		case Type::ui16:
			cc.uxth(gDst.w(), gDst.w());
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

} // namespace nautilus::compiler::asmjit
