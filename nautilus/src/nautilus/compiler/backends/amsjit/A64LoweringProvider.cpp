
#include "nautilus/compiler/backends/amsjit/A64LoweringProvider.hpp"
#include "nautilus/CompilationStatistics.hpp"
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
                                                                  CompilationStatistics* statistics) {
	CodeHolder code;
	code.init(runtime.environment(), runtime.cpuFeatures());
	ThrowOnError errHandler;
	code.setErrorHandler(&errHandler);

	LoweringContext ctx(std::move(ir), code, options, statistics);
	ctx.processAll();

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
	return result;
}

// ── LoweringContext construction ──────────────────────────────────────────────

AsmJitLoweringProvider::LoweringContext::LoweringContext(std::shared_ptr<ir::IRGraph> ir, CodeHolder& code,
                                                         const engine::Options& options,
                                                         CompilationStatistics* statistics)
    : cc(&code), ir(std::move(ir)) {
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

// ── Two-pass compilation ──────────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::processAll() {
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
	frame.setValue(op->getIdentifier(), reg);
}

void AsmJitLoweringProvider::LoweringContext::visitConstInt(ir::ConstIntOperation* op, RegisterFrame& frame) {
	auto reg = allocReg(op->getStamp());
	cc.mov(toGp(reg), op->getValue());
	frame.setValue(op->getIdentifier(), reg);
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
		cc.fadd(toVec(result), toVec(left), toVec(right));
	} else {
		cc.add(toGp(result), toGp(left), toGp(right));
	}
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitSub(ir::SubOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		cc.fsub(toVec(result), toVec(left), toVec(right));
	} else {
		cc.sub(toGp(result), toGp(left), toGp(right));
	}
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitMul(ir::MulOperation* op, RegisterFrame& frame) {
	auto left = frame.getValue(op->getLeftInput()->getIdentifier());
	auto right = frame.getValue(op->getRightInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	if (isFloatType(op->getStamp())) {
		cc.fmul(toVec(result), toVec(left), toVec(right));
	} else {
		cc.mul(toGp(result), toGp(left), toGp(right));
	}
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
		cc.cmp(toGp(left), toGp(right));
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
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitAnd(ir::AndOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.and_(toGp(result), left, right);
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitOr(ir::OrOperation* op, RegisterFrame& frame) {
	auto left = toGp(frame.getValue(op->getLeftInput()->getIdentifier()));
	auto right = toGp(frame.getValue(op->getRightInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.orr(toGp(result), left, right);
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitNot(ir::NotOperation* op, RegisterFrame& frame) {
	auto input = toGp(frame.getValue(op->getInput()->getIdentifier()));
	auto result = allocReg(op->getStamp());
	cc.eor(toGp(result), input, Imm(1));
	frame.setValue(op->getIdentifier(), result);
}

void AsmJitLoweringProvider::LoweringContext::visitNegate(ir::NegateOperation* op, RegisterFrame& frame) {
	auto src = frame.getValue(op->getInput()->getIdentifier());
	auto result = allocReg(op->getStamp());
	cc.mvn(toGp(result), toGp(src));
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
			// Ensure sub-32-bit return values are properly narrowed so the
			// caller sees a clean W0 value that matches the ABI contract.
			auto retType = op->getReturnValue()->getStamp();
			switch (retType) {
			case Type::i8:
				cc.sxtb(gp.x(), gp.w());
				break;
			case Type::b:
			case Type::ui8:
				cc.uxtb(gp.w(), gp.w());
				break;
			case Type::i16:
				cc.sxth(gp.x(), gp.w());
				break;
			case Type::ui16:
				cc.uxth(gp.w(), gp.w());
				break;
			default:
				break;
			}
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

	frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
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
	auto stackMem = cc.newStack(static_cast<uint32_t>(op->getSize()), 8 /*align*/);
	auto ptrReg = cc.newIntPtr();
	cc.loadAddressOf(ptrReg, stackMem);
	frame.setValue(op->getIdentifier(), AsmReg(ptrReg));
}

// ── External function calls ───────────────────────────────────────────────────

void AsmJitLoweringProvider::LoweringContext::visitProxyCall(ir::ProxyCallOperation* op, RegisterFrame& frame) {
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
		frame.setValue(op->getIdentifier(), result);
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
		frame.setValue(op->getIdentifier(), result);
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
	frame.setValue(op->getIdentifier(), result);
}

} // namespace nautilus::compiler::asmjit
