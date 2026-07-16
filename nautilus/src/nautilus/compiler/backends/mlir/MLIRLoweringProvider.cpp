
#include "nautilus/compiler/backends/mlir/MLIRLoweringProvider.hpp"
#include "nautilus/compiler/backends/mlir/LLVMBackendHooks.hpp"
#include "nautilus/compiler/backends/mlir/debug/IRSourceMap.hpp"
#include "nautilus/compiler/backends/mlir/intrinsics/MLIRBackendIntrinsic.hpp"
#include "nautilus/compiler/ir/operations/AllocaOperation.hpp"
#include "nautilus/compiler/ir/operations/ArithmeticOperations/ModOperation.hpp"
#include "nautilus/compiler/ir/operations/IndirectCallOperation.hpp"
#include "nautilus/exceptions/NotImplementedException.hpp"
#include "nautilus/exceptions/RuntimeException.hpp"
#include "nautilus/tracing/Types.hpp"
#include <fmt/format.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/SmallVector.h>
#include <llvm/ADT/StringRef.h>
#include <llvm/Transforms/Utils/Cloning.h>
#include <mlir/Dialect/Arith/IR/Arith.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlow.h>
#include <mlir/Dialect/ControlFlow/IR/ControlFlowOps.h>
#include <mlir/Dialect/Func/IR/FuncOps.h>
#include <mlir/Dialect/LLVMIR/LLVMDialect.h>
#include <mlir/Dialect/LLVMIR/LLVMTypes.h>
#include <mlir/Dialect/Math/IR/Math.h>
#include <mlir/IR/Attributes.h>
#include <mlir/IR/Builders.h>
#include <mlir/IR/BuiltinAttributes.h>
#include <mlir/IR/Location.h>
#include <mlir/IR/Operation.h>
#include <mlir/IR/PatternMatch.h>
#include <mlir/IR/TypeRange.h>
#include <mlir/IR/Value.h>
#include <mlir/IR/Verifier.h>

namespace nautilus::compiler::mlir {
using namespace ::mlir;

//==-----------------------==//
//==-- UTILITY FUNCTIONS --==//
//==-----------------------==//
::mlir::Type MLIRLoweringProvider::getMLIRType(Type type) {
	switch (type) {
	case Type::v:
		return ::mlir::LLVM::LLVMVoidType::get(context);
	case Type::b:
		return builder->getI1Type();
	case Type::ui8:
	case Type::i8:
		return builder->getI8Type();
	case Type::ui16:
	case Type::i16:
		return builder->getI16Type();
	case Type::ui32:
	case Type::i32:
		return builder->getI32Type();
	case Type::ui64:
	case Type::i64:
		return builder->getI64Type();
	case Type::f32:
		return builder->getF32Type();
	case Type::f64:
		return builder->getF64Type();
	case Type::ptr:
		return mlir::LLVM::LLVMPointerType::get(context);
	}
	throw NotImplementedException("No matching type for stamp ");
}

std::vector<mlir::Type> MLIRLoweringProvider::getMLIRType(std::span<ir::Operation* const> types) {
	std::vector<mlir::Type> resultTypes;
	resultTypes.reserve(types.size());
	for (auto* type : types) {
		resultTypes.push_back(getMLIRType(type->getStamp()));
	}
	return resultTypes;
}

mlir::Value MLIRLoweringProvider::getConstInt(const std::string& location, Type stamp, int64_t value) {
	auto type = getMLIRType(stamp);
	return mlir::arith::ConstantOp::create(*builder, getNameLoc(location), type, builder->getIntegerAttr(type, value));
}

mlir::Value MLIRLoweringProvider::getConstBool(const std::string& location, bool value) {
	return mlir::LLVM::ConstantOp::create(*builder, getNameLoc(location), builder->getI1Type(),
	                                      builder->getIntegerAttr(builder->getIndexType(), value));
}

mlir::Location MLIRLoweringProvider::getNameLoc(const std::string& name) {
	// When debug info is active:
	//  * If we know which Nautilus op is currently being lowered, tag the
	//    NameLoc with the "$N" identifier and point at the op's line in
	//    the IR source dump so EmitDbgValuePass can attach the matching
	//    llvm.intr.dbg.declare.
	//  * Otherwise (locations created outside dispatch) fall back to the
	//    IR source file with line 0 so DIScopeForLLVMFuncOpPass still
	//    builds a DISubprogram whose DIFile refers to the real IR dump.
	if (debugInfo_.enable && irSourceMap_ != nullptr) {
		::mlir::StringAttr fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
		if (currentOp_ != nullptr && currentFunctionLines_ != nullptr) {
			const uint32_t id = currentOp_->getIdentifier().getId();
			if (auto it = currentFunctionLines_->operationLines.find(id);
			    it != currentFunctionLines_->operationLines.end()) {
				const std::string dollarName = "$" + std::to_string(id);
				auto baseLocation = mlir::FileLineColLoc::get(fileAttr, it->second, 1);
				return mlir::NameLoc::get(builder->getStringAttr(dollarName), baseLocation);
			}
		}
		// Terminators (br / if / return) have no `$N = ...` line to
		// consult, but `generateMLIR` set `currentOpLine_` from the
		// block's positional op-line list.  Use it so the translated
		// instruction carries a real !dbg and GDB stops on the
		// terminator line rather than skipping the whole block.
		if (currentOpLine_ != 0) {
			auto baseLocation = mlir::FileLineColLoc::get(fileAttr, currentOpLine_, 1);
			return mlir::NameLoc::get(builder->getStringAttr(name), baseLocation);
		}
		auto baseLocation = mlir::FileLineColLoc::get(fileAttr, 0, 0);
		return mlir::NameLoc::get(builder->getStringAttr(name), baseLocation);
	}
	auto baseLocation = mlir::FileLineColLoc::get(builder->getStringAttr("Query_1"), 0, 0);
	return mlir::NameLoc::get(builder->getStringAttr(name), baseLocation);
}

mlir::Location MLIRLoweringProvider::makeDollarLoc(uint32_t id, llvm::StringRef fallbackName) {
	if (!debugInfo_.enable || irSourceMap_ == nullptr || currentFunctionLines_ == nullptr) {
		return getNameLoc(fallbackName.str());
	}
	uint32_t line = 0;
	if (auto it = currentFunctionLines_->operationLines.find(id); it != currentFunctionLines_->operationLines.end()) {
		line = it->second;
	}
	auto fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
	auto fileLine = mlir::FileLineColLoc::get(fileAttr, line, 1);
	return mlir::NameLoc::get(builder->getStringAttr("$" + std::to_string(id)), fileLine);
}

mlir::Value MLIRLoweringProvider::ensureDebugAlloca(uint32_t id, mlir::Type type) {
	if (auto it = debugAllocas_.find(id); it != debugAllocas_.end()) {
		return it->second;
	}
	// Walk up to the enclosing func.func.  The alloca goes at the
	// entry block — a dynamic alloca in a loop body would leak stack
	// every iteration and isn't what DWARF expects for a local.
	auto* currentBlock = builder->getInsertionBlock();
	auto* funcOp = currentBlock ? currentBlock->getParentOp() : nullptr;
	while (funcOp != nullptr && !llvm::isa<mlir::func::FuncOp>(funcOp)) {
		funcOp = funcOp->getParentOp();
	}
	if (funcOp == nullptr) {
		return {};
	}
	auto& entryBlock = llvm::cast<mlir::func::FuncOp>(funcOp).getBody().front();

	// Two lines matter here:
	//  * prologueLine — the !dbg line on the alloca itself, kept at the
	//    function header so GDB collapses every alloca into a single
	//    "function entry" stop rather than bouncing between each
	//    variable's later decl line.
	//  * declLine — where the variable was introduced in the IR dump;
	//    surfaces as DILocalVariable.line, attached below as a side
	//    attribute so EmitDbgValuePass can find it without needing a
	//    handle to the IRSourceMap.
	uint32_t declLine = 0;
	if (currentFunctionLines_ != nullptr) {
		if (auto it = currentFunctionLines_->operationLines.find(id);
		    it != currentFunctionLines_->operationLines.end()) {
			declLine = it->second;
		}
	}
	const uint32_t prologueLine = currentFunctionHeaderLine_ != 0 ? currentFunctionHeaderLine_ : declLine;

	auto savedIP = builder->saveInsertionPoint();
	builder->setInsertionPointToStart(&entryBlock);
	auto ptrTy = mlir::LLVM::LLVMPointerType::get(context);
	auto i64Ty = mlir::IntegerType::get(context, 64);
	auto fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
	auto prologueFileLine = mlir::FileLineColLoc::get(fileAttr, prologueLine, 1);
	auto allocaLoc = mlir::NameLoc::get(builder->getStringAttr("$" + std::to_string(id)), prologueFileLine);
	auto one = mlir::LLVM::ConstantOp::create(*builder, allocaLoc, i64Ty, builder->getI64IntegerAttr(1));
	auto alloca = mlir::LLVM::AllocaOp::create(*builder, allocaLoc, ptrTy, type, one.getResult(), /*align=*/0u);
	alloca->setAttr("nautilus.debug.decl_line", builder->getI32IntegerAttr(declLine));
	builder->restoreInsertionPoint(savedIP);
	debugAllocas_[id] = alloca.getResult();
	return alloca.getResult();
}

void MLIRLoweringProvider::storeDebugValue(uint32_t id, mlir::Value value, mlir::Location loc) {
	if (!debugInfo_.enable || !value) {
		return;
	}
	auto alloca = ensureDebugAlloca(id, value.getType());
	if (!alloca) {
		return;
	}
	mlir::LLVM::StoreOp::create(*builder, loc, value, alloca);
}

void MLIRLoweringProvider::setDebugInfo(DebugInfoOptions debugInfo, std::shared_ptr<const IRSourceMap> sourceMap) {
	debugInfo_ = std::move(debugInfo);
	irSourceMap_ = std::move(sourceMap);
	// Retag the already-created ModuleOp with a location pointing at the
	// IR source dump.  DIScopeForLLVMFuncOpPass derives the
	// DICompileUnit's DW_AT_name / DW_AT_comp_dir from the module's
	// location, and without this step it would inherit the "Query_1"
	// placeholder that getNameLoc() produced at construction time, which
	// breaks GDB's ability to resolve the compilation-unit source path.
	if (debugInfo_.enable && !debugInfo_.sourceFile.empty() && theModule) {
		auto fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
		auto loc = mlir::FileLineColLoc::get(fileAttr, 1, 1);
		theModule->setLoc(loc);
	}
}

mlir::arith::CmpIPredicate convertToIntMLIRComparison(ir::CompareOperation::Comparator comparisonType, Type& stamp) {
	if (isSignedInteger(stamp)) {
		switch (comparisonType) {
		case (ir::CompareOperation::Comparator::EQ):
			return mlir::arith::CmpIPredicate::eq;
		case (ir::CompareOperation::Comparator::NE):
			return mlir::arith::CmpIPredicate::ne;
		case (ir::CompareOperation::Comparator::LT):
			return mlir::arith::CmpIPredicate::slt;
		case (ir::CompareOperation::Comparator::LE):
			return mlir::arith::CmpIPredicate::sle;
		case (ir::CompareOperation::Comparator::GT):
			return mlir::arith::CmpIPredicate::sgt;
		case (ir::CompareOperation::Comparator::GE):
			return mlir::arith::CmpIPredicate::sge;
		default:
			return mlir::arith::CmpIPredicate::slt;
		}
	} else if (isUnsignedInteger(stamp)) {
		switch (comparisonType) {
		case (ir::CompareOperation::Comparator::EQ):
			return mlir::arith::CmpIPredicate::eq;
		case (ir::CompareOperation::Comparator::NE):
			return mlir::arith::CmpIPredicate::ne;
		case (ir::CompareOperation::Comparator::LT):
			return mlir::arith::CmpIPredicate::ult;
		case (ir::CompareOperation::Comparator::LE):
			return mlir::arith::CmpIPredicate::ule;
		case (ir::CompareOperation::Comparator::GT):
			return mlir::arith::CmpIPredicate::ugt;
		case (ir::CompareOperation::Comparator::GE):
			return mlir::arith::CmpIPredicate::uge;
		default:
			return mlir::arith::CmpIPredicate::ult;
		}
	}
	throw RuntimeException("");
}

mlir::arith::CmpFPredicate convertToFloatMLIRComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons
	// are always signed.
	case (ir::CompareOperation::Comparator::LT):
		return mlir::arith::CmpFPredicate::OLT;
	case (ir::CompareOperation::Comparator::LE):
		return mlir::arith::CmpFPredicate::OLE;
	case (ir::CompareOperation::Comparator::EQ):
		return mlir::arith::CmpFPredicate::OEQ;
	case (ir::CompareOperation::Comparator::GT):
		return mlir::arith::CmpFPredicate::OGT;
	case (ir::CompareOperation::Comparator::GE):
		return mlir::arith::CmpFPredicate::OGE;
	case (ir::CompareOperation::Comparator::NE):
		// Unlike EQ/LT/LE/GT/GE (which are all false whenever either operand is
		// NaN, matching C++'s "ordered" float comparison semantics), C++'s `!=`
		// is true whenever either operand is NaN -- it's the negation of `==`,
		// not an independently-ordered predicate. ONE ("ordered and not-equal")
		// is false for NaN operands and would silently disagree with `!=` on
		// every NaN comparison; UNE ("unordered or not-equal") is the correct
		// match.
		return mlir::arith::CmpFPredicate::UNE;
	default:
		return mlir::arith::CmpFPredicate::OLT;
	}
}

