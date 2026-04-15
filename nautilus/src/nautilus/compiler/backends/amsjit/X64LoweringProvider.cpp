
#include "nautilus/compiler/backends/amsjit/X64LoweringProvider.hpp"
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
                                                                  ::asmjit::JitRuntime& runtime) {
	CodeHolder code;
	code.init(runtime.environment(), runtime.cpuFeatures());
	ThrowOnError errHandler;
	code.setErrorHandler(&errHandler);

	LoweringContext ctx(std::move(ir), code);
	ctx.processAll();

	// Capture label offsets while code is still in the CodeHolder (before rt.add resets it).
	std::unordered_map<std::string, uint64_t> offsets;
	for (const auto& [name, funcNode] : ctx.getFuncNodes()) {
		offsets[name] = code.labelOffset(funcNode->label());
	}

	void* basePtr = nullptr;
	if (runtime.add(&basePtr, &code)) {
		return {};
	}

	LowerResult result;
	result.basePtr = basePtr;
	for (const auto& [name, offset] : offsets) {
		result.jitPtrs[name] = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(basePtr) + offset);
	}
	return result;
}

// ── LoweringContext construction ──────────────────────────────────────────────

AsmJitLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir, CodeHolder& code)
    : cc(&code), ir(std::move(ir)) {
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

// ── Two-pass compilation ──────────────────────────────────────────────────────
//
// Pass 1: call cc.newFunc() for every FunctionOperation, storing each FuncNode*.
//         This allocates stable Labels before any code is emitted, enabling
//         forward and mutual references between functions.
//
// Pass 2: for each function, call cc.addFunc(funcNode), emit the body,
//         and call cc.endFunc(). cc.finalize() resolves all label references.

void AsmJitLoweringProvider::LoweringContext::processAll() {
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
	frame.setValue(op->getIdentifier(), reg);
}

void AsmJitLoweringProvider::LoweringContext::visitConstInt(ir::ConstIntOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(op->getStamp());
	cc.mov(toGp(reg), op->getValue());
	frame.setValue(op->getIdentifier(), reg);
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
	frame.setValue(op->getIdentifier(), reg);
}

void AsmJitLoweringProvider::LoweringContext::visitConstPtr(ir::ConstPtrOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(Type::ptr);
	cc.mov(toGp(reg), reinterpret_cast<uint64_t>(op->getValue()));
	frame.setValue(op->getIdentifier(), reg);
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
	}
	frame.setValue(op->getIdentifier(), result);
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
	}
	frame.setValue(op->getIdentifier(), result);
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
	}
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
		if (op->getLeftInput()->getStamp() == Type::f32)
			cc.ucomiss(toXmm(left), toXmm(right));
		else
			cc.ucomisd(toXmm(left), toXmm(right));
		// ucomiss/ucomisd is unordered: NaN operands set CF=1 and ZF=1.
		// This gives correct IEEE 754 semantics (NaN comparisons always false for EQ/LT/LE/GT/GE, true for NE).
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
	} else if (op->getLeftInput()->getStamp() == Type::ptr && isInteger(op->getRightInput()->getStamp())) {
		// Null-pointer check: compare pointer against zero.
		cc.test(toGp(left), toGp(left));
		if (op->getComparator() == ir::CompareOperation::EQ)
			cc.sete(resultGp);
		else
			cc.setne(resultGp);
	} else {
		cc.cmp(toGp(left), toGp(right));
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
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), left);
	cc.and_(toGp(result), right);
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), left);
	cc.or_(toGp(result), right);
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitNot(ir::NotOperation* op, RegisterFrame& frame) {
	// NotOperation is logical NOT on a boolean: result = input XOR 1.
	auto input = toGp(frame.getValue(op->getInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), input);
	cc.xor_(toGp(result), 1);
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* op, RegisterFrame& frame) {
	// NegateOperation is bitwise NOT (~x): result = input XOR all-ones.
	auto src = frame.getValue(op->getInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), toGp(src));
	cc.not_(toGp(result));
	frame.setValue(op->getIdentifier(), result);
}

// ── Binary bit operations ─────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitShift(ir::ShiftOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.mov(toGp(result), left);
	// The shift count operand must be the CL register; AsmJit's Compiler
	// handles that constraint when a GP register is given as the count.
	if (op->getType() == ir::ShiftOperation::LS) {
		cc.shl(toGp(result), right.r8());
	} else if (isUnsignedType(op->getStamp())) {
		cc.shr(toGp(result), right.r8());
	} else {
		cc.sar(toGp(result), right.r8());
	}
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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

	// False branch: copy arguments then jump.
	cc.bind(elsePath);
	processBlockInvocation(op->getFalseBlockInvocation(), frame);
	cc.jmp(falseLabel);

	// Emit the bodies of both successor blocks.
	processBlock(op->getTrueBlockInvocation().getBlock(), frame);
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
		if (std::holds_alternative<Xmm>(retReg))
			cc.ret(toXmm(retReg));
		else
			cc.ret(toGp(retReg));
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

	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
	// Allocate aligned stack space and capture its address in a GP register.
	auto stackMem = cc.newStack(static_cast<uint32_t>(op->getSize()), 8 /*align*/);
	auto ptrReg = cc.newIntPtr();
	cc.lea(ptrReg, stackMem);
	frame.setValue(op->getIdentifier(), AsmReg(ptrReg));
}

// ── External function calls ───────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitProxyCall(ir::ProxyCallOperation* op, RegisterFrame& frame) {
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
		frame.setValue(op->getIdentifier(), result);
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
		frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), reg);
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
		// Integer → integer: sign/zero-extend from source width.
		auto gSrc = toGp(src);
		auto gDst = toGp(result);
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
	} else if (srcIsFloat && !dstIsFloat) {
		// Float → integer: truncate toward zero.
		auto gDst = toGp(result);
		auto xSrc = toXmm(src);
		if (srcType == Type::f32)
			cc.cvttss2si(gDst.r64(), xSrc);
		else
			cc.cvttsd2si(gDst.r64(), xSrc);
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
	frame.setValue(op->getIdentifier(), result);
}

} // namespace nautilus::compiler::asmjit