mlir::LLVM::ICmpPredicate convertToLLVMComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons
	// are always signed.
	case (ir::CompareOperation::Comparator::LT):
		return mlir::LLVM::ICmpPredicate::ult;
	case (ir::CompareOperation::Comparator::LE):
		return mlir::LLVM::ICmpPredicate::ule;
	case (ir::CompareOperation::Comparator::EQ):
		return mlir::LLVM::ICmpPredicate::eq;
	case (ir::CompareOperation::Comparator::GT):
		return mlir::LLVM::ICmpPredicate::ugt;
	case (ir::CompareOperation::Comparator::GE):
		return mlir::LLVM::ICmpPredicate::uge;
	case (ir::CompareOperation::Comparator::NE):
		return mlir::LLVM::ICmpPredicate::ne;
	default:
		assert(false);
		return mlir::LLVM::ICmpPredicate::ult;
	}
}

mlir::arith::CmpIPredicate convertToBooleanMLIRComparison(ir::CompareOperation::Comparator comparisonType) {
	switch (comparisonType) {
	// the U in U(LT/LE/..) stands for unordered, not unsigned! Float comparisons
	// are always signed.
	case (ir::CompareOperation::Comparator::EQ):
		return mlir::arith::CmpIPredicate::eq;
	case (ir::CompareOperation::Comparator::NE):
		return mlir::arith::CmpIPredicate::ne;
	case ir::CompareOperation::LT:
		return mlir::arith::CmpIPredicate::slt;
	case ir::CompareOperation::LE:
		return mlir::arith::CmpIPredicate::sle;
	case ir::CompareOperation::GT:
		return mlir::arith::CmpIPredicate::sgt;
	case ir::CompareOperation::GE:
		return mlir::arith::CmpIPredicate::sge;
	default:
		assert(false);
		return mlir::arith::CmpIPredicate::sge;
	}
}

mlir::LLVM::MemoryEffectsAttr getMemoryEffectsAttr(const FunctionAttributes& fnAttrs, mlir::MLIRContext* context) {
	switch (fnAttrs.modRefInfo) {
	case nautilus::ModRefInfo::NoModRef: {
		return mlir::LLVM::MemoryEffectsAttr::get(context, LLVM::ModRefInfo::NoModRef, LLVM::ModRefInfo::NoModRef,
		                                          LLVM::ModRefInfo::NoModRef, LLVM::ModRefInfo::NoModRef,
		                                          LLVM::ModRefInfo::NoModRef, LLVM::ModRefInfo::NoModRef);
	}
	case nautilus::ModRefInfo::Ref: {
		return mlir::LLVM::MemoryEffectsAttr::get(context, LLVM::ModRefInfo::Ref, LLVM::ModRefInfo::Ref,
		                                          LLVM::ModRefInfo::Ref, LLVM::ModRefInfo::Ref, LLVM::ModRefInfo::Ref,
		                                          LLVM::ModRefInfo::Ref);
	}
	case nautilus::ModRefInfo::ModRef: {
		return mlir::LLVM::MemoryEffectsAttr::get(context, LLVM::ModRefInfo::ModRef, LLVM::ModRefInfo::ModRef,
		                                          LLVM::ModRefInfo::ModRef, LLVM::ModRefInfo::ModRef,
		                                          LLVM::ModRefInfo::ModRef, LLVM::ModRefInfo::ModRef);
	}
	case nautilus::ModRefInfo::Mod: {
		return mlir::LLVM::MemoryEffectsAttr::get(context, LLVM::ModRefInfo::Mod, LLVM::ModRefInfo::Mod,
		                                          LLVM::ModRefInfo::Mod, LLVM::ModRefInfo::Mod, LLVM::ModRefInfo::Mod,
		                                          LLVM::ModRefInfo::Mod);
	}
	default: {
		throw NotImplementedException("Unsupported ModRefInfo");
	}
	}
}

void setLLVMAttributes(mlir::LLVM::LLVMFuncOp& funcOp, const FunctionAttributes& fnAttrs,
                       ::mlir::MLIRContext* context) {
	funcOp.setMemoryEffectsAttr(getMemoryEffectsAttr(fnAttrs, context));
	funcOp.setWillReturn(fnAttrs.willReturn);
	funcOp.setNoUnwind(fnAttrs.noUnwind);
}

static mlir::LLVM::ModRefInfo toLLVMModRef(nautilus::ModRefInfo m) {
	using MR = mlir::LLVM::ModRefInfo;
	switch (m) {
	case nautilus::ModRefInfo::NoModRef:
		return MR::NoModRef;
	case nautilus::ModRefInfo::Ref:
		return MR::Ref;
	case nautilus::ModRefInfo::Mod:
		return MR::Mod;
	case nautilus::ModRefInfo::ModRef:
		return MR::ModRef;
	}
	return MR::ModRef; // fallback
}

void setFuncAttributes(mlir::func::FuncOp funcOp, const FunctionAttributes& fnAttrs) {
	mlir::MLIRContext* ctx = funcOp.getContext();

	// (A) Prefer structured memory effects over passthrough("memory(...)").
	// Pick a conservative mapping; tweak argMem/inaccessibleMem/errnoMem if you can.
	auto mr = toLLVMModRef(fnAttrs.modRefInfo);
	auto memEffects = mlir::LLVM::MemoryEffectsAttr::get(ctx,
	                                                     /*other=*/mr,
	                                                     /*argMem=*/mr,
	                                                     /*inaccessibleMem=*/mr,
	                                                     /*errnoMem=*/mr,
	                                                     /*targetMem0=*/mr,
	                                                     /*targetMem1=*/mr);

	// MUST be discardable to survive convert-func-to-llvm.
	funcOp->setDiscardableAttr("memory_effects", memEffects);

	// (B) Forward plain LLVM IR attributes via passthrough (optional).
	llvm::SmallVector<mlir::Attribute> passthrough;

	if (fnAttrs.willReturn)
		passthrough.push_back(mlir::StringAttr::get(ctx, "willreturn"));
	if (fnAttrs.noUnwind)
		passthrough.push_back(mlir::StringAttr::get(ctx, "nounwind"));

	// Important: don't also add "memory(...)" here if you already set memory_effects,
	// or you'll end up with two competing ways to describe memory behavior.
	if (!passthrough.empty())
		funcOp->setDiscardableAttr("passthrough", mlir::ArrayAttr::get(ctx, passthrough));
}

mlir::FlatSymbolRefAttr MLIRLoweringProvider::insertExternalFunction(const std::string& name, void* functionPtr,
                                                                     const mlir::Type& resultType,
                                                                     const std::vector<mlir::Type>& argTypes,
                                                                     const std::vector<Type>& argStamps,
                                                                     const FunctionAttributes& fnAttrs) {
	// The InsertionGuard saves the current insertion point (IP) and restores it
	// after scope is left.
	mlir::PatternRewriter::InsertionGuard insertGuard(*builder);
	builder->restoreInsertionPoint(*globalInsertPoint);

	// Create function name (potentially using pointer address for inline optimization).
	// A plugin may install a hook that provides an alternative name for
	// external proxy calls given the runtime address (e.g. the inlining
	// plugin returns the hex-formatted pointer so the JIT-time inliner can
	// match it against registered bitcode).
	std::string functionName = name;
	if (options->getOptionOrDefault("mlir.inline_invoke_calls", false)) {
		if (const auto& hook = getLLVMBackendHooks().proxyCallNameOverride) {
			if (auto overridden = hook(functionPtr)) {
				functionName = *overridden;
			}
		}
	}

	// Use func dialect for external functions to enable better optimization
	// Handle void vs non-void return types
	llvm::SmallVector<mlir::Type> resultTypes;
	if (!llvm::isa<mlir::LLVM::LLVMVoidType>(resultType)) {
		resultTypes.push_back(resultType);
	}

	auto functionType = builder->getFunctionType(argTypes, resultTypes);
	auto funcOp = mlir::func::FuncOp::create(*builder, theModule.getLoc(), functionName, functionType);

	// Mark as private (will be converted to external linkage during lowering if needed)
	funcOp.setPrivate();

	// Sub-32-bit integer arguments must carry an explicit extension attribute:
	// several C ABIs (Darwin AArch64, x86-64 SysV) require the *caller* to
	// sign/zero-extend them, and the natively compiled callee assumes that
	// happened. Without the attribute LLVM emits the call with whatever the
	// register's upper bits held (e.g. bits of a truncated pointer), and the
	// callee mis-reads the argument. The *result* deliberately gets no such
	// attribute: not every ABI guarantees the callee extends a narrow return
	// (AAPCS64 does not), and an unattributed narrow result is always safe --
	// LLVM re-extends it itself before any wider use.
	for (size_t i = 0; i < argStamps.size() && i < argTypes.size(); ++i) {
		switch (argStamps[i]) {
		case Type::i8:
		case Type::i16:
			funcOp.setArgAttr(static_cast<unsigned>(i), "llvm.signext", mlir::UnitAttr::get(context));
			break;
		case Type::b:
		case Type::ui8:
		case Type::ui16:
			funcOp.setArgAttr(static_cast<unsigned>(i), "llvm.zeroext", mlir::UnitAttr::get(context));
			break;
		default:
			break;
		}
	}

	// Set function attributes
	setFuncAttributes(funcOp, fnAttrs);

	jitProxyFunctionSymbols.push_back(functionName);
	if (functionPtr == nullptr) {
		functionPtr = ProxyFunctions.getProxyFunctionAddress(functionName);
	}
	jitProxyFunctionTargetAddresses.push_back(functionPtr);
	return mlir::SymbolRefAttr::get(context, functionName);
}

//==---------------------------------==//
//==-- MAIN WORK - Generating MLIR --==//
//==---------------------------------==//
MLIRLoweringProvider::MLIRLoweringProvider(mlir::MLIRContext& context, const engine::Options& options,
                                           MLIRIntrinsicManager& intrinsicManager)
    : intrinsicManager(intrinsicManager), context(&context), options(&options) {
	// Create builder object, which helps to generate MLIR. Create Module, which
	// contains generated MLIR.
	builder = std::make_unique<mlir::OpBuilder>(&context);
	builder->getContext()->loadDialect<mlir::arith::ArithDialect>();
	builder->getContext()->loadDialect<mlir::math::MathDialect>();
	builder->getContext()->loadDialect<mlir::cf::ControlFlowDialect>();
	builder->getContext()->loadDialect<mlir::LLVM::LLVMDialect>();
	builder->getContext()->loadDialect<mlir::func::FuncDialect>();
	this->theModule = mlir::ModuleOp::create(getNameLoc("module"));
	// Store InsertPoint for inserting globals such as Strings or TupleBuffers.
	globalInsertPoint = new mlir::RewriterBase::InsertPoint(theModule.getBody(), theModule.begin());
}

MLIRLoweringProvider::~MLIRLoweringProvider() {
	delete globalInsertPoint;
}

mlir::OwningOpRef<mlir::ModuleOp> MLIRLoweringProvider::generateModuleFromIR(std::shared_ptr<ir::IRGraph> ir) {
	// Generate MLIR for all function operations in the IR graph
	const auto& functions = ir->getFunctionOperations();

	// New path: multiple functions via TraceModule
	std::unordered_map<std::string, ::mlir::func::FuncOp> functionDefinitions;
	for (const auto& functionOp : functions) {
		auto defintion = this->generateFunctionDefinitions(*functionOp);
		functionDefinitions.insert({functionOp->getName(), defintion});
	}

	for (const auto& functionOp : functions) {
		ValueFrame frame;
		auto& funcref = functionDefinitions.at(functionOp->getName());
		generateFunction(funcref, *functionOp, frame);
	}

	// If MLIR module creation is incorrect, gracefully emit error message, return
	// nullptr, and continue.
	if (failed(mlir::verify(theModule))) {
		theModule.emitError("module verification error");
		return nullptr;
	}
	return theModule;
}

void MLIRLoweringProvider::generateMLIR(const ir::BasicBlock* basicBlock, ValueFrame& frame) {
	// Pull the positional op-line list for this block — used to give
	// terminators (`br`, `if`, `return`) a non-zero !dbg even though
	// they have no `$N` id to look up in operationLines.
	const std::vector<uint32_t>* opLines = nullptr;
	if (debugInfo_.enable && currentFunctionLines_ != nullptr) {
		if (auto it = currentFunctionLines_->blockOpLines.find(basicBlock->getIdentifier().getId());
		    it != currentFunctionLines_->blockOpLines.end()) {
			opLines = &it->second;
		}
	}
	size_t opIdx = 0;
	for (auto* operation : basicBlock->getOperations()) {
		// Record which Nautilus op is currently being lowered so
		// getNameLoc() can produce a FileLineColLoc pointing at the
		// right line of the IR source dump.  Cleared after dispatch so
		// helper ops created outside the visitXxx hooks don't inherit
		// a stale location.
		currentOp_ = operation;
		currentOpLine_ = (opLines && opIdx < opLines->size()) ? (*opLines)[opIdx] : 0;
		dispatch(operation, frame);
		currentOp_ = nullptr;
		currentOpLine_ = 0;

		// Shadow-store the op's result into its $N alloca.  Control-flow
		// ops (branch, return) don't register a value and are skipped.
		if (debugInfo_.enable && frame.contains(operation->getIdentifier())) {
			const uint32_t id = operation->getIdentifier().getId();
			if (auto produced = resolveOperand(operation, frame)) {
				storeDebugValue(id, produced, makeDollarLoc(id, "debug.store"));
			}
		}
		++opIdx;
	}
}

void MLIRLoweringProvider::visitNegate(ir::NegateOperation* negateOperation, ValueFrame& frame) {
	auto input = resolveOperand(negateOperation->getInput(), frame);
	if (isFloat(negateOperation->getStamp())) {
		auto negate = mlir::LLVM::FNegOp::create(*builder, getNameLoc("binOpResult"), input.getType(), input,
		                                         mlir::LLVM::FastmathFlags::none);
		bind(frame, negateOperation, negate);
	} else {
		auto constInt = mlir::arith::ConstantOp::create(*builder, getNameLoc("location"), input.getType(),
		                                                builder->getIntegerAttr(input.getType(), ~0));
		auto negate = mlir::arith::XOrIOp::create(*builder, getNameLoc("comparison"), input, constInt);
		bind(frame, negateOperation, negate);
	}
}

void MLIRLoweringProvider::visitNot(ir::NotOperation* notOperation, ValueFrame& frame) {
	auto input = resolveOperand(notOperation->getInput(), frame);
	auto constInt = getConstBool("loc", true);
	auto negate = mlir::arith::XOrIOp::create(*builder, getNameLoc("comparison"), input, constInt);
	bind(frame, notOperation, negate);
}

void MLIRLoweringProvider::visitSelect(ir::SelectOperation* selectOperation, ValueFrame& frame) {
	auto condition = resolveOperand(selectOperation->getCondition(), frame);
	auto trueValue = resolveOperand(selectOperation->getTrueValue(), frame);
	auto falseValue = resolveOperand(selectOperation->getFalseValue(), frame);
	auto mlirSelectOp =
	    mlir::arith::SelectOp::create(*builder, getNameLoc("selectResult"), condition, trueValue, falseValue);
	bind(frame, selectOperation, mlirSelectOp);
}

void MLIRLoweringProvider::visitOr(ir::OrOperation* orOperation, ValueFrame& frame) {
	auto leftInput = resolveOperand(orOperation->getLeftInput(), frame);
	auto rightInput = resolveOperand(orOperation->getRightInput(), frame);
	auto mlirOrOp = mlir::LLVM::OrOp::create(*builder, getNameLoc("binOpResult"), leftInput, rightInput);
	bind(frame, orOperation, mlirOrOp);
}

void MLIRLoweringProvider::visitAnd(ir::AndOperation* andOperation, ValueFrame& frame) {
	auto leftInput = resolveOperand(andOperation->getLeftInput(), frame);
	auto rightInput = resolveOperand(andOperation->getRightInput(), frame);
	auto mlirAndOp = mlir::LLVM::AndOp::create(*builder, getNameLoc("binOpResult"), leftInput, rightInput);
	bind(frame, andOperation, mlirAndOp);
}

::mlir::func::FuncOp MLIRLoweringProvider::generateFunctionDefinitions(const ir::FunctionOperation& functionOp) {
	// Currently, no function definitions other than the main execute function are created.
	// This is a placeholder for future functionality.

	// Generate execute function. Set input/output types and get its entry block.
	llvm::SmallVector<mlir::Type> inputTypes(0);
	for (auto& inputArg : functionOp.getFunctionBasicBlock().getArguments()) {
		inputTypes.emplace_back(getMLIRType(inputArg->getStamp()));
	}

	// Handle void vs non-void return types
	llvm::SmallVector<mlir::Type> outputTypes;
	if (functionOp.getOutputArg() != Type::v) {
		outputTypes.push_back(getMLIRType(functionOp.getOutputArg()));
	}

	auto functionInOutTypes = builder->getFunctionType(inputTypes, outputTypes);

	// Locate the function on the line where its header appears in the IR
	// source dump.  Without this, DIScopeForLLVMFuncOpPass synthesizes a
	// DISubprogram with `line: 0`, which DWARF treats as "no location" —
	// GDB's `step` cannot land inside the function even though per-op
	// !dbg metadata on the body is correct.  `functionLines` is populated
	// by dumpIRWithSourceMap for exactly this purpose.
	::mlir::Location loc = getNameLoc("EntryPoint");
	if (debugInfo_.enable && irSourceMap_ != nullptr) {
		uint32_t line = 0;
		if (auto it = irSourceMap_->functionLines.find(functionOp.getName()); it != irSourceMap_->functionLines.end()) {
			line = it->second;
		}
		auto fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
		auto fileLoc = mlir::FileLineColLoc::get(fileAttr, line, 1);
		loc = mlir::NameLoc::get(builder->getStringAttr(functionOp.getName()), fileLoc);
	}

	// Intentionally do NOT attach a DISubprogramAttr via FusedLoc here.
	// The `llvm.emit_c_interface` attribute makes convert-func-to-llvm
	// generate both `execute` and `_mlir_ciface_execute`; both get the
	// function's source location, which would end up sharing the same
	// DISubprogram node and fail LLVM's "DISubprogram attached to more
	// than one function" verification.  DIScopeForLLVMFuncOpPass, which
	// runs after conversion, materializes a distinct DISubprogram on
	// every llvm.func that lacks one — correct for both functions.

	auto mlirFunction = mlir::func::FuncOp::create(*builder, loc, functionOp.getName(), functionInOutTypes);

	// Avoid function name mangling.
	mlirFunction->setAttr("llvm.emit_c_interface", mlir::UnitAttr::get(context));

	// Only set result attributes if the function returns a value
	if (functionOp.getOutputArg() != Type::v) {
		if (isUnsignedInteger(functionOp.getStamp())) {
			mlirFunction.setResultAttr(0, "llvm.zeroext", mlir::UnitAttr::get(context));
		} else if (isSignedInteger(functionOp.getStamp())) {
			mlirFunction.setResultAttr(0, "llvm.signext", mlir::UnitAttr::get(context));
		}
	}

	theModule.push_back(mlirFunction);
	return mlirFunction;
}

void MLIRLoweringProvider::generateFunction(mlir::func::FuncOp& mlirFunction, const ir::FunctionOperation& functionOp,
                                            ValueFrame& frame) {
	// Clear per-function state so that basic block identifiers from a previous
	// function are not incorrectly reused.  Block identifiers are simple numeric
	// strings that restart at 0 for every function, so stale entries would cause
	// the new function's control-flow blocks to alias blocks from the previous
	// function.
	blockMapping.clear();
	definedValues.clear();
	inductionVars.clear();
	debugAllocas_.clear();
	functionAllocaSlots_.clear();
	currentFunctionHeaderLine_ = 0;
	currentFunctionLines_ = nullptr;
	if (debugInfo_.enable && irSourceMap_ != nullptr) {
		if (auto it = irSourceMap_->functionLines.find(functionOp.getName()); it != irSourceMap_->functionLines.end()) {
			currentFunctionHeaderLine_ = it->second;
		}
		if (auto it = irSourceMap_->functions.find(functionOp.getName()); it != irSourceMap_->functions.end()) {
			currentFunctionLines_ = &it->second;
		}
	}

	// add entry block for the function
	mlirFunction.addEntryBlock();

	// Set InsertPoint to beginning of the execute function.
	builder->setInsertionPointToStart(&mlirFunction.getBody().front());

	// Materialise the function's alloca table into one LLVM alloca per
	// entry, pinned at the prologue.  visitAlloca() just looks the slot up
	// by index later.  Doing this here (rather than per-use) replaces the
	// old hoisting phase: the alloca order is determined by the trace's
	// allocaSpecs vector, not by the position of the AllocaOperation in
	// any block.
	const auto& allocaSpecs = functionOp.getAllocaSpecs();
	if (!allocaSpecs.empty()) {
		auto i8Type = builder->getI8Type();
		auto ptrTy = LLVM::LLVMPointerType::get(context);
		auto i64Ty = IntegerType::get(context, 64);
		functionAllocaSlots_.reserve(allocaSpecs.size());
		for (const auto& spec : allocaSpecs) {
			Value sizeVal = LLVM::ConstantOp::create(*builder, getNameLoc("location"), i64Ty,
			                                         builder->getI64IntegerAttr(spec.size));
			auto align = static_cast<uint32_t>(std::max<size_t>(spec.align, 1));
			auto slot = LLVM::AllocaOp::create(*builder, getNameLoc("location"), ptrTy, i8Type, sizeVal, align);
			functionAllocaSlots_.emplace_back(slot);
		}
	}

	// Store references to function args in the valueMap map.
	auto valueMapIterator = mlirFunction.args_begin();
	const auto& irArgs = functionOp.getFunctionBasicBlock().getArguments();
	for (int i = 0; i < (int) irArgs.size(); ++i) {
		bind(frame, irArgs.at(i), valueMapIterator[i]);
		// When debug info is active, tag the entry-block argument with a
		// `$N` NameLoc and emit a store into its shadow alloca.  The
		// store's !dbg points at the function header line — we want the
		// prologue (allocas + param saves) to collapse into one GDB
		// stop instead of bouncing through each param's later decl line.
		if (debugInfo_.enable && irSourceMap_ != nullptr) {
			const uint32_t id = irArgs.at(i)->getIdentifier().getId();
			auto argNameLoc = makeDollarLoc(id, "arg");
			mlirFunction.getArgument(i).setLoc(argNameLoc);
			auto fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
			auto storeLoc = mlir::FileLineColLoc::get(fileAttr, currentFunctionHeaderLine_, 1);
			storeDebugValue(id, mlirFunction.getArgument(i), storeLoc);
		}
	}

	// Generate MLIR for operations in function body (BasicBlock).
	generateMLIR(&functionOp.getFunctionBasicBlock(), frame);

	// Don't add the function again - it's already been added in generateFunctionDefinitions
}

void MLIRLoweringProvider::visitLoad(ir::LoadOperation* loadOp, ValueFrame& frame) {
	auto address = resolveOperand(loadOp->getAddress(), frame);
	auto mlirLoadOp =
	    mlir::LLVM::LoadOp::create(*builder, getNameLoc("loadedValue"), getMLIRType(loadOp->getStamp()), address);
	bind(frame, loadOp, mlirLoadOp);
}

void MLIRLoweringProvider::visitConstInt(ir::ConstIntOperation* constIntOp, ValueFrame& frame) {
	bind(frame, constIntOp, getConstInt("ConstantOp", constIntOp->getStamp(), constIntOp->getValue()));
}

void MLIRLoweringProvider::visitConstPtr(ir::ConstPtrOperation* constPtr, ValueFrame& frame) {
	auto val = (int64_t) constPtr->getValue();
	auto constInt = mlir::arith::ConstantOp::create(*builder, getNameLoc("location"), builder->getI64Type(),
	                                                builder->getIntegerAttr(builder->getI64Type(), val));

	auto elementAddress = mlir::LLVM::IntToPtrOp::create(
	    *builder, getNameLoc("fieldAccess"), mlir::LLVM::LLVMPointerType::get(context), mlir::ValueRange(constInt));
	bind(frame, constPtr, elementAddress);
}

void MLIRLoweringProvider::visitConstFloat(ir::ConstFloatOperation* constFloatOp, ValueFrame& frame) {
	if (isFloat(constFloatOp->getStamp())) {
		auto floatType = (constFloatOp->getStamp() == Type::f32) ? builder->getF32Type() : builder->getF64Type();
		bind(frame, constFloatOp,
		     mlir::LLVM::ConstantOp::create(*builder, getNameLoc("constantFloat"), floatType,
		                                    builder->getFloatAttr(floatType, constFloatOp->getValue())));
	}
}

//==---------------------------==//
//==-- ARITHMETIC OPERATIONS --==//
//==---------------------------==//
void MLIRLoweringProvider::visitAdd(ir::AddOperation* addOp, ValueFrame& frame) {
	auto leftInput = resolveOperand(addOp->getLeftInput(), frame);
	auto rightInput = resolveOperand(addOp->getRightInput(), frame);
	if (addOp->getLeftInput()->getStamp() == Type::ptr) {
		// if we add something to a ptr we have to use a llvm getelementptr
		mlir::Value elementAddress =
		    mlir::LLVM::GEPOp::create(*builder, getNameLoc("fieldAccess"), mlir::LLVM::LLVMPointerType::get(context),
		                              builder->getI8Type(), leftInput, mlir::ArrayRef<mlir::Value>({rightInput}));
		bind(frame, addOp, elementAddress);
	} else if (isFloat(addOp->getStamp())) {
		auto mlirAddOp = mlir::LLVM::FAddOp::create(*builder, getNameLoc("binOpResult"), leftInput.getType(), leftInput,
		                                            rightInput, mlir::LLVM::FastmathFlags::none);
		bind(frame, addOp, mlirAddOp);
	} else {
		if (!inductionVars.contains(addOp->getLeftInput()->getIdentifier())) {
			if (!frame.contains(addOp->getIdentifier())) {
				auto mlirAddOp = mlir::LLVM::AddOp::create(*builder, getNameLoc("binOpResult"), leftInput, rightInput);
				bind(frame, addOp, mlirAddOp);
			} else {
				auto mlirAddOp = mlir::LLVM::AddOp::create(*builder, getNameLoc("binOpResult"), leftInput, rightInput);
				bind(frame, addOp, mlirAddOp);
			}
		}
	}
}

void MLIRLoweringProvider::visitSub(ir::SubOperation* subIntOp, ValueFrame& frame) {
	auto leftInput = resolveOperand(subIntOp->getLeftInput(), frame);
	auto rightInput = resolveOperand(subIntOp->getRightInput(), frame);
	if (subIntOp->getLeftInput()->getStamp() == Type::ptr) {
		// if we add something to a ptr we have to use a llvm getelementptr
		mlir::Value elementAddress =
		    mlir::LLVM::GEPOp::create(*builder, getNameLoc("fieldAccess"), mlir::LLVM::LLVMPointerType::get(context),
		                              builder->getI8Type(), leftInput, mlir::ArrayRef<mlir::Value>({rightInput}));
		bind(frame, subIntOp, elementAddress);
	} else if (isFloat(subIntOp->getStamp())) {
		auto mlirSubOp =
		    mlir::LLVM::FSubOp::create(*builder, getNameLoc("binOpResult"), leftInput, rightInput,
		                               mlir::LLVM::FastmathFlagsAttr::get(context, mlir::LLVM::FastmathFlags::none));
		bind(frame, subIntOp, mlirSubOp);
	} else {
		auto mlirSubOp = mlir::LLVM::SubOp::create(*builder, getNameLoc("binOpResult"), leftInput, rightInput);
		bind(frame, subIntOp, mlirSubOp);
	}
}

void MLIRLoweringProvider::visitMul(ir::MulOperation* mulOp, ValueFrame& frame) {
	auto leftInput = resolveOperand(mulOp->getLeftInput(), frame);
	auto rightInput = resolveOperand(mulOp->getRightInput(), frame);
	auto resultType = leftInput.getType();
	if (isFloat(mulOp->getStamp())) {
		auto mlirMulOp = mlir::LLVM::FMulOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput,
		                                            rightInput, mlir::LLVM::FastmathFlags::none);
		bind(frame, mulOp, mlirMulOp);
	} else {
		auto mlirMulOp =
		    mlir::LLVM::MulOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput, rightInput);
		bind(frame, mulOp, mlirMulOp);
	}
}

void MLIRLoweringProvider::visitDiv(ir::DivOperation* divIntOp, ValueFrame& frame) {
	auto leftInput = resolveOperand(divIntOp->getLeftInput(), frame);
	auto rightInput = resolveOperand(divIntOp->getRightInput(), frame);
	auto resultType = leftInput.getType();
	if (isFloat(divIntOp->getStamp())) {
		auto mlirDivOp = mlir::LLVM::FDivOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput,
		                                            rightInput, mlir::LLVM::FastmathFlags::none);
		bind(frame, divIntOp, mlirDivOp);
	} else {
		if (isSignedInteger(divIntOp->getStamp())) {
			auto mlirDivOp =
			    mlir::LLVM::SDivOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			bind(frame, divIntOp, mlirDivOp);
		} else {
			auto mlirDivOp =
			    mlir::LLVM::UDivOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			bind(frame, divIntOp, mlirDivOp);
		}
	}
}

void MLIRLoweringProvider::visitMod(ir::ModOperation* modIntOp, ValueFrame& frame) {
	auto leftInput = resolveOperand(modIntOp->getLeftInput(), frame);
	auto rightInput = resolveOperand(modIntOp->getRightInput(), frame);
	auto resultType = leftInput.getType();
	if (isFloat(modIntOp->getStamp())) {
		auto mlirDivOp = mlir::LLVM::FRemOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput,
		                                            rightInput, mlir::LLVM::FastmathFlags::none);
		bind(frame, modIntOp, mlirDivOp);
	} else {
		if (isSignedInteger(modIntOp->getStamp())) {
			auto mlirDivOp =
			    mlir::arith::RemSIOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			bind(frame, modIntOp, mlirDivOp);
		} else {
			auto mlirDivOp =
			    mlir::arith::RemUIOp::create(*builder, getNameLoc("binOpResult"), resultType, leftInput, rightInput);
			bind(frame, modIntOp, mlirDivOp);
		}
	}
}

void MLIRLoweringProvider::visitStore(ir::StoreOperation* storeOp, ValueFrame& frame) {
	auto value = resolveOperand(storeOp->getValue(), frame);
	auto address = resolveOperand(storeOp->getAddress(), frame);
	mlir::LLVM::StoreOp::create(*builder, getNameLoc("outputStore"), value, address);
}

void MLIRLoweringProvider::visitReturn(ir::ReturnOperation* returnOp, ValueFrame& frame) {
	// Insert return into function block.
	// Use func::ReturnOp for func::FuncOp functions (not LLVM::ReturnOp)
	if (!returnOp->hasReturnValue()) {
		mlir::func::ReturnOp::create(*builder, getNameLoc("return"), mlir::ValueRange());
	} else {
		mlir::func::ReturnOp::create(*builder, getNameLoc("return"), resolveOperand(returnOp->getReturnValue(), frame));
	}
}

void MLIRLoweringProvider::visitProxyCall(ir::ProxyCallOperation* proxyCallOp, ValueFrame& frame) {
	// First check if this is handled by an intrinsic
	if (auto intrinsic = intrinsicManager.getIntrinsic(proxyCallOp->getFunctionPtr())) {
		// Intrinsic handlers (plugins) read the call's arguments straight
		// from the block-scoped frame, so seed any cross-block operand into
		// it first -- the frame-side equivalent of resolveOperand's
		// `definedValues` fallback.
		for (auto* input : proxyCallOp->getInputs()) {
			if (input != nullptr && !frame.contains(input->getIdentifier())) {
				frame.setValue(input->getIdentifier(), resolveOperand(input, frame));
			}
		}
		const auto& intrinsicFunction = *intrinsic;
		if (intrinsicFunction(builder, proxyCallOp, frame)) {
			return;
		}
	}

	// Determine the final function name (may use pointer address for inlining
	// when a plugin installs a name-override hook; see insertExternalFunction).
	const std::string functionName = [&]() {
		if (options->getOptionOrDefault("mlir.inline_invoke_calls", false)) {
			if (const auto& hook = getLLVMBackendHooks().proxyCallNameOverride) {
				if (auto overridden = hook(proxyCallOp->getFunctionPtr())) {
					return *overridden;
				}
			}
		}
		return proxyCallOp->getFunctionName();
	}();

	// Collect function arguments
	std::vector<mlir::Value> functionArgs;
	for (const auto& arg : proxyCallOp->getInputArguments()) {
		functionArgs.push_back(resolveOperand(arg, frame));
	}

	// Try to find an existing function declaration (prefer func dialect)
	if (auto func = theModule.lookupSymbol<mlir::func::FuncOp>(functionName)) {
		// Function already exists in func dialect - use func::CallOp
		if (proxyCallOp->getStamp() != Type::v) {
			auto res = mlir::func::CallOp::create(*builder, getNameLoc("funcCall"), func, functionArgs);
			bind(frame, proxyCallOp, res.getResult(0));
		} else {
			mlir::func::CallOp::create(*builder, getNameLoc("funcCall"), func, functionArgs);
		}
		return;
	}

	// Function doesn't exist yet - create external function declaration using func dialect
	std::vector<Type> argStamps;
	argStamps.reserve(proxyCallOp->getInputArguments().size());
	for (const auto& arg : proxyCallOp->getInputArguments()) {
		argStamps.push_back(arg->getStamp());
	}
	insertExternalFunction(functionName, proxyCallOp->getFunctionPtr(), getMLIRType(proxyCallOp->getStamp()),
	                       getMLIRType(proxyCallOp->getInputArguments()), argStamps,
	                       proxyCallOp->getFunctionAttributes());

	// Now lookup the function we just created and call it using func::CallOp
	auto func = theModule.lookupSymbol<mlir::func::FuncOp>(functionName);
	if (proxyCallOp->getStamp() != Type::v) {
		auto res = mlir::func::CallOp::create(*builder, getNameLoc("funcCall"), func, functionArgs);
		bind(frame, proxyCallOp, res.getResult(0));
	} else {
		mlir::func::CallOp::create(*builder, getNameLoc("funcCall"), func, functionArgs);
	}
}

void MLIRLoweringProvider::visitIndirectCall(ir::IndirectCallOperation* indirectCallOp, ValueFrame& frame) {
	auto calleePtr = resolveOperand(indirectCallOp->getFunctionPtrOperand(), frame);
	// For indirect calls in the MLIR LLVM dialect, the callee pointer is the first element of the operands.
	std::vector<mlir::Value> allOperands;
	allOperands.push_back(calleePtr);
	std::vector<mlir::Type> argTypes;
	for (const auto& arg : indirectCallOp->getInputArguments()) {
		allOperands.push_back(resolveOperand(arg, frame));
		argTypes.push_back(getMLIRType(arg->getStamp()));
	}
	auto resultMLIRType = getMLIRType(indirectCallOp->getStamp());
	auto fnType = mlir::LLVM::LLVMFunctionType::get(resultMLIRType, argTypes);
	if (indirectCallOp->getStamp() != Type::v) {
		auto res = mlir::LLVM::CallOp::create(*builder, getNameLoc("indirectCall"), fnType, allOperands);
		bind(frame, indirectCallOp, res.getResult());
	} else {
		mlir::LLVM::CallOp::create(*builder, builder->getUnknownLoc(), fnType, allOperands);
	}
}

void MLIRLoweringProvider::visitFunctionAddressOf(ir::FunctionAddressOfOperation* funcAddrOp, ValueFrame& frame) {
	auto functionName = funcAddrOp->getFunctionName();
	auto ptrType = mlir::LLVM::LLVMPointerType::get(builder->getContext());

	// The nested function is compiled as a func::FuncOp in this module.
	// To get its address as an !llvm.ptr, we create a helper function that
	// returns the function reference via func::ConstantOp. The helper's return
	// type uses the original function type (not !llvm.ptr) so that the
	// func-to-llvm pass can cleanly convert everything — ConstantOp becomes
	// llvm.mlir.addressof and the function type becomes !llvm.ptr.
	auto helperName = "__nautilus_fptr_" + functionName;
	auto funcOp = theModule.lookupSymbol<mlir::func::FuncOp>(functionName);
	auto funcType = funcOp.getFunctionType();

	if (!theModule.lookupSymbol<mlir::func::FuncOp>(helperName)) {
		mlir::PatternRewriter::InsertionGuard insertGuard(*builder);
		builder->restoreInsertionPoint(*globalInsertPoint);

		// Create helper: func @__nautilus_fptr_add() -> (original-func-type)
		auto helperFnType = builder->getFunctionType({}, {funcType});
		auto helperFn = mlir::func::FuncOp::create(*builder, theModule.getLoc(), helperName, helperFnType);
		helperFn.setPrivate();

		auto* entryBlock = helperFn.addEntryBlock();
		builder->setInsertionPointToStart(entryBlock);

		// Get the function reference via func::ConstantOp and return it.
		auto funcConstant = mlir::func::ConstantOp::create(*builder, theModule.getLoc(), funcType, functionName);
		mlir::func::ReturnOp::create(*builder, theModule.getLoc(), funcConstant.getResult());
	}

	// Call the helper. After func-to-llvm, the return type becomes !llvm.ptr.
	auto callOp = mlir::func::CallOp::create(*builder, getNameLoc("funcAddr"), helperName, mlir::TypeRange {funcType},
	                                         mlir::ValueRange {});
	// Cast the function-typed result to !llvm.ptr for downstream use.
	auto castOp =
	    mlir::UnrealizedConversionCastOp::create(*builder, getNameLoc("funcAddr"), ptrType, callOp.getResult(0));
	bind(frame, funcAddrOp, castOp.getResult(0));
}

void MLIRLoweringProvider::visitCompare(ir::CompareOperation* compareOp, ValueFrame& frame) {
	auto leftStamp = compareOp->getLeftInput()->getStamp();
	auto rightStamp = compareOp->getRightInput()->getStamp();

	if ((isInteger(leftStamp) && isFloat(rightStamp)) || ((isInteger(rightStamp) && isFloat(leftStamp)))) {
		// Avoid comparing integer to float
		throw NotImplementedException("Type missmatch: cannot compare");
	} else if (isInteger(leftStamp) && isInteger(rightStamp)) {
		// handle integer
		auto cmpOp = mlir::arith::CmpIOp::create(
		    *builder, getNameLoc("comparison"), convertToIntMLIRComparison(compareOp->getComparator(), leftStamp),
		    resolveOperand(compareOp->getLeftInput(), frame), resolveOperand(compareOp->getRightInput(), frame));
		bind(frame, compareOp, cmpOp);
	} else if (isFloat(leftStamp) && isFloat(rightStamp)) {
		// handle float comparison
		auto cmpOp = mlir::arith::CmpFOp::create(
		    *builder, getNameLoc("comparison"), convertToFloatMLIRComparison(compareOp->getComparator()),
		    resolveOperand(compareOp->getLeftInput(), frame), resolveOperand(compareOp->getRightInput(), frame));
		bind(frame, compareOp, cmpOp);
	} else if (leftStamp == Type::b && rightStamp == Type::b) {
		// handle float comparison
		auto cmpOp = mlir::arith::CmpIOp::create(
		    *builder, getNameLoc("comparison"), convertToBooleanMLIRComparison(compareOp->getComparator()),
		    resolveOperand(compareOp->getLeftInput(), frame), resolveOperand(compareOp->getRightInput(), frame));
		bind(frame, compareOp, cmpOp);
	} else if (leftStamp == Type::ptr && rightStamp == Type::ptr) {
		// handle float comparison
		auto cmpOp = mlir::LLVM::ICmpOp::create(
		    *builder, getNameLoc("comparison"), convertToLLVMComparison(compareOp->getComparator()),
		    resolveOperand(compareOp->getLeftInput(), frame), resolveOperand(compareOp->getRightInput(), frame));
		bind(frame, compareOp, cmpOp);
	} else {
		throw NotImplementedException("Unknown type to compare");
	}
}

void MLIRLoweringProvider::visitIf(ir::IfOperation* ifOp, ValueFrame& frame) {
	auto parentBlockInsertionPoint = builder->saveInsertionPoint();

	// Capture the branch's source location BEFORE generating the
	// successor blocks — those recursions will reassign
	// `currentOpLine_` as they lower each nested op and leave it at 0
	// by the time control returns here.
	auto branchLoc = getNameLoc("branch");

	// create true block and set block arguments
	std::vector<mlir::Value> trueBlockArgs;
	mlir::Block* trueBlock = generateBasicBlock(ifOp->getTrueBlockInvocation(), frame);
	for (auto blockArg : ifOp->getTrueBlockInvocation().getArguments()) {
		trueBlockArgs.push_back(resolveOperand(blockArg, frame));
	}

	// create false block and set block arguments
	std::vector<mlir::Value> elseBlockArgs;
	mlir::Block* elseBlock = generateBasicBlock(ifOp->getFalseBlockInvocation(), frame);
	for (auto blockArg : ifOp->getFalseBlockInvocation().getArguments()) {
		elseBlockArgs.push_back(resolveOperand(blockArg, frame));
	}

	builder->restoreInsertionPoint(parentBlockInsertionPoint);
	// create cond branch operation, which evaluates the condition and branches to the true or false block
	auto mlirOp = mlir::cf::CondBranchOp::create(*builder, branchLoc, resolveOperand(ifOp->getValue(), frame),
	                                             trueBlock, trueBlockArgs, elseBlock, elseBlockArgs);

	// set the branch weights for branches by using the branch probabilities
	// The if probability indicates how likely the true branch is taken and is derived from the val<bool> condition
	// during tracing.
	// We only set weights if the probability is not neutral (0.5)
	if (ifOp->getProbability() < 0.4 || ifOp->getProbability() > 0.6) {
		::llvm::SmallVector<int32_t, 2> weights;
		weights.push_back(static_cast<int32_t>(ifOp->getProbability() * 1000));
		weights.push_back(static_cast<int32_t>((1.0 - ifOp->getProbability()) * 1000));
		mlirOp.setWeights(weights);
	}
}

void MLIRLoweringProvider::visitBranch(ir::BranchOperation* branchOp, ValueFrame& frame) {
	// Capture before recursion so the generated `cf.br` carries the
	// terminator line rather than the last nested op's line 0 reset.
	auto branchLoc = getNameLoc("branch");
	std::vector<mlir::Value> mlirTargetBlockArguments;
	for (auto targetBlockArgument : branchOp->getNextBlockInvocation().getArguments()) {
		mlirTargetBlockArguments.push_back(resolveOperand(targetBlockArgument, frame));
	}
	auto* mlirTargetBlock = generateBasicBlock(branchOp->getNextBlockInvocation(), frame);
	mlir::cf::BranchOp::create(*builder, branchLoc, mlirTargetBlock, mlirTargetBlockArguments);
}

mlir::Block* MLIRLoweringProvider::generateBasicBlock(ir::BasicBlockInvocation& blockInvocation, ValueFrame&) {
	auto targetBlock = blockInvocation.getBlock();
	// Check if the block already exists.
	if (blockMapping.contains(targetBlock->getIdentifier())) {
		return blockMapping[targetBlock->getIdentifier()];
	}

	auto parentBlockInsertionPoint = builder->saveInsertionPoint();
	// Create new block.
	auto mlirBasicBlock = builder->createBlock(builder->getBlock()->getParent());

	// Tag each block arg with a `$N` NameLoc (when debug info is on)
	// so the store at block entry below and the dbg.declare emitted by
	// EmitDbgValuePass line up with the right shadow alloca.
	auto& targetBlockArguments = targetBlock->getArguments();
	for (auto& blockArg : targetBlockArguments) {
		auto argLoc = debugInfo_.enable && irSourceMap_ != nullptr
		                  ? makeDollarLoc(blockArg->getIdentifier().getId(), "arg")
		                  : getNameLoc("arg");
		mlirBasicBlock->addArgument(getMLIRType(blockArg->getStamp()), argLoc);
	}
	ValueFrame blockFrame;
	for (uint32_t i = 0; i < targetBlockArguments.size(); i++) {
		bind(blockFrame, targetBlock->getArguments()[i], mlirBasicBlock->getArgument(i));
	}

	blockMapping[blockInvocation.getBlock()->getIdentifier()] = mlirBasicBlock;
	builder->setInsertionPointToStart(mlirBasicBlock);

	// Refresh each block arg's shadow alloca at block entry, tagged
	// with the block's header line — not the variable's original decl
	// line — so GDB advances to the block header on branch-in rather
	// than jumping back to wherever $N was first introduced.
	if (debugInfo_.enable && currentFunctionLines_ != nullptr) {
		uint32_t blockLine = 0;
		if (auto it = currentFunctionLines_->blockHeaderLines.find(targetBlock->getIdentifier().getId());
		    it != currentFunctionLines_->blockHeaderLines.end()) {
			blockLine = it->second;
		}
		auto fileAttr = builder->getStringAttr(debugInfo_.sourceFile);
		auto storeLoc = mlir::FileLineColLoc::get(fileAttr, blockLine, 1);
		for (uint32_t i = 0; i < targetBlockArguments.size(); i++) {
			const uint32_t id = targetBlockArguments[i]->getIdentifier().getId();
			storeDebugValue(id, mlirBasicBlock->getArgument(i), storeLoc);
		}
	}

	generateMLIR(targetBlock, blockFrame);
	builder->restoreInsertionPoint(parentBlockInsertionPoint);

	return mlirBasicBlock;
}

void MLIRLoweringProvider::bind(ValueFrame& frame, const ir::Operation* op, ::mlir::Value value) {
	frame.setValue(op->getIdentifier(), value);
	definedValues[op] = value;
}

::mlir::Value MLIRLoweringProvider::resolveOperand(const ir::Operation* value, ValueFrame& frame) {
	// The block-scoped binding wins: inside its owning block, an argument
	// shadows the same-named incoming definition it was named after.
	if (frame.contains(value->getIdentifier())) {
		return frame.getValue(value->getIdentifier());
	}
	// Cross-block operand: the value is defined in a dominating block, so
	// its own SSA value can be used directly and has already been recorded
	// (the DFS lowering visits a definition's block before any block that
	// uses it).
	auto it = definedValues.find(value);
	if (it == definedValues.end()) {
		throw RuntimeException("MLIRLoweringProvider: no SSA value recorded for operation " +
		                       value->getIdentifier().toString());
	}
	return it->second;
}

MLIRLoweringProvider::ValueFrame
MLIRLoweringProvider::createFrameFromParentBlock(MLIRLoweringProvider::ValueFrame& frame,
                                                 ir::BasicBlockInvocation& invocation) {
	auto invocationArguments = invocation.getArguments();
	auto& childBlockArguments = invocation.getBlock()->getArguments();
	// NES_ASSERT(invocationArguments.size() == childBlockArguments.size(),
	//            "the number of invocation parameters has to be the same as the
	//            number of block arguments in the invoked block.");
	ValueFrame childFrame;
	// Copy all frame values to the child frame that are arguments of the child
	// block.
	for (uint64_t i = 0; i < invocationArguments.size(); i++) {
		auto parentOperation = invocationArguments[i];
		auto parentValue = resolveOperand(parentOperation, frame);
		auto& childBlockArgument = childBlockArguments[i];
		bind(childFrame, childBlockArgument, parentValue);
	}
	return childFrame;
}

void MLIRLoweringProvider::visitCast(ir::CastOperation* castOperation, MLIRLoweringProvider::ValueFrame& frame) {
	auto inputStamp = castOperation->getInput()->getStamp();
	auto outputStamp = castOperation->getStamp();
	auto mlirInput = resolveOperand(castOperation->getInput(), frame);
	auto outputType = getMLIRType(outputStamp);

	if (inputStamp == outputStamp) {
		// we skip the cast if input and output is the same stamp.
		bind(frame, castOperation, mlirInput);
		return;
	}

	if (isFloat(inputStamp) && isFloat(outputStamp)) {
		if (getBitWith(inputStamp) == getBitWith(outputStamp)) {
			// we skip the cast if input and output is the same bitwith.
			bind(frame, castOperation, mlirInput);
			return;
		} else if (getBitWith(inputStamp) < getBitWith(outputStamp)) {
			// upcast
			auto mlirCast = mlir::arith::ExtFOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		} else if (getBitWith(inputStamp) > getBitWith(outputStamp)) {
			// downcast
			auto mlirCast = mlir::arith::TruncFOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		}
	} else if (isInteger(inputStamp) && isFloat(outputStamp)) {
		if (isSignedInteger(inputStamp)) {
			// cast to signed int to float
			auto mlirCast = mlir::arith::SIToFPOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		} else {
			// cast to signed int to double
			auto mlirCast = mlir::arith::UIToFPOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		}
	} else if (isFloat(inputStamp) && isInteger(outputStamp)) {
		if (isSignedInteger(outputStamp)) {
			// cast float or double to signed int
			auto mlirCast = mlir::arith::FPToSIOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		} else if (isUnsignedInteger(outputStamp)) {
			// cast float or double to unsigned int
			auto mlirCast = mlir::arith::FPToUIOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		}
	} else if (isInteger(inputStamp) && isInteger(outputStamp) && getBitWith(inputStamp) == getBitWith(outputStamp)) {
		// we skip the cast if input and output is the same stamp.
		bind(frame, castOperation, mlirInput);
		return;
	} else if (isInteger(inputStamp) && outputStamp == Type::ptr) {
		auto ptrType = mlir::LLVM::LLVMPointerType::get(context);
		auto i64Input = mlirInput;
		if (getBitWith(inputStamp) < 64) {
			auto i64Type = builder->getI64Type();
			if (isSignedInteger(inputStamp)) {
				i64Input = mlir::arith::ExtSIOp::create(*builder, getNameLoc("location"), i64Type, mlirInput);
			} else {
				i64Input = mlir::arith::ExtUIOp::create(*builder, getNameLoc("location"), i64Type, mlirInput);
			}
		}
		auto mlirCast = mlir::LLVM::IntToPtrOp::create(*builder, getNameLoc("location"), ptrType, i64Input);
		bind(frame, castOperation, mlirCast);
		return;
	} else if (inputStamp == Type::ptr && isInteger(outputStamp)) {
		auto mlirCast = mlir::LLVM::PtrToIntOp::create(*builder, getNameLoc("location"), outputType, mlirInput);
		bind(frame, castOperation, mlirCast);
		return;
	} else if (inputStamp == Type::ptr && isFloat(outputStamp)) {
		auto i64Type = builder->getI64Type();
		auto i64Value = mlir::LLVM::PtrToIntOp::create(*builder, getNameLoc("location"), i64Type, mlirInput);
		auto mlirCast = mlir::arith::UIToFPOp::create(*builder, getNameLoc("location"), outputType, i64Value);
		bind(frame, castOperation, mlirCast);
		return;
	} else if (isFloat(inputStamp) && outputStamp == Type::ptr) {
		auto i64Type = builder->getI64Type();
		auto i64Value = mlir::arith::FPToUIOp::create(*builder, getNameLoc("location"), i64Type, mlirInput).getResult();
		auto ptrType = mlir::LLVM::LLVMPointerType::get(context);
		auto mlirCast = mlir::LLVM::IntToPtrOp::create(*builder, getNameLoc("location"), ptrType, i64Value);
		bind(frame, castOperation, mlirCast);
		return;
	} else if (inputStamp == Type::ptr && outputStamp == Type::ptr) {
		bind(frame, castOperation, mlirInput);
		return;
	} else if (getBitWith(inputStamp) < getBitWith(outputStamp)) {
		// upcast
		if (isSignedInteger(inputStamp) && (isUnsignedInteger(outputStamp) || isSignedInteger(outputStamp))) {
			auto mlirCast =
			    mlir::arith::ExtSIOp::create(*builder, getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		} else if (isUnsignedInteger(inputStamp) && (isUnsignedInteger(outputStamp) || isSignedInteger(outputStamp))) {
			auto mlirCast =
			    mlir::arith::ExtUIOp::create(*builder, getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
			bind(frame, castOperation, mlirCast);
			return;
		}
	} else if (isInteger(inputStamp) && isInteger(outputStamp) && getBitWith(inputStamp) > getBitWith(outputStamp)) {
		// downcast
		auto mlirCast =
		    mlir::arith::TruncIOp::create(*builder, getNameLoc("location"), getMLIRType(outputStamp), mlirInput);
		bind(frame, castOperation, mlirCast);
		return;
	} else if (isInteger(inputStamp) && outputStamp == Type::b) {
		auto mlirZero = getConstInt("location", inputStamp, 0);
		auto mlirCmp = mlir::arith::CmpIOp::create(*builder, getNameLoc("location"), mlir::arith::CmpIPredicate::ne,
		                                           mlirInput, mlirZero);
		bind(frame, castOperation, mlirCmp);
		return;
	}

	throw NotImplementedException(
	    fmt::format("Cast from {} to {} is not supported.", toString(inputStamp), toString(outputStamp)));
}

void MLIRLoweringProvider::visitBinaryComp(ir::BinaryCompOperation* binaryCompOperation,
                                           nautilus::compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) {
	auto leftInput = resolveOperand(binaryCompOperation->getLeftInput(), frame);
	auto rightInput = resolveOperand(binaryCompOperation->getRightInput(), frame);
	mlir::Value op;
	switch (binaryCompOperation->getType()) {
	case ir::BinaryCompOperation::BAND:
		op = mlir::arith::AndIOp::create(*builder, getNameLoc("location"), leftInput, rightInput);
		break;
	case ir::BinaryCompOperation::BOR:
		op = mlir::arith::OrIOp::create(*builder, getNameLoc("location"), leftInput, rightInput);
		break;
	case ir::BinaryCompOperation::XOR:
		op = mlir::arith::XOrIOp::create(*builder, getNameLoc("location"), leftInput, rightInput);
		break;
	}
	bind(frame, binaryCompOperation, op);
}

void MLIRLoweringProvider::visitShift(ir::ShiftOperation* shiftOperation,
                                      nautilus::compiler::mlir::MLIRLoweringProvider::ValueFrame& frame) {
	auto leftInput = resolveOperand(shiftOperation->getLeftInput(), frame);
	auto rightInput = resolveOperand(shiftOperation->getRightInput(), frame);
	mlir::Value op;
	switch (shiftOperation->getType()) {
	case ir::ShiftOperation::LS:
		op = mlir::arith::ShLIOp::create(*builder, getNameLoc("location"), leftInput, rightInput);
		break;
	case ir::ShiftOperation::RS:
		if (isSignedInteger(shiftOperation->getStamp())) {
			op = mlir::arith::ShRSIOp::create(*builder, getNameLoc("location"), leftInput, rightInput);
		} else {
			op = mlir::arith::ShRUIOp::create(*builder, getNameLoc("location"), leftInput, rightInput);
		}
		break;
	}
	bind(frame, shiftOperation, op);
}

void MLIRLoweringProvider::visitAlloca(ir::AllocaOperation* allocaOperation, ValueFrame& frame) {
	// Real allocations were emitted once in generateFunction()'s prologue;
	// every per-use AllocaOperation just looks up the corresponding slot.
	auto index = allocaOperation->getIndex();
	assert(index < functionAllocaSlots_.size() && "AllocaOperation index out of range for function");
	bind(frame, allocaOperation, functionAllocaSlots_[index]);
}

void MLIRLoweringProvider::visitConstBoolean(ir::ConstBooleanOperation* constBooleanOp,
                                             MLIRLoweringProvider::ValueFrame& frame) {
	auto constOp =
	    mlir::arith::ConstantOp::create(*builder, getNameLoc("location"), builder->getI1Type(),
	                                    builder->getIntegerAttr(builder->getI1Type(), constBooleanOp->getValue()));
	bind(frame, constBooleanOp, constOp);
}

std::vector<std::string> MLIRLoweringProvider::getJitProxyFunctionSymbols() {
	return std::move(jitProxyFunctionSymbols);
}

std::vector<void*> MLIRLoweringProvider::getJitProxyTargetAddresses() {
	return std::move(jitProxyFunctionTargetAddresses);
}

} // namespace nautilus::compiler::mlir
